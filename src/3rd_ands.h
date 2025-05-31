/* sproutline - v0.11 - public domain sprite outline detector - http://github.org/ands/sproutline
                        no warranty implied; use at your own risk

 ============================    Contributors    =========================

 Andreas Mantler (ands)
 @r-lyeh: v0.11 removed some code, made changes and added new filters

License:
   This software is in the public domain. Where that dedication is not
   recognized, you are granted a perpetual, irrevocable license to copy
   and modify this file however you want.

*/

uint8_t* s2o_dilate(const uint8_t *data, int w, int h) {
    // original code by Andreas Mantler (@ands): Public Domain
    uint8_t* result = (uint8_t*)malloc(w * h);
    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {
            result[y * w + x] = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int cx = x + dx;
                    int cy = y + dy;
                    if (data[cy * w + cx]) {
                        result[y * w + x] = !0;
                        dy = 1;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

uint8_t* s2o_outline(const uint8_t *data, int w, int h) {
    // original code by Andreas Mantler (@ands): Public Domain
    uint8_t* result = (uint8_t*)malloc(w * h);
    for (int x = 0; x < w; x++) {
        result[x] = data[x];
        result[(h - 1) * w + x] = data[(h - 1) * w + x];
    }
    for (int y = 1; y < h - 1; y++) {
        result[y * w] = data[y * w];
        for (int x = 1; x < w - 1; x++) {
            if (data[y * w + x] &&
                (
                    !data[y * w + x - 1] ||
                    !data[y * w + x + 1] ||
                    !data[y * w + x - w] ||
                    !data[y * w + x + w]
                )) {
                result[y * w + x] = !0;
            }
            else {
                result[y * w + x] = 0;
            }
        }
        result[y * w + w - 1] = data[y * w + w - 1];
    }
    return result;
}

uint8_t* s2o_floodfill(uint8_t *bitmap, int w, int h) {
    int size = w * h;
    bool *visited = calloc(size, sizeof(bool));
    if (!visited) return bitmap;

    int *qx = malloc(size * sizeof(int));
    int *qy = malloc(size * sizeof(int));
    if (!qx || !qy) {
        free(visited);
        return bitmap;
    }

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    // Pass 1: Flood-fill background-connected black pixels from borders
    int front = 0, back = 0;
    for (int x = 0; x < w; ++x) {
        int top = x;
        int bottom = (h - 1) * w + x;
        if (bitmap[top] == 0 && !visited[top]) {
            visited[top] = true; qx[back] = x; qy[back++] = 0;
        }
        if (bitmap[bottom] == 0 && !visited[bottom]) {
            visited[bottom] = true; qx[back] = x; qy[back++] = h - 1;
        }
    }
    for (int y = 0; y < h; ++y) {
        int left = y * w;
        int right = y * w + (w - 1);
        if (bitmap[left] == 0 && !visited[left]) {
            visited[left] = true; qx[back] = 0; qy[back++] = y;
        }
        if (bitmap[right] == 0 && !visited[right]) {
            visited[right] = true; qx[back] = w - 1; qy[back++] = y;
        }
    }

    // Flood-fill background black
    while (front < back) {
        int x = qx[front];
        int y = qy[front++];
        int i = y * w + x;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
            int ni = ny * w + nx;
            if (bitmap[ni] == 0 && !visited[ni]) {
                visited[ni] = true;
                qx[back] = nx; qy[back++] = ny;
            }
        }
    }

    // Pass 2: Fill any black pixel not connected to background — enclosed regions
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int i = y * w + x;
            if (bitmap[i] == 0 && !visited[i]) {
                // Flood-fill this enclosed black region to white
                int front2 = 0, back2 = 0;
                qx[back2] = x; qy[back2++] = y;
                visited[i] = true;

                while (front2 < back2) {
                    int cx = qx[front2];
                    int cy = qy[front2++];
                    int ci = cy * w + cx;

                    bitmap[ci] = 1; // fill to white

                    for (int d = 0; d < 4; ++d) {
                        int nx = cx + dx[d], ny = cy + dy[d];
                        if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
                        int ni = ny * w + nx;
                        if (bitmap[ni] == 0 && !visited[ni]) {
                            visited[ni] = true;
                            qx[back2] = nx; qy[back2++] = ny;
                        }
                    }
                }
            }
        }
    }

    free(qx); free(qy); free(visited);
    return bitmap;
}

uint8_t* s2o_erode(const uint8_t *data, int w, int h) {
    uint8_t* result = (uint8_t*)malloc(w * h);
    if (!result) return NULL;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int i = y * w + x;

            if (data[i] == 0) {
                result[i] = 0; // already black, stay black
                continue;
            }

            // Check 8 neighbors
            int keep = 1;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue; // skip center
                    int ni = (y + dy) * w + (x + dx);
                    if (data[ni] == 0) {
                        keep = 0;
                        goto exit_neighbors;
                    }
                }
            }
        exit_neighbors:
            result[i] = keep ? 1 : 0;
        }
    }

    // Optionally: copy border unchanged
    for (int x = 0; x < w; x++) {
        result[x] = 0;
        result[(h - 1) * w + x] = 0;
    }
    for (int y = 0; y < h; y++) {
        result[y * w] = 0;
        result[y * w + (w - 1)] = 0;
    }

    return result;
}

uint8_t* fill_closed_outline_shapes(uint8_t *bitmap, int w, int h) {
    int size = w * h;
    bool *visited = calloc(size, sizeof(bool));
    if (!visited) return bitmap;

    int *qx = malloc(size * sizeof(int));
    int *qy = malloc(size * sizeof(int));
    if (!qx || !qy) {
        free(visited); return bitmap;
    }

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    // Step 1: Flood-fill all background-connected black pixels
    int front = 0, back = 0;

    for (int x = 0; x < w; x++) {
        int i_top = x;
        int i_bot = (h - 1) * w + x;
        if (bitmap[i_top] == 0 && !visited[i_top]) {
            visited[i_top] = true; qx[back] = x; qy[back++] = 0;
        }
        if (bitmap[i_bot] == 0 && !visited[i_bot]) {
            visited[i_bot] = true; qx[back] = x; qy[back++] = h - 1;
        }
    }
    for (int y = 0; y < h; y++) {
        int i_left = y * w;
        int i_right = y * w + (w - 1);
        if (bitmap[i_left] == 0 && !visited[i_left]) {
            visited[i_left] = true; qx[back] = 0; qy[back++] = y;
        }
        if (bitmap[i_right] == 0 && !visited[i_right]) {
            visited[i_right] = true; qx[back] = w - 1; qy[back++] = y;
        }
    }

    while (front < back) {
        int x = qx[front];
        int y = qy[front++];
        int i = y * w + x;

        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d], ny = y + dy[d];
            if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
            int ni = ny * w + nx;
            if (bitmap[ni] == 0 && !visited[ni]) {
                visited[ni] = true;
                qx[back] = nx; qy[back++] = ny;
            }
        }
    }

    // Step 2: Any unvisited black pixels are enclosed — fill them white
    for (int i = 0; i < size; i++) {
        if (bitmap[i] == 0 && !visited[i]) {
            bitmap[i] = 1;
        }
    }

    free(qx); free(qy); free(visited);
    return bitmap;
}

uint8_t* s2o_safe_dilate_interior_white(const uint8_t *data, int w, int h) {
    uint8_t* result = (uint8_t*)malloc(w * h);
    if (!result) return NULL;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int i = y * w + x;
            if (data[i] == 1) {
                result[i] = 1;
                continue;
            }

            // Check for white neighbors (so we can "spread" white into tiny gaps)
            int filled = 0;
            for (int dy = -1; dy <= 1 && !filled; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int ni = (y + dy) * w + (x + dx);
                    if (data[ni] == 1) {
                        filled = 1;
                        break;
                    }
                }
            }
            result[i] = filled ? 1 : 0;
        }
    }

    return result;
}

uint8_t* s2o_morph_dilate_white(const uint8_t *data, int w, int h) {
    uint8_t* result = (uint8_t*)malloc(w * h);
    if (!result) return NULL;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int i = y * w + x;
            result[i] = data[i]; // default: keep current

            if (data[i] == 0) {
                // if any neighbor is white, set to white
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int ni = (y + dy) * w + (x + dx);
                        if (data[ni] == 1) {
                            result[i] = 1;
                            goto done;
                        }
                    }
                }
            }
        done:;
        }
    }

    return result;
}

uint8_t* s2o_morph_erode_white(const uint8_t *data, int w, int h) {
    uint8_t* result = (uint8_t*)malloc(w * h);
    if (!result) return NULL;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int i = y * w + x;
            result[i] = 0;

            if (data[i] == 1) {
                // only preserve if all neighbors are white
                int all_white = 1;
                for (int dy = -1; dy <= 1 && all_white; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int ni = (y + dy) * w + (x + dx);
                        if (data[ni] == 0) {
                            all_white = 0;
                            break;
                        }
                    }
                }
                if (all_white) result[i] = 1;
            }
        }
    }

    return result;
}

uint8_t* s2o_morph_close_white(const uint8_t *data, int w, int h) {
    uint8_t* dilated = s2o_morph_dilate_white(data, w, h);
    uint8_t* closed = s2o_morph_erode_white(dilated, w, h);
    free(dilated);
    return closed;
}

uint8_t* fill_enclosed_black_regions(uint8_t *bitmap, int w, int h) {
    int size = w * h;
    bool *visited = calloc(size, sizeof(bool));
    if (!visited) return bitmap;

    int *qx = malloc(size * sizeof(int));
    int *qy = malloc(size * sizeof(int));
    if (!qx || !qy) {
        free(visited); return bitmap;
    }

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    // Step 1: Mark all background-connected black pixels
    int front = 0, back = 0;

    for (int x = 0; x < w; x++) {
        int top = x;
        int bottom = (h - 1) * w + x;
        if (bitmap[top] == 0 && !visited[top]) {
            visited[top] = true; qx[back] = x; qy[back++] = 0;
        }
        if (bitmap[bottom] == 0 && !visited[bottom]) {
            visited[bottom] = true; qx[back] = x; qy[back++] = h - 1;
        }
    }
    for (int y = 0; y < h; y++) {
        int left = y * w;
        int right = y * w + (w - 1);
        if (bitmap[left] == 0 && !visited[left]) {
            visited[left] = true; qx[back] = 0; qy[back++] = y;
        }
        if (bitmap[right] == 0 && !visited[right]) {
            visited[right] = true; qx[back] = w - 1; qy[back++] = y;
        }
    }

    while (front < back) {
        int x = qx[front];
        int y = qy[front++];
        int i = y * w + x;

        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
            int ni = ny * w + nx;
            if (bitmap[ni] == 0 && !visited[ni]) {
                visited[ni] = true;
                qx[back] = nx; qy[back++] = ny;
            }
        }
    }

    // Step 2: Fill all unvisited black pixels — they’re enclosed holes
    for (int i = 0; i < size; i++) {
        if (bitmap[i] == 0 && !visited[i]) {
            bitmap[i] = 1; // fill it
        }
    }

    free(qx); free(qy); free(visited);
    return bitmap;
}
