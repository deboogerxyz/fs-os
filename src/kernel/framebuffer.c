
#include <kernel/framebuffer.h>

/* Framebuffer globals */
static uint32_t* g_fb;
static uint32_t g_pitch;
static uint32_t g_width;
static uint32_t g_height;
static uint32_t g_bpp;

void fb_init(uint32_t* fb, uint32_t pitch, uint32_t w, uint32_t h,
             uint32_t bpp) {
    /* Set globals to the parameter values we received from main (multiboot
     * info) */
    g_fb     = fb;
    g_pitch  = pitch;
    g_width  = w;
    g_height = h;
    g_bpp    = bpp;

    for (uint32_t y = 0; y < h; y++) {
        for (uint32_t x = 0; x < w; x++) {
            g_fb[y * w + x] = 0x000000;
        }
    }
}

uint32_t* fb_get_ptr(void) {
    return g_fb;
}

uint32_t fb_get_width(void) {
    return g_width;
}

uint32_t fb_get_height(void) {
    return g_height;
}

void fb_setpx_col(uint32_t y, uint32_t x, uint32_t col) {
    if (y >= g_height || x >= g_width)
        return;

    g_fb[y * g_width + x] = col;
}

void fb_drawrect_col(uint32_t y, uint32_t x, uint32_t h, uint32_t w,
                     uint32_t col) {
    if (y >= g_height || x >= g_width)
        return;

    /* If width or height go out of the screen, set them to the screen limit */
    if (y + h >= g_height)
        h = g_height - y - 1;
    if (x + w >= g_width)
        w = g_width - x - 1;

    for (uint32_t cur_y = y; cur_y < y + h; cur_y++)
        for (uint32_t cur_x = x; cur_x < x + w; cur_x++)
            g_fb[cur_y * g_width + cur_x] = col;
}

void fb_drawrect_fast(uint32_t y, uint32_t x, uint32_t h, uint32_t w,
                      uint32_t col) {
    for (uint32_t cur_y = y; cur_y < y + h; cur_y++)
        for (uint32_t cur_x = x; cur_x < x + w; cur_x++)
            g_fb[cur_y * g_width + cur_x] = col;
}

