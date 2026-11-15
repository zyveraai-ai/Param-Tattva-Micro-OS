
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdint.h>

#include <math.h>

#include <time.h>



typedef struct { int w, h; uint8_t *rgb, *orig, *guide, *mask; } Level_t;

static uint32_t rng = 0x9E3779B9;

static inline uint32_t frand() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }



void fill_loops(uint8_t *m, int w, int h) {

    uint8_t *ext = (uint8_t*)calloc(w * h, 1);

    int *q = (int*)malloc(w * h * 4), qH = 0, qT = 0;

    for (int x = 0; x < w; x++) {

        if (!m[x]) { ext[x] = 1; q[qT++] = x; }

        int b = (h - 1) * w + x; if (!m[b]) { ext[b] = 1; q[qT++] = b; }

    }

    for (int y = 1; y < h - 1; y++) {

        int l = y * w, r = l + w - 1;

        if (!m[l]) { ext[l] = 1; q[qT++] = l; }

        if (!m[r]) { ext[r] = 1; q[qT++] = r; }

    }

    while (qH < qT) {

        int c = q[qH++], cx = c % w, cy = c / w;

        if (cx > 0 && !m[c-1] && !ext[c-1]) { ext[c-1] = 1; q[qT++] = c-1; }

        if (cx < w-1 && !m[c+1] && !ext[c+1]) { ext[c+1] = 1; q[qT++] = c+1; }

        if (cy > 0 && !m[c-w] && !ext[c-w]) { ext[c-w] = 1; q[qT++] = c-w; }

        if (cy < h-1 && !m[c+w] && !ext[c+w]) { ext[c+w] = 1; q[qT++] = c+w; }

    }

    for (int i = 0; i < w * h; i++) if (!ext[i]) m[i] = 1;

    free(ext); free(q);

}



void auto_select(uint8_t *rgb, uint8_t *m, int w, int h) {

    fill_loops(m, w, h);

    int minX = w, maxX = 0, minY = h, maxY = 0, cnt = 0;

    for (int i = 0; i < w * h; i++) if (m[i]) {

        int x = i % w, y = i / w;

        if (x < minX) minX = x; if (x > maxX) maxX = x;

        if (y < minY) minY = y; if (y > maxY) maxY = y; cnt++;

    }

    if (cnt < 20) return;

    int bg[4096] = {0}, fg[4096] = {0}, bT = 0, fT = 0;

    int x0 = minX > 24 ? minX - 24 : 0, x1 = maxX + 24 < w - 1 ? maxX + 24 : w - 1;

    int y0 = minY > 24 ? minY - 24 : 0, y1 = maxY + 24 < h - 1 ? maxY + 24 : h - 1;

    for (int y = y0; y <= y1; y++) for (int x = x0; x <= x1; x++) {

        int i = y * w + x, b = ((rgb[i*3]>>4)<<8) | ((rgb[i*3+1]>>4)<<4) | (rgb[i*3+2]>>4);

        if (!m[i]) { bg[b] += 4; bT += 4; } else { fg[b]++; fT++; }

    }

    uint8_t *snap = (uint8_t*)calloc(w * h, 1); int kept = 0;

    for (int y = minY; y <= maxY; y++) for (int x = minX; x <= maxX; x++) {

        int i = y * w + x; if (!m[i]) continue;

        int b = ((rgb[i*3]>>4)<<8) | ((rgb[i*3+1]>>4)<<4) | (rgb[i*3+2]>>4);

        int g = (x>0 && y>0) ? abs(rgb[i*3]-rgb[(i-1)*3]) + abs(rgb[i*3+1]-rgb[(i-w)*3+1]) : 0;

        if (((double)fg[b]/(fT+1)) > ((double)bg[b]/(bT+1))*1.15 || g > 35) { snap[i] = 1; kept++; }

    }

    if (kept > 40) {

        memset(m, 0, w * h);

        for (int y = 5; y < h - 5; y++) for (int x = 5; x < w - 5; x++) if (snap[y*w+x])

            for (int dy = -4; dy <= 4; dy++) for (int dx = -4; dx <= 4; dx++) m[(y+dy)*w+(x+dx)] = 1;

        fill_loops(m, w, h);

    }

    free(snap);

    printf("🎯 [v10.0 Auto-Select] Snapped to %d object pixels!\n", kept);

}



double build_guide(const uint8_t *rgb, const uint8_t *m, uint8_t *g, int w, int h, int *ry_line) {

    memcpy(g, rgb, w * h * 3);

    int minX = w, maxX = 0, minY = h, maxY = 0;

    for (int i = 0; i < w * h; i++) if (m[i]) {

        int x = i % w, y = i / w;

        if (x < minX) minX = x; if (x > maxX) maxX = x;

        if (y < minY) minY = y; if (y > maxY) maxY = y;

    }

    double slope = -0.32, bestE = 1e18;

    int midY = minY + (maxY - minY) / 2;

    for (int s = -12; s <= 2; s++) {

        double ts = s * 0.04, err = 0; int p = 0;

        for (int y = midY; y <= maxY; y += 2) for (int x = minX; x <= maxX; x++) {

            int i = y * w + x;

            if (m[i] && !m[i-1] && x > 4 && !m[i-3]) {

                for (int st = 4; st < w; st++) {

                    int rx = x - 3 + st, ry = (int)round(y + st * ts);

                    if (rx >= w - 3 || ry < 3 || ry >= h - 3) break;

                    if (!m[ry*w+rx] && !m[ry*w+rx+2]) {

                        const uint8_t *L = rgb + (i-3)*3, *R = rgb + (ry*w+rx+2)*3;

                        err += (L[0]-R[0])*(L[0]-R[0]) + (L[1]-R[1])*(L[1]-R[1]) + (L[2]-R[2])*(L[2]-R[2]);

                        p++; break;

                    }

                }

            }

        }

        if (p > 5 && err / p < bestE) { bestE = err / p; slope = ts; }

    }

    int rCurb = minY + ((maxY - minY) * 34) / 100;

    for (int x = 0; x < w; x++) ry_line[x] = (int)round(rCurb + (maxX - x) * (-slope));



    for (int y = 0; y < h; y++) for (int x = 0; x < w; x++) {

        int i = y * w + x; if (!m[i]) continue;

        int dL = 999, dR = 999, dU = 999, dUR = 999;

        const uint8_t *cL = NULL, *cR = NULL, *cU = NULL, *cUR = NULL;

        for (int st = 1; st < w; st++) {

            int lx = x - st, ly = (int)round(y - st * slope);

            if (lx < 2 || ly < 2 || ly >= h - 2) break;

            if (!m[ly*w+lx]) { cL = rgb + (ly*w+lx)*3; dL = st; break; }

        }

        for (int st = 1; st < w; st++) {

            int rx = x + st, ry = (int)round(y + st * slope);

            if (rx >= w - 2 || ry < 2 || ry >= h - 2) break;

            if (!m[ry*w+rx]) { cR = rgb + (ry*w+rx)*3; dR = st; break; }

        }

        for (int st = 1; st < h; st++) {

            int uy = y - st, ux = x + ((st*5+y)%9) - 4;

            if (ux < 2) ux = 2; if (ux >= w - 2) ux = w - 3;

            if (uy < 2) break;

            if (!m[uy*w+ux]) { cU = rgb + (uy*w+ux)*3; dU = st; break; }

        }

        for (int st = 1; st < w; st++) {

            int ux = x + st, uy = y - (int)(st * 0.45);

            if (ux >= w - 2 || uy < 2) break;

            if (!m[uy*w+ux]) { cUR = rgb + (uy*w+ux)*3; dUR = st; break; }

        }

        if (y >= ry_line[x]) {

            if (cL && cR) {

                double t = (double)dL / (dL + dR);

                for (int c=0;c<3;c++) g[i*3+c] = (uint8_t)(cL[c]*(1-t) + cR[c]*t);

            } else if (cR) memcpy(g+i*3, cR, 3);

            else if (cL) memcpy(g+i*3, cL, 3);

        } else {

            double rS=0, gS=0, bS=0, wS=0;

            if (cU) { double wU = 12000.0/((dU+6)*(dU+6)); rS+=cU[0]*wU; gS+=cU[1]*wU; bS+=cU[2]*wU; wS+=wU; }

            if (cUR){ double wR = 8000.0/((dUR+8)*(dUR+8)); rS+=cUR[0]*wR; gS+=cUR[1]*wR; bS+=cUR[2]*wR; wS+=wR; }

            if (cL && dL < 18) { double wL = 9000.0/((dL+5)*(dL+5)); rS+=cL[0]*wL; gS+=cL[1]*wL; bS+=cL[2]*wL; wS+=wL; }

            if (wS > 0) { g[i*3] = rS/wS; g[i*3+1] = gS/wS; g[i*3+2] = bS/wS; }

        }

    }

    return slope;

}



static inline float p_ssd(const Level_t *L, int ax, int ay, int bx, int by, int r, int hill) {

    int w = L->w, h = L->h; float e = 0, gw = hill ? 1.15f : 2.4f;

    for (int dy = -r; dy <= r; dy += 2) for (int dx = -r; dx <= r; dx += 2) {

        const uint8_t *p1 = L->rgb + ((ay+dy)*w + ax+dx)*3;

        const uint8_t *g1 = L->guide + ((ay+dy)*w + ax+dx)*3;

        const uint8_t *p2 = L->rgb + ((by+dy)*w + bx+dx)*3;

        e += (p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) + (p1[2]-p2[2])*(p1[2]-p2[2]);

        e += ((g1[0]-p2[0])*(g1[0]-p2[0]) + (g1[1]-p2[1])*(g1[1]-p2[1]) + (g1[2]-p2[2])*(g1[2]-p2[2])) * gw;

    }

    float dy = (float)(ay - by) / h, dx = (float)(ax - bx) / w;

    return e + dy*dy*15000.0f + dx*dx*3200.0f;

}



void solve_lvl(Level_t *L, int *nX, int *nY, int eps, const int *ry_line) {

    int w = L->w, h = L->h, r = 2;

    uint8_t *ok = (uint8_t*)calloc(w * h, 1);

    for (int y = r+1; y < h-r-1; y++) for (int x = r+1; x < w-r-1; x++) {

        uint8_t v = 1;

        for (int dy = -r; dy <= r && v; dy++) for (int dx = -r; dx <= r; dx++) if (L->mask[(y+dy)*w+x+dx]) v = 0;

        ok[y*w+x] = v;

    }

    for (int i = 0; i < w * h; i++) {

        int x = i % w, y = i / w;

        if (!L->mask[i]) { nX[i] = x; nY[i] = y; }

        else if (nX[i]<r+1 || nX[i]>=w-r-1 || nY[i]<r+1 || nY[i]>=h-r-1 || !ok[nY[i]*w+nX[i]]) {

            int hill = (y < ry_line[x]), bX = r+1, bY = r+1; float bC = 1e18f;

            for (int t = 0; t < 80; t++) {

                int rx = r+1 + (frand()%(w-2*r-2)), ry = r+1 + (frand()%(h-2*r-2));

                if (!ok[ry*w+rx] || (hill && ry >= ry_line[rx]-2) || (!hill && ry < ry_line[rx]+2)) continue;

                float c = p_ssd(L, x, y, rx, ry, r, hill);

                if (c < bC) { bC = c; bX = rx; bY = ry; }

            }

            nX[i] = bX; nY[i] = bY;

        }

    }

    float *bCost = (float*)calloc(w * h, 4);

    for (int y = r; y < h-r; y++) for (int x = r; x < w-r; x++)

        if (L->mask[y*w+x]) bCost[y*w+x] = p_ssd(L, x, y, nX[y*w+x], nY[y*w+x], r, y < ry_line[x]);



    for (int ep = 0; ep < eps; ep++) {

        int rev = ep & 1, sY = rev ? h-r-1 : r, eY = rev ? r-1 : h-r, st = rev ? -1 : 1;

        for (int y = sY; y != eY; y += st) for (int x = (rev?w-r-1:r); x != (rev?r-1:w-r); x += st) {

            int i = y * w + x; if (!L->mask[i]) continue;

            int hill = (y < ry_line[x]), cX = nX[i], cY = nY[i]; float cC = bCost[i];

            int n1 = y*w + (x-st), x1 = nX[n1]+st, y1 = nY[n1];

            if (x1>=r+1 && x1<w-r-1 && y1>=r+1 && y1<h-r-1 && ok[y1*w+x1] && ((hill && y1<ry_line[x1]) || (!hill && y1>=ry_line[x1]))) {

                float c = p_ssd(L, x, y, x1, y1, r, hill) * 0.9f; if (c < cC) { cC = c; cX = x1; cY = y1; }

            }

            int n2 = (y-st)*w + x, x2 = nX[n2], y2 = nY[n2]+st;

            if (x2>=r+1 && x2<w-r-1 && y2>=r+1 && y2<h-r-1 && ok[y2*w+x2] && ((hill && y2<ry_line[x2]) || (!hill && y2>=ry_line[x2]))) {

                float c = p_ssd(L, x, y, x2, y2, r, hill) * 0.9f; if (c < cC) { cC = c; cX = x2; cY = y2; }

            }

            for (int rad = w/3; rad >= 2; rad >>= 1) {

                int rx = cX + (int)(frand()%(2*rad+1)) - rad, ry = cY + (int)(frand()%(2*rad+1)) - rad;

                if (rx>=r+1 && rx<w-r-1 && ry>=r+1 && ry<h-r-1 && ok[ry*w+rx] && ((hill && ry<ry_line[rx]) || (!hill && ry>=ry_line[rx]))) {

                    float c = p_ssd(L, x, y, rx, ry, r, hill); if (c < cC) { cC = c; cX = rx; cY = ry; }

                }

            }

            nX[i] = cX; nY[i] = cY; bCost[i] = cC;

        }

        for (int y = r; y < h-r; y++) for (int x = r; x < w-r; x++) {

            int i = y * w + x; if (!L->mask[i]) continue;

            if (y < ry_line[x]) {

                const uint8_t *p0 = L->orig + (nY[i]*w + nX[i])*3;

                const uint8_t *pL = L->orig + (nY[i-1]*w + nX[i-1])*3;

                for (int c=0;c<3;c++) L->rgb[i*3+c] = (p0[c]*3 + pL[c]) >> 2;

            } else {

                int rS=0, gS=0, bS=0, cnt=0;

                for (int dy=-1;dy<=1;dy++) for (int dx=-1;dx<=1;dx++) {

                    int ni = (y+dy)*w + x+dx, sx = nX[ni]-dx, sy = nY[ni]-dy;

                    if (sx>=0 && sx<w && sy>=0 && sy<h && !L->mask[sy*w+sx]) {

                        const uint8_t *sp = L->orig + (sy*w+sx)*3;

                        rS += sp[0]; gS += sp[1]; bS += sp[2]; cnt++;

                    }

                }

                if (cnt > 0) {

                    L->rgb[i*3]   = ((rS/cnt) + L->guide[i*3]) >> 1;

                    L->rgb[i*3+1] = ((gS/cnt) + L->guide[i*3+1]) >> 1;

                    L->rgb[i*3+2] = ((bS/cnt) + L->guide[i*3+2]) >> 1;

                }

            }

        }

    }

    free(bCost); free(ok);

}



int main(int argc, char **argv) {

    if (argc < 3) return 1;

    FILE *fin = fopen(argv[1], "rb"); if (!fin) return 1;

    int32_t w = 0, h = 0, mode = 2;

    fread(&w, 4, 1, fin); fread(&h, 4, 1, fin); fread(&mode, 4, 1, fin);

    uint8_t *rgb = (uint8_t*)malloc(w * h * 3), *m = (uint8_t*)malloc(w * h);

    fread(rgb, 1, w * h * 3, fin); fread(m, 1, w * h, fin); fclose(fin);



    if (mode == 1) auto_select(rgb, m, w, h);

    else {

        fill_loops(m, w, h);

        uint8_t *dil = (uint8_t*)calloc(w * h, 1);

        for (int y = 5; y < h-5; y++) for (int x = 5; x < w-5; x++) if (m[y*w+x])

            for (int dy = -5; dy <= 5; dy++) for (int dx = -5; dx <= 5; dx++) if (dx*dx+dy*dy <= 25) dil[(y+dy)*w+x+dx] = 1;

        memcpy(m, dil, w * h); free(dil);



        uint8_t *orig = (uint8_t*)malloc(w * h * 3), *guide = (uint8_t*)malloc(w * h * 3);

        memcpy(orig, rgb, w * h * 3);

        int *ry = (int*)malloc(w * 4);

        double sl = build_guide(rgb, m, guide, w, h, ry);

        for (int i = 0; i < w * h; i++) if (m[i]) memcpy(rgb + i*3, guide + i*3, 3);



        Level_t L0 = { w, h, rgb, orig, guide, m };

        int *nX = (int*)malloc(w * h * 4), *nY = (int*)malloc(w * h * 4);

        for (int i = 0; i < w * h; i++) { nX[i] = 0; nY[i] = 0; }

        solve_lvl(&L0, nX, nY, 24, ry);



        free(nX); free(nY); free(orig); free(guide); free(ry);

        memset(m, 0, w * h);

        printf("⚡ [Drishti v10.0 MVP] 6-Ray Resonance Complete (Slope: %.2f)!\n", sl);

    }

    FILE *fout = fopen(argv[2], "wb");

    if (fout) { fwrite(rgb, 1, w * h * 3, fout); fwrite(m, 1, w * h, fout); fclose(fout); }

    free(rgb); free(m);

    return 0;

}

