/*
 * ============================================================================
 * ⚡ PARAM-TATTVA DRISHTI-ENGINE v8.0 (2-STAGE STRUCTURE + TEXTURE C-TRAINER)
 * - Stage 1 (Rekha-Purti): Perspective Ray-Matched Structural Guide separates
 *   desert/sky zones from tilted road zones so bushes NEVER spill onto the road!
 * - Stage 2 (Varna-Purti): 4-Level Pyramid EM PatchMatch locked to Structure Guide.
 * - Auto Stray-Speck Filter: Ignores accidental tiny finger touches on screen edges.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

typedef struct {
    int w, h;
    uint8_t *rgb;
    uint8_t *guide; // Stage 1 Structural Zone Guide
    uint8_t *mask;
} ImageLevel_t;

static uint32_t rng_state = 0x2468ACE1;
static inline uint32_t fast_rand() {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return rng_state;
}

// Keep only significant mask regions (removes accidental tiny finger grazes on sky/edges!)
void filter_stray_touches(uint8_t *mask, int w, int h) {
    int *labels = (int*)calloc(w * h, sizeof(int));
    int *q = (int*)malloc(w * h * sizeof(int));
    int max_size = 0, max_label = 0, cur_label = 0;
    int sizes[512] = {0};

    for (int i = 0; i < w * h; i++) {
        if (mask[i] && !labels[i] && cur_label < 510) {
            cur_label++;
            int qH = 0, qT = 0;
            q[qT++] = i;
            labels[i] = cur_label;
            while (qH < qT) {
                int c = q[qH++];
                int cx = c % w, cy = c / w;
                int nb[4] = { c - 1, c + 1, c - w, c + w };
                if (cx > 0 && mask[nb[0]] && !labels[nb[0]]) { labels[nb[0]] = cur_label; q[qT++] = nb[0]; }
                if (cx < w - 1 && mask[nb[1]] && !labels[nb[1]]) { labels[nb[1]] = cur_label; q[qT++] = nb[1]; }
                if (cy > 0 && mask[nb[2]] && !labels[nb[2]]) { labels[nb[2]] = cur_label; q[qT++] = nb[2]; }
                if (cy < h - 1 && mask[nb[3]] && !labels[nb[3]]) { labels[nb[3]] = cur_label; q[qT++] = nb[3]; }
            }
            sizes[cur_label] = qT;
            if (qT > max_size) { max_size = qT; max_label = cur_label; }
        }
    }
    // Remove tiny accidental specks (< 15% of main painted object)
    if (max_size > 100) {
        for (int i = 0; i < w * h; i++) {
            if (mask[i] && sizes[labels[i]] < (max_size / 6)) mask[i] = 0;
        }
    }
    free(labels);
    free(q);
}

void fill_enclosed_loop(uint8_t *mask, int w, int h) {
    uint8_t *ext = (uint8_t*)calloc(w * h, 1);
    int *q = (int*)malloc(w * h * sizeof(int));
    int qH = 0, qT = 0;

    for (int x = 0; x < w; x++) {
        if (!mask[x]) { ext[x] = 1; q[qT++] = x; }
        int b = (h - 1) * w + x;
        if (!mask[b]) { ext[b] = 1; q[qT++] = b; }
    }
    for (int y = 1; y < h - 1; y++) {
        int l = y * w, r = y * w + (w - 1);
        if (!mask[l]) { ext[l] = 1; q[qT++] = l; }
        if (!mask[r]) { ext[r] = 1; q[qT++] = r; }
    }
    while (qH < qT) {
        int c = q[qH++];
        int cx = c % w, cy = c / w;
        if (cx > 0 && !mask[c - 1] && !ext[c - 1]) { ext[c - 1] = 1; q[qT++] = c - 1; }
        if (cx < w - 1 && !mask[c + 1] && !ext[c + 1]) { ext[c + 1] = 1; q[qT++] = c + 1; }
        if (cy > 0 && !mask[c - w] && !ext[c - w]) { ext[c - w] = 1; q[qT++] = c - w; }
        if (cy < h - 1 && !mask[c + w] && !ext[c + w]) { ext[c + w] = 1; q[qT++] = c + w; }
    }
    for (int i = 0; i < w * h; i++) if (!ext[i]) mask[i] = 1;
    free(ext);
    free(q);
}

// ============================================================================
// 🎯 MODE 1: 3D BAYES HISTOGRAM AUTO-SELECT
// ============================================================================
void auto_select_object_c(uint8_t *rgb, uint8_t *mask, int w, int h) {
    filter_stray_touches(mask, w, h);
    fill_enclosed_loop(mask, w, h);

    int minX = w, maxX = 0, minY = h, maxY = 0, count = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (mask[y * w + x]) {
                if (x < minX) minX = x; if (x > maxX) maxX = x;
                if (y < minY) minY = y; if (y > maxY) maxY = y;
                count++;
            }
        }
    }
    if (count < 20) return;

    int bg_hist[4096] = {0}, fg_hist[4096] = {0};
    int bg_total = 0, fg_total = 0;
    int pad = 26;
    int rMinX = (minX - pad > 0) ? minX - pad : 0;
    int rMaxX = (maxX + pad < w - 1) ? maxX + pad : w - 1;
    int rMinY = (minY - pad > 0) ? minY - pad : 0;
    int rMaxY = (maxY + pad < h - 1) ? maxY + pad : h - 1;

    for (int y = rMinY; y <= rMaxY; y++) {
        for (int x = rMinX; x <= rMaxX; x++) {
            int i = y * w + x;
            int bin = ((rgb[i * 3] >> 4) << 8) | ((rgb[i * 3 + 1] >> 4) << 4) | (rgb[i * 3 + 2] >> 4);
            if (!mask[i]) { bg_hist[bin] += 4; bg_total += 4; }
            else { fg_hist[bin] += 1; fg_total += 1; }
        }
    }

    uint8_t *snapped = (uint8_t*)calloc(w * h, 1);
    int kept = 0;
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            int i = y * w + x;
            if (!mask[i]) continue;
            int bin = ((rgb[i * 3] >> 4) << 8) | ((rgb[i * 3 + 1] >> 4) << 4) | (rgb[i * 3 + 2] >> 4);
            double p_bg = (double)bg_hist[bin] / (bg_total + 1.0);
            double p_fg = (double)fg_hist[bin] / (fg_total + 1.0);
            int grad = 0;
            if (x > 0 && x < w - 1 && y > 0 && y < h - 1) {
                grad = abs((int)rgb[i * 3] - (int)rgb[(i - 1) * 3]) +
                       abs((int)rgb[i * 3 + 1] - (int)rgb[(i - w) * 3 + 1]);
            }
            if (p_fg > p_bg * 1.15 || grad > 35) { snapped[i] = 1; kept++; }
        }
    }

    if (kept > 40) {
        uint8_t *dil = (uint8_t*)calloc(w * h, 1);
        int rad = 5;
        for (int y = rad; y < h - rad; y++) {
            for (int x = rad; x < w - rad; x++) {
                if (snapped[y * w + x]) {
                    for (int dy = -rad; dy <= rad; dy++)
                        for (int dx = -rad; dx <= rad; dx++)
                            if (dx * dx + dy * dy <= rad * rad) dil[(y + dy) * w + (x + dx)] = 1;
                }
            }
        }
        fill_enclosed_loop(dil, w, h);
        memcpy(mask, dil, w * h);
        free(dil);
    }
    free(snapped);
    printf("🎯 [C-Train Auto-Select] 3D Cube Snapped to %d object pixels!\n", kept);
}

// ============================================================================
// 📐 STAGE 1 (REKHA-PURTI): PERSPECTIVE RAY-MATCHED STRUCTURAL GUIDE
//    Finds the exact tilted road/horizon angle across the hole and builds a
//    clean structural zone map so bushes NEVER bleed down into the road!
// ============================================================================
void build_structural_guide(const uint8_t *rgb, const uint8_t *mask, uint8_t *guide, int w, int h) {
    memcpy(guide, rgb, w * h * 3);

    // 1. Find optimal perspective slope (dy/dx) that connects left boundary colors to right boundary colors
    double best_slope = -0.28;
    double min_slope_err = 1e18;

    for (int s_idx = -12; s_idx <= 4; s_idx++) {
        double test_slope = s_idx * 0.05; // Tests slopes from -0.60 to +0.20
        double err_sum = 0;
        int pairs = 0;

        for (int y = 6; y < h - 6; y += 2) {
            for (int x = 6; x < w - 6; x++) {
                int i = y * w + x;
                // Left boundary entry point into mask
                if (mask[i] && !mask[i - 1]) {
                    int lx = x - 3;
                    if (lx < 2) continue;
                    int ly = y;
                    if (mask[ly * w + lx]) continue;

                    // Trace across mask along test_slope to find right boundary exit
                    for (int step = 4; step < w; step++) {
                        int rx = lx + step;
                        int ry = (int)round(ly + step * test_slope);
                        if (rx >= w - 3 || ry < 3 || ry >= h - 3) break;
                        if (!mask[ry * w + rx] && !mask[ry * w + (rx + 2)]) {
                            const uint8_t *pL = rgb + (ly * w + lx) * 3;
                            const uint8_t *pR = rgb + (ry * w + rx + 2) * 3;
                            double dr = (double)pL[0] - pR[0];
                            double dg = (double)pL[1] - pR[1];
                            double db = (double)pL[2] - pR[2];
                            err_sum += (dr * dr + dg * dg + db * db);
                            pairs++;
                            break;
                        }
                    }
                }
            }
        }
        if (pairs > 10) {
            double avg_err = err_sum / pairs;
            if (avg_err < min_slope_err) {
                min_slope_err = avg_err;
                best_slope = test_slope;
            }
        }
    }

    // 2. Fill structural guide along the discovered perspective rays (85% weight) + vertical/horizontal continuity (15%)
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = y * w + x;
            if (!mask[i]) continue;

            double rSum = 0, gSum = 0, bSum = 0, wSum = 0;
            double slopes[2] = { best_slope, best_slope * 0.6 };
            double sWt[2]    = { 6.0, 2.0 };

            for (int si = 0; si < 2; si++) {
                double sl = slopes[si];
                // Trace Left-Down along perspective line
                int dL = 0;
                const uint8_t *cL = NULL;
                for (int st = 1; st < w; st++) {
                    int lx = x - st;
                    int ly = (int)round(y - st * sl);
                    if (lx < 1 || ly < 1 || ly >= h - 1) break;
                    if (!mask[ly * w + lx]) {
                        cL = rgb + (ly * w + lx) * 3;
                        dL = st;
                        break;
                    }
                }
                // Trace Right-Up along perspective line
                int dR = 0;
                const uint8_t *cR = NULL;
                for (int st = 1; st < w; st++) {
                    int rx = x + st;
                    int ry = (int)round(y + st * sl);
                    if (rx >= w - 1 || ry < 1 || ry >= h - 1) break;
                    if (!mask[ry * w + rx]) {
                        cR = rgb + (ry * w + rx) * 3;
                        dR = st;
                        break;
                    }
                }

                if (cL && cR) {
                    double t = (double)dL / (double)(dL + dR);
                    double r = cL[0] * (1.0 - t) + cR[0] * t;
                    double g = cL[1] * (1.0 - t) + cR[1] * t;
                    double b = cL[2] * (1.0 - t) + cR[2] * t;
                    rSum += r * sWt[si]; gSum += g * sWt[si]; bSum += b * sWt[si];
                    wSum += sWt[si];
                } else if (cR) {
                    rSum += cR[0] * sWt[si]; gSum += cR[1] * sWt[si]; bSum += cR[2] * sWt[si];
                    wSum += sWt[si];
                } else if (cL) {
                    rSum += cL[0] * sWt[si]; gSum += cL[1] * sWt[si]; bSum += cL[2] * sWt[si];
                    wSum += sWt[si];
                }
            }

            if (wSum > 0) {
                guide[i * 3]     = (uint8_t)(rSum / wSum);
                guide[i * 3 + 1] = (uint8_t)(gSum / wSum);
                guide[i * 3 + 2] = (uint8_t)(bSum / wSum);
            }
        }
    }

    // Gentle edge-preserving smoothing of the structural guide
    uint8_t *tmp = (uint8_t*)malloc(w * h * 3);
    memcpy(tmp, guide, w * h * 3);
    for (int pass = 0; pass < 4; pass++) {
        for (int y = 1; y < h - 1; y++) {
            for (int x = 1; x < w - 1; x++) {
                int i = y * w + x;
                if (!mask[i]) continue;
                for (int c = 0; c < 3; c++) {
                    tmp[i * 3 + c] = (
                        guide[(i - 1) * 3 + c] + guide[(i + 1) * 3 + c] +
                        guide[(i - w) * 3 + c] + guide[(i + w) * 3 + c]
                    ) >> 2;
                }
            }
        }
        memcpy(guide, tmp, w * h * 3);
    }
    free(tmp);
}

// ============================================================================
// ⚡ STAGE 2 (VARNA-PURTI): STRUCTURE-LOCKED PYRAMID EM PATCHMATCH
// ============================================================================
static inline float patch_ssd_guided(const ImageLevel_t *lvl, int ax, int ay, int bx, int by, int rad) {
    int w = lvl->w, h = lvl->h;
    const uint8_t *rgb = lvl->rgb;
    const uint8_t *guide = lvl->guide;
    float err = 0.0f;

    for (int dy = -rad; dy <= rad; dy++) {
        int y1 = ay + dy, y2 = by + dy;
        const uint8_t *row1  = rgb + (y1 * w) * 3;
        const uint8_t *gRow1 = guide + (y1 * w) * 3;
        const uint8_t *row2  = rgb + (y2 * w) * 3;

        for (int dx = -rad; dx <= rad; dx++) {
            const uint8_t *p1 = row1 + (ax + dx) * 3;
            const uint8_t *g1 = gRow1 + (ax + dx) * 3;
            const uint8_t *p2 = row2 + (bx + dx) * 3;

            // 1. Texture coherence cost
            int dr = (int)p1[0] - (int)p2[0];
            int dg = (int)p1[1] - (int)p2[1];
            int db = (int)p1[2] - (int)p2[2];
            err += (float)(dr * dr + dg * dg + db * db);

            // 2. REKHA-BANDHAN ZONE LOCK: Candidate patch MUST match the Structural Zone Guide!
            //    Prevents orange desert bushes from ever matching grey asphalt road zone!
            int sdr = (int)g1[0] - (int)p2[0];
            int sdg = (int)g1[1] - (int)p2[1];
            int sdb = (int)g1[2] - (int)p2[2];
            err += (float)(sdr * sdr + sdg * sdg + sdb * sdb) * 1.85f;
        }
    }

    // Proximity penalty
    float dy_pen = (float)(ay - by) / (float)h;
    float dx_pen = (float)(ax - bx) / (float)w;
    err += (dy_pen * dy_pen * 18000.0f) + (dx_pen * dx_pen * 3500.0f);
    return err;
}

void solve_level_em(ImageLevel_t *lvl, int *nnfX, int *nnfY, int epochs, double *out_final_loss) {
    int w = lvl->w, h = lvl->h;
    int rad = 2;

    uint8_t *valid_src = (uint8_t*)calloc(w * h, 1);
    for (int y = rad + 1; y < h - rad - 1; y++) {
        for (int x = rad + 1; x < w - rad - 1; x++) {
            uint8_t ok = 1;
            for (int dy = -rad; dy <= rad && ok; dy++) {
                for (int dx = -rad; dx <= rad; dx++) {
                    if (lvl->mask[(y + dy) * w + (x + dx)]) { ok = 0; break; }
                }
            }
            valid_src[y * w + x] = ok;
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = y * w + x;
            if (!lvl->mask[i]) {
                nnfX[i] = x; nnfY[i] = y;
            } else {
                int sx = nnfX[i], sy = nnfY[i];
                if (sx < rad + 1 || sx >= w - rad - 1 || sy < rad + 1 || sy >= h - rad - 1 || !valid_src[sy * w + sx]) {
                    float bestInit = 1e18f;
                    int bX = rad + 1, bY = rad + 1;
                    for (int tr = 0; tr < 80; tr++) {
                        int rx = (rad + 1) + (fast_rand() % (w - 2 * rad - 2));
                        int ry = (rad + 1) + (fast_rand() % (h - 2 * rad - 2));
                        if (valid_src[ry * w + rx]) {
                            float c = patch_ssd_guided(lvl, x, y, rx, ry, rad);
                            if (c < bestInit) { bestInit = c; bX = rx; bY = ry; }
                        }
                    }
                    nnfX[i] = bX; nnfY[i] = bY;
                }
            }
        }
    }

    float *bestCost = (float*)malloc(w * h * sizeof(float));
    for (int y = rad; y < h - rad; y++) {
        for (int x = rad; x < w - rad; x++) {
            int i = y * w + x;
            if (lvl->mask[i]) bestCost[i] = patch_ssd_guided(lvl, x, y, nnfX[i], nnfY[i], rad);
            else bestCost[i] = 0.0f;
        }
    }

    for (int ep = 0; ep < epochs; ep++) {
        int reverse = (ep & 1);
        int startY = reverse ? (h - rad - 1) : rad;
        int endY   = reverse ? (rad - 1) : (h - rad);
        int step   = reverse ? -1 : 1;

        for (int y = startY; y != endY; y += step) {
            for (int x = (reverse ? w - rad - 1 : rad); x != (reverse ? rad - 1 : w - rad); x += step) {
                int i = y * w + x;
                if (!lvl->mask[i]) continue;

                float curC = bestCost[i];
                int curX = nnfX[i], curY = nnfY[i];

                // Horizontal neighbor propagation
                int n1 = y * w + (x - step);
                int c1x = nnfX[n1] + step, c1y = nnfY[n1];
                if (c1x >= rad + 1 && c1x < w - rad - 1 && c1y >= rad + 1 && c1y < h - rad - 1 && valid_src[c1y * w + c1x]) {
                    float cost1 = patch_ssd_guided(lvl, x, y, c1x, c1y, rad);
                    if (cost1 < curC) { curC = cost1; curX = c1x; curY = c1y; }
                }

                // Vertical neighbor propagation
                int n2 = (y - step) * w + x;
                int c2x = nnfX[n2], c2y = nnfY[n2] + step;
                if (c2x >= rad + 1 && c2x < w - rad - 1 && c2y >= rad + 1 && c2y < h - rad - 1 && valid_src[c2y * w + c2x]) {
                    float cost2 = patch_ssd_guided(lvl, x, y, c2x, c2y, rad);
                    if (cost2 < curC) { curC = cost2; curX = c2x; curY = c2y; }
                }

                // Diagonal neighbor propagation (Follows angled road lines directly!)
                int n3 = (y - step) * w + (x - step);
                int c3x = nnfX[n3] + step, c3y = nnfY[n3] + step;
                if (c3x >= rad + 1 && c3x < w - rad - 1 && c3y >= rad + 1 && c3y < h - rad - 1 && valid_src[c3y * w + c3x]) {
                    float cost3 = patch_ssd_guided(lvl, x, y, c3x, c3y, rad);
                    if (cost3 < curC) { curC = cost3; curX = c3x; curY = c3y; }
                }

                // Random search
                int maxR = (w > h ? w : h) / 2;
                for (int r = maxR; r >= 2; r >>= 1) {
                    int rx = curX + ((int)(fast_rand() % (2 * r + 1)) - r);
                    int ry = curY + ((int)(fast_rand() % (2 * r + 1)) - r);
                    if (rx >= rad + 1 && rx < w - rad - 1 && ry >= rad + 1 && ry < h - rad - 1 && valid_src[ry * w + rx]) {
                        float costR = patch_ssd_guided(lvl, x, y, rx, ry, rad);
                        if (costR < curC) { curC = costR; curX = rx; curY = ry; }
                    }
                }

                nnfX[i] = curX; nnfY[i] = curY; bestCost[i] = curC;
            }
        }

        // M-Step: 25-Patch Coherence Voting
        double total_loss = 0;
        int masked_cnt = 0;
        for (int y = rad; y < h - rad; y++) {
            for (int x = rad; x < w - rad; x++) {
                int i = y * w + x;
                if (!lvl->mask[i]) continue;
                total_loss += sqrt(bestCost[i] / 150.0f);
                masked_cnt++;

                float rSum = 0.0f, gSum = 0.0f, bSum = 0.0f, wSum = 0.0f;
                for (int dy = -rad; dy <= rad; dy++) {
                    for (int dx = -rad; dx <= rad; dx++) {
                        int nx = x + dx, ny = y + dy;
                        int ni = ny * w + nx;
                        int sx = nnfX[ni] - dx;
                        int sy = nnfY[ni] - dy;
                        if (sx >= 0 && sx < w && sy >= 0 && sy < h && !lvl->mask[sy * w + sx]) {
                            float wt = 1.0f / (1.0f + bestCost[ni] * 0.00015f);
                            const uint8_t *sp = lvl->rgb + (sy * w + sx) * 3;
                            rSum += sp[0] * wt;
                            gSum += sp[1] * wt;
                            bSum += sp[2] * wt;
                            wSum += wt;
                        }
                    }
                }
                if (wSum > 0.0f) {
                    lvl->rgb[i * 3]     = (uint8_t)(rSum / wSum + 0.5f);
                    lvl->rgb[i * 3 + 1] = (uint8_t)(gSum / wSum + 0.5f);
                    lvl->rgb[i * 3 + 2] = (uint8_t)(bSum / wSum + 0.5f);
                }
            }
        }
        *out_final_loss = (masked_cnt > 0) ? (total_loss / masked_cnt) : 0.0;
    }

    free(bestCost);
    free(valid_src);
}

int main(int argc, char **argv) {
    if (argc < 3) return 1;
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    FILE *fin = fopen(argv[1], "rb");
    if (!fin) return 1;
    int32_t w = 0, h = 0, mode = 2;
    fread(&w, 4, 1, fin);
    fread(&h, 4, 1, fin);
    fread(&mode, 4, 1, fin);

    uint8_t *rgb = (uint8_t*)malloc(w * h * 3);
    uint8_t *mask = (uint8_t*)malloc(w * h);
    fread(rgb, 1, w * h * 3, fin);
    fread(mask, 1, w * h, fin);
    fclose(fin);

    if (mode == 1) {
        auto_select_object_c(rgb, mask, w, h);
    } else {
        filter_stray_touches(mask, w, h);
        fill_enclosed_loop(mask, w, h);

        // Dilate mask by 4px to cover object edges cleanly
        uint8_t *dil = (uint8_t*)calloc(w * h, 1);
        for (int y = 4; y < h - 4; y++) {
            for (int x = 4; x < w - 4; x++) {
                if (mask[y * w + x]) {
                    for (int dy = -4; dy <= 4; dy++)
                        for (int dx = -4; dx <= 4; dx++)
                            if (dx * dx + dy * dy <= 16) dil[(y + dy) * w + (x + dx)] = 1;
                }
            }
        }
        memcpy(mask, dil, w * h);
        free(dil);

        // STAGE 1: Build Rekha-Purti Structural Zone Guide at full resolution
        uint8_t *full_guide = (uint8_t*)malloc(w * h * 3);
        build_structural_guide(rgb, mask, full_guide, w, h);

        // Initialize masked pixels with structural guide (instead of blurry isotropic average!)
        for (int i = 0; i < w * h; i++) {
            if (mask[i]) {
                rgb[i * 3]     = full_guide[i * 3];
                rgb[i * 3 + 1] = full_guide[i * 3 + 1];
                rgb[i * 3 + 2] = full_guide[i * 3 + 2];
            }
        }

        // STAGE 2: Build 4-Level Pyramid with both RGB and Structural Guide
        #define PYR_LEVELS 4
        ImageLevel_t pyr[PYR_LEVELS];
        pyr[0].w = w; pyr[0].h = h;
        pyr[0].rgb = rgb; pyr[0].guide = full_guide; pyr[0].mask = mask;

        for (int L = 1; L < PYR_LEVELS; L++) {
            int pw = pyr[L - 1].w, ph = pyr[L - 1].h;
            int cw = pw / 2, ch = ph / 2;
            pyr[L].w = cw; pyr[L].h = ch;
            pyr[L].rgb   = (uint8_t*)malloc(cw * ch * 3);
            pyr[L].guide = (uint8_t*)malloc(cw * ch * 3);
            pyr[L].mask  = (uint8_t*)malloc(cw * ch);

            for (int y = 0; y < ch; y++) {
                for (int x = 0; x < cw; x++) {
                    int p0 = (2 * y) * pw + (2 * x);
                    int p1 = p0 + 1, p2 = p0 + pw, p3 = p2 + 1;
                    pyr[L].mask[y * cw + x] = (pyr[L - 1].mask[p0] || pyr[L - 1].mask[p3]) ? 1 : 0;
                    for (int c = 0; c < 3; c++) {
                        pyr[L].rgb[(y * cw + x) * 3 + c] = (
                            pyr[L - 1].rgb[p0 * 3 + c] + pyr[L - 1].rgb[p1 * 3 + c] +
                            pyr[L - 1].rgb[p2 * 3 + c] + pyr[L - 1].rgb[p3 * 3 + c]
                        ) >> 2;
                        pyr[L].guide[(y * cw + x) * 3 + c] = (
                            pyr[L - 1].guide[p0 * 3 + c] + pyr[L - 1].guide[p1 * 3 + c] +
                            pyr[L - 1].guide[p2 * 3 + c] + pyr[L - 1].guide[p3 * 3 + c]
                        ) >> 2;
                    }
                }
            }
        }

        int *nnfX = NULL, *nnfY = NULL;
        double init_loss = 0, final_loss = 0;

        for (int L = PYR_LEVELS - 1; L >= 0; L--) {
            int cw = pyr[L].w, ch = pyr[L].h;
            int *curX = (int*)malloc(cw * ch * sizeof(int));
            int *curY = (int*)malloc(cw * ch * sizeof(int));

            if (L == PYR_LEVELS - 1) {
                for (int y = 0; y < ch; y++)
                    for (int x = 0; x < cw; x++) { curX[y * cw + x] = x; curY[y * cw + x] = y; }
            } else {
                int pw = pyr[L + 1].w, ph = pyr[L + 1].h;
                for (int y = 0; y < ch; y++) {
                    for (int x = 0; x < cw; x++) {
                        int i = y * cw + x;
                        int px = (x / 2 < pw) ? x / 2 : pw - 1;
                        int py = (y / 2 < ph) ? y / 2 : ph - 1;
                        int pi = py * pw + px;
                        curX[i] = nnfX[pi] * 2 + (x & 1);
                        curY[i] = nnfY[pi] * 2 + (y & 1);
                        if (pyr[L].mask[i]) {
                            int sx = curX[i], sy = curY[i];
                            if (sx >= 0 && sx < cw && sy >= 0 && sy < ch && !pyr[L].mask[sy * cw + sx]) {
                                pyr[L].rgb[i * 3]     = pyr[L].rgb[(sy * cw + sx) * 3];
                                pyr[L].rgb[i * 3 + 1] = pyr[L].rgb[(sy * cw + sx) * 3 + 1];
                                pyr[L].rgb[i * 3 + 2] = pyr[L].rgb[(sy * cw + sx) * 3 + 2];
                            }
                        }
                    }
                }
                free(nnfX); free(nnfY);
            }

            nnfX = curX; nnfY = curY;
            int epochs = (L == 0) ? 6 : 8;
            double lvl_loss = 0;
            solve_level_em(&pyr[L], nnfX, nnfY, epochs, &lvl_loss);
            if (L == PYR_LEVELS - 1) init_loss = lvl_loss;
            if (L == 0) final_loss = lvl_loss;
        }

        free(nnfX); free(nnfY);
        for (int L = 1; L < PYR_LEVELS; L++) {
            free(pyr[L].rgb); free(pyr[L].guide); free(pyr[L].mask);
        }
        free(full_guide);
        memset(mask, 0, w * h);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double ms = (t1.tv_sec - t0.tv_sec) * 1e3 + (t1.tv_nsec - t0.tv_nsec) / 1e6;
        printf("⚡ [C-Engine v8.0 Rekha-Locked] 30 EM Epochs in %.0f ms | Loss: %.1f -> %.1f\n", ms, init_loss, final_loss);
    }

    FILE *fout = fopen(argv[2], "wb");
    if (fout) {
        fwrite(rgb, 1, w * h * 3, fout);
        fwrite(mask, 1, w * h, fout);
        fclose(fout);
    }
    free(rgb); free(mask);
    return 0;
}
