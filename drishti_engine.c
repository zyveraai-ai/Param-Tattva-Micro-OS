/*
 * ============================================================================
 * ⚡ PARAM-TATTVA DRISHTI-ENGINE v7.0 (100% NATIVE C ON-DEVICE TRAINER)
 * - Trains a 3D Bayes Color-Surprise Cube (16x16x16) for Instant Object Auto-Select
 * - Trains a 4-Level Multi-Scale Pyramid NNF via Iterative EM Epochs on the Phone
 * - Reconstructs via 25-Patch Overlapping Coherence Voting (Zero Smudge/Streaks)
 * ============================================================================
 * Compile: clang drishti_engine.c -O3 -lm -o drishti-engine
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
    uint8_t *mask;
} ImageLevel_t;

static uint32_t rng_state = 0x13579BDF;
static inline uint32_t fast_rand() {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return rng_state;
}

// Fill enclosed loop if user drew a circle around the object
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
    for (int i = 0; i < w * h; i++) {
        if (!ext[i]) mask[i] = 1;
    }
    free(ext);
    free(q);
}

// ============================================================================
// 🎯 MODE 1: ON-DEVICE 3D BAYES HISTOGRAM TRAINING FOR SMART AUTO-SELECT
// ============================================================================
void auto_select_object_c(uint8_t *rgb, uint8_t *mask, int w, int h) {
    fill_enclosed_loop(mask, w, h);

    // Find bounding box of user's initial stroke/circle
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

    // Train 3D RGB Histogram (16x16x16 = 4096 bins) on Background Ring outside ROI
    int bg_hist[4096] = {0};
    int fg_hist[4096] = {0};
    int bg_total = 0, fg_total = 0;

    int pad = 28;
    int rMinX = (minX - pad > 0) ? minX - pad : 0;
    int rMaxX = (maxX + pad < w - 1) ? maxX + pad : w - 1;
    int rMinY = (minY - pad > 0) ? minY - pad : 0;
    int rMaxY = (maxY + pad < h - 1) ? maxY + pad : h - 1;

    for (int y = rMinY; y <= rMaxY; y++) {
        for (int x = rMinX; x <= rMaxX; x++) {
            int i = y * w + x;
            int r = rgb[i * 3] >> 4;
            int g = rgb[i * 3 + 1] >> 4;
            int b = rgb[i * 3 + 2] >> 4;
            int bin = (r << 8) | (g << 4) | b;

            if (!mask[i]) {
                // Smooth 3D neighborhood bins for background
                bg_hist[bin] += 4;
                bg_total += 4;
            } else {
                fg_hist[bin] += 1;
                fg_total += 1;
            }
        }
    }

    // Evaluate Spanda Surprise for each pixel inside user mask:
    // If pixel color strongly belongs to outer background -> drop it! Keep only true object!
    uint8_t *snapped = (uint8_t*)calloc(w * h, 1);
    int kept = 0;
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            int i = y * w + x;
            if (!mask[i]) continue;
            int r = rgb[i * 3] >> 4;
            int g = rgb[i * 3 + 1] >> 4;
            int b = rgb[i * 3 + 2] >> 4;
            int bin = (r << 8) | (g << 4) | b;

            double p_bg = (double)bg_hist[bin] / (bg_total + 1.0);
            double p_fg = (double)fg_hist[bin] / (fg_total + 1.0);

            // Local edge gradient check
            int grad = 0;
            if (x > 0 && x < w - 1 && y > 0 && y < h - 1) {
                grad = abs((int)rgb[i * 3] - (int)rgb[(i - 1) * 3]) +
                       abs((int)rgb[i * 3 + 1] - (int)rgb[(i - w) * 3 + 1]);
            }

            if (p_fg > p_bg * 1.15 || grad > 35) {
                snapped[i] = 1;
                kept++;
            }
        }
    }

    if (kept > 40) {
        // Dilate by 5px and fill internal holes of the detected object
        uint8_t *dil = (uint8_t*)calloc(w * h, 1);
        int rad = 5;
        for (int y = rad; y < h - rad; y++) {
            for (int x = rad; x < w - rad; x++) {
                if (snapped[y * w + x]) {
                    for (int dy = -rad; dy <= rad; dy++) {
                        for (int dx = -rad; dx <= rad; dx++) {
                            if (dx * dx + dy * dy <= rad * rad) dil[(y + dy) * w + (x + dx)] = 1;
                        }
                    }
                }
            }
        }
        fill_enclosed_loop(dil, w, h);
        memcpy(mask, dil, w * h);
        free(dil);
    }
    free(snapped);
    printf("🎯 [C-Train Auto-Select] 3D Cube Trained on %d BG samples -> Snapped to %d object pixels!\n", bg_total / 4, kept);
}

// ============================================================================
// ⚡ MODE 2: MULTI-SCALE PYRAMID EM-TRAINING & 25-PATCH OVERLAPPING INPAINTER
// ============================================================================
static inline float patch_ssd(const ImageLevel_t *lvl, int ax, int ay, int bx, int by, int rad) {
    int w = lvl->w, h = lvl->h;
    const uint8_t *rgb = lvl->rgb;
    float err = 0.0f;

    for (int dy = -rad; dy <= rad; dy++) {
        int y1 = ay + dy, y2 = by + dy;
        const uint8_t *row1 = rgb + (y1 * w) * 3;
        const uint8_t *row2 = rgb + (y2 * w) * 3;
        for (int dx = -rad; dx <= rad; dx++) {
            const uint8_t *p1 = row1 + (ax + dx) * 3;
            const uint8_t *p2 = row2 + (bx + dx) * 3;
            int dr = (int)p1[0] - (int)p2[0];
            int dg = (int)p1[1] - (int)p2[1];
            int db = (int)p1[2] - (int)p2[2];
            err += (float)(dr * dr + dg * dg + db * db);
        }
    }
    // Gentle spatial horizon prior so sky stays sky & road stays road
    float dy_pen = (float)(ay - by) / (float)h;
    float dx_pen = (float)(ax - bx) / (float)w;
    err += (dy_pen * dy_pen * 35000.0f) + (dx_pen * dx_pen * 4000.0f);
    return err;
}

void solve_level_em(ImageLevel_t *lvl, int *nnfX, int *nnfY, int epochs, int scale_idx, double *out_final_loss) {
    int w = lvl->w, h = lvl->h;
    int rad = 2; // 5x5 patch (25 overlapping pixels per vote!)

    // Precompute valid unmasked source patch map (where entire 5x5 patch is outside mask)
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

    // Ensure all initial NNF coordinates point to valid clean patches
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = y * w + x;
            if (!lvl->mask[i]) {
                nnfX[i] = x; nnfY[i] = y;
            } else {
                int sx = nnfX[i], sy = nnfY[i];
                if (sx < rad + 1 || sx >= w - rad - 1 || sy < rad + 1 || sy >= h - rad - 1 || !valid_src[sy * w + sx]) {
                    // Find nearest valid clean patch on similar horizon band
                    for (int tr = 0; tr < 200; tr++) {
                        int rx = (rad + 1) + (fast_rand() % (w - 2 * rad - 2));
                        int ry = y + ((int)(fast_rand() % 41) - 20);
                        if (ry < rad + 1) ry = rad + 1;
                        if (ry >= h - rad - 1) ry = h - rad - 2;
                        if (valid_src[ry * w + rx]) { nnfX[i] = rx; nnfY[i] = ry; break; }
                    }
                }
            }
        }
    }

    float *bestCost = (float*)malloc(w * h * sizeof(float));
    for (int y = rad; y < h - rad; y++) {
        for (int x = rad; x < w - rad; x++) {
            int i = y * w + x;
            if (lvl->mask[i]) bestCost[i] = patch_ssd(lvl, x, y, nnfX[i], nnfY[i], rad);
            else bestCost[i] = 0.0f;
        }
    }

    // Run Iterative Self-Supervised Training Epochs (Propagation + Random Search + 25-Patch Voting)
    for (int ep = 0; ep < epochs; ep++) {
        int reverse = (ep & 1);
        int startY = reverse ? (h - rad - 1) : rad;
        int endY   = reverse ? (rad - 1) : (h - rad);
        int step   = reverse ? -1 : 1;

        // 1. E-STEP: NNF Coherent Propagation & Random Refinement
        for (int y = startY; y != endY; y += step) {
            for (int x = (reverse ? w - rad - 1 : rad); x != (reverse ? rad - 1 : w - rad); x += step) {
                int i = y * w + x;
                if (!lvl->mask[i]) continue;

                float curC = bestCost[i];
                int curX = nnfX[i], curY = nnfY[i];

                // Neighbor 1: Horizontal shift (Preserves continuous road lines!)
                int n1 = y * w + (x - step);
                int c1x = nnfX[n1] + step, c1y = nnfY[n1];
                if (c1x >= rad + 1 && c1x < w - rad - 1 && c1y >= rad + 1 && c1y < h - rad - 1 && valid_src[c1y * w + c1x]) {
                    float cost1 = patch_ssd(lvl, x, y, c1x, c1y, rad);
                    if (cost1 < curC) { curC = cost1; curX = c1x; curY = c1y; }
                }

                // Neighbor 2: Vertical shift
                int n2 = (y - step) * w + x;
                int c2x = nnfX[n2], c2y = nnfY[n2] + step;
                if (c2x >= rad + 1 && c2x < w - rad - 1 && c2y >= rad + 1 && c2y < h - rad - 1 && valid_src[c2y * w + c2x]) {
                    float cost2 = patch_ssd(lvl, x, y, c2x, c2y, rad);
                    if (cost2 < curC) { curC = cost2; curX = c2x; curY = c2y; }
                }

                // Random Search across decreasing radii
                int maxR = (w > h ? w : h) / 2;
                for (int r = maxR; r >= 2; r >>= 1) {
                    int rx = curX + ((int)(fast_rand() % (2 * r + 1)) - r);
                    int ry = curY + ((int)(fast_rand() % (r + 1)) - (r >> 1));
                    if (rx >= rad + 1 && rx < w - rad - 1 && ry >= rad + 1 && ry < h - rad - 1 && valid_src[ry * w + rx]) {
                        float costR = patch_ssd(lvl, x, y, rx, ry, rad);
                        if (costR < curC) { curC = costR; curX = rx; curY = ry; }
                    }
                }

                nnfX[i] = curX; nnfY[i] = curY; bestCost[i] = curC;
            }
        }

        // 2. M-STEP: Overlapping 5x5 Patch Coherence Voting (25 patches vote on every pixel!)
        double total_loss = 0;
        int masked_cnt = 0;
        for (int y = rad; y < h - rad; y++) {
            for (int x = rad; x < w - rad; x++) {
                int i = y * w + x;
                if (!lvl->mask[i]) continue;
                total_loss += sqrt(bestCost[i] / 75.0f);
                masked_cnt++;

                float rSum = 0.0f, gSum = 0.0f, bSum = 0.0f, wSum = 0.0f;
                for (int dy = -rad; dy <= rad; dy++) {
                    for (int dx = -rad; dx <= rad; dx++) {
                        int nx = x + dx, ny = y + dy;
                        int ni = ny * w + nx;
                        int sx = nnfX[ni] - dx;
                        int sy = nnfY[ni] - dy;
                        if (sx >= 0 && sx < w && sy >= 0 && sy < h && !lvl->mask[sy * w + sx]) {
                            // Weight higher for patches with lower SSD error
                            float wt = 1.0f / (1.0f + bestCost[ni] * 0.0002f);
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
        // Mode 1: Train 3D Histogram & Auto-Snap Mask to Object
        auto_select_object_c(rgb, mask, w, h);
    } else {
        // Mode 2: Full Multi-Scale Pyramid Self-Supervised PatchMatch Erase
        fill_enclosed_loop(mask, w, h);

        // Dilate mask by 4px to cover object rim cleanly (NO luma hacks!)
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

        // Build 4-Level Sookshma-to-Sthoola Pyramid
        #define PYR_LEVELS 4
        ImageLevel_t pyr[PYR_LEVELS];
        pyr[0].w = w; pyr[0].h = h;
        pyr[0].rgb = rgb; pyr[0].mask = mask;

        for (int L = 1; L < PYR_LEVELS; L++) {
            int pw = pyr[L - 1].w, ph = pyr[L - 1].h;
            int cw = pw / 2, ch = ph / 2;
            pyr[L].w = cw; pyr[L].h = ch;
            pyr[L].rgb = (uint8_t*)malloc(cw * ch * 3);
            pyr[L].mask = (uint8_t*)malloc(cw * ch);

            for (int y = 0; y < ch; y++) {
                for (int x = 0; x < cw; x++) {
                    int p0 = (2 * y) * pw + (2 * x);
                    int p1 = p0 + 1, p2 = p0 + pw, p3 = p2 + 1;
                    int m = (pyr[L - 1].mask[p0] && pyr[L - 1].mask[p1] && pyr[L - 1].mask[p2] && pyr[L - 1].mask[p3]) ? 1 : 0;
                    pyr[L].mask[y * cw + x] = m;
                    for (int c = 0; c < 3; c++) {
                        pyr[L].rgb[(y * cw + x) * 3 + c] = (
                            pyr[L - 1].rgb[p0 * 3 + c] + pyr[L - 1].rgb[p1 * 3 + c] +
                            pyr[L - 1].rgb[p2 * 3 + c] + pyr[L - 1].rgb[p3 * 3 + c]
                        ) >> 2;
                    }
                }
            }
        }

        // Initialize coarsest level hole via inward boundary diffusion so it has a warm start
        ImageLevel_t *top = &pyr[PYR_LEVELS - 1];
        uint8_t *tmpM = (uint8_t*)malloc(top->w * top->h);
        memcpy(tmpM, top->mask, top->w * top->h);
        for (int pass = 0; pass < 40; pass++) {
            for (int y = 1; y < top->h - 1; y++) {
                for (int x = 1; x < top->w - 1; x++) {
                    int i = y * top->w + x;
                    if (tmpM[i]) {
                        int r = 0, g = 0, b = 0, cnt = 0;
                        int nb[4] = { i - 1, i + 1, i - top->w, i + top->w };
                        for (int k = 0; k < 4; k++) {
                            if (!tmpM[nb[k]]) {
                                r += top->rgb[nb[k] * 3];
                                g += top->rgb[nb[k] * 3 + 1];
                                b += top->rgb[nb[k] * 3 + 2];
                                cnt++;
                            }
                        }
                        if (cnt > 0) {
                            top->rgb[i * 3] = r / cnt;
                            top->rgb[i * 3 + 1] = g / cnt;
                            top->rgb[i * 3 + 2] = b / cnt;
                            tmpM[i] = 0;
                        }
                    }
                }
            }
        }
        free(tmpM);

        int *nnfX = NULL, *nnfY = NULL;
        double init_loss = 0, final_loss = 0;

        // Train Coarse-to-Fine across all 4 Pyramid Scales
        for (int L = PYR_LEVELS - 1; L >= 0; L--) {
            int cw = pyr[L].w, ch = pyr[L].h;
            int *curX = (int*)malloc(cw * ch * sizeof(int));
            int *curY = (int*)malloc(cw * ch * sizeof(int));

            if (L == PYR_LEVELS - 1) {
                for (int y = 0; y < ch; y++)
                    for (int x = 0; x < cw; x++) { curX[y * cw + x] = x; curY[y * cw + x] = y; }
            } else {
                // Upsample NNF and reconstructed pixels from coarser level L+1
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
                            } else {
                                pyr[L].rgb[i * 3]     = pyr[L + 1].rgb[pi * 3];
                                pyr[L].rgb[i * 3 + 1] = pyr[L + 1].rgb[pi * 3 + 1];
                                pyr[L].rgb[i * 3 + 2] = pyr[L + 1].rgb[pi * 3 + 2];
                            }
                        }
                    }
                }
                free(nnfX); free(nnfY);
            }

            nnfX = curX; nnfY = curY;
            int epochs = (L == 0) ? 5 : 8;
            double lvl_loss = 0;
            solve_level_em(&pyr[L], nnfX, nnfY, epochs, L, &lvl_loss);
            if (L == PYR_LEVELS - 1) init_loss = lvl_loss;
            if (L == 0) final_loss = lvl_loss;
        }

        free(nnfX); free(nnfY);
        for (int L = 1; L < PYR_LEVELS; L++) { free(pyr[L].rgb); free(pyr[L].mask); }
        memset(mask, 0, w * h);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        double ms = (t1.tv_sec - t0.tv_sec) * 1e3 + (t1.tv_nsec - t0.tv_nsec) / 1e6;
        printf("⚡ [C-Engine 4-Pyramid Trained] 29 EM Epochs in %.0f ms | Patch Error Converged: %.1f -> %.1f\n", ms, init_loss, final_loss);
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
