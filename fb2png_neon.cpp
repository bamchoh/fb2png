
#include "fb2png_neon.h"

#include <arm_neon.h>

void convert_rgb565_to_rgb888(const uint16_t* src, uint8_t* dst, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        uint16_t pixel = src[i];

        uint8_t r5 = (pixel >> 11) & 0x1F;
        uint8_t g6 = (pixel >> 5)  & 0x3F;
        uint8_t b5 = pixel         & 0x1F;

        // 拡張：5bit → 8bit、6bit → 8bit
        uint8_t r8 = (r5 << 3) | (r5 >> 2);
        uint8_t g8 = (g6 << 2) | (g6 >> 4);
        uint8_t b8 = (b5 << 3) | (b5 >> 2);

        dst[i * 3 + 0] = r8;
        dst[i * 3 + 1] = g8;
        dst[i * 3 + 2] = b8;
    }
}

void convert_rgb565_to_rgb888_neon(const uint16_t* src, uint8_t* dst, size_t count) {
    size_t i = 0;
    for (; i + 8 <= count; i += 8) {
        uint16x8_t pixels = vld1q_u16(&src[i]);

        // 各色をマスク＆右シフト（_n命令を使わない）
        uint16x8_t r_tmp = vandq_u16(pixels, vdupq_n_u16(0xF800));
        uint16x8_t g_tmp = vandq_u16(pixels, vdupq_n_u16(0x07E0));
        uint16x8_t b_tmp = vandq_u16(pixels, vdupq_n_u16(0x001F));

        uint16x8_t r_shifted = vshrq_n_u16(r_tmp, 11);
        uint16x8_t g_shifted = vshrq_n_u16(g_tmp, 5);

        uint8x8_t r5 = vmovn_u16(r_shifted);
        uint8x8_t g6 = vmovn_u16(g_shifted);
        uint8x8_t b5 = vmovn_u16(b_tmp);

        // 拡張処理（5/6bit → 8bit）
        uint8x8_t r8 = vorr_u8(vshl_n_u8(r5, 3), vshr_n_u8(r5, 2));
        uint8x8_t g8 = vorr_u8(vshl_n_u8(g6, 2), vshr_n_u8(g6, 4));
        uint8x8_t b8 = vorr_u8(vshl_n_u8(b5, 3), vshr_n_u8(b5, 2));

        // RGBインターリーブ形式で格納
        for (int j = 0; j < 8; ++j) {
            dst[(i + j) * 3 + 0] = r8[j];
            dst[(i + j) * 3 + 1] = g8[j];
            dst[(i + j) * 3 + 2] = b8[j];
        }
    }

    // 残りのピクセルを通常処理
    for (; i < count; ++i) {
        uint16_t pixel = src[i];
        uint8_t r5 = (pixel >> 11) & 0x1F;
        uint8_t g6 = (pixel >> 5)  & 0x3F;
        uint8_t b5 =  pixel        & 0x1F;

        dst[i * 3 + 0] = (r5 << 3) | (r5 >> 2);
        dst[i * 3 + 1] = (g6 << 2) | (g6 >> 4);
        dst[i * 3 + 2] = (b5 << 3) | (b5 >> 2);
    }
}

int fb_init(FBContext *ctx, const char *fb_path) {
    ctx->fb_fd = open(fb_path, O_RDONLY);
    if (ctx->fb_fd < 0) return -1;

    ioctl(ctx->fb_fd, FBIOGET_VSCREENINFO, &ctx->vinfo);
    ioctl(ctx->fb_fd, FBIOGET_FSCREENINFO, &ctx->finfo);

    ctx->width = ctx->vinfo.xres;
    ctx->height = ctx->vinfo.yres;
    ctx->screensize = ctx->finfo.line_length * ctx->height;

    ctx->fb_ptr = (uint16_t *)mmap(NULL, ctx->screensize, PROT_READ, MAP_SHARED, ctx->fb_fd, 0);
    if (ctx->fb_ptr == MAP_FAILED) return -2;

    ctx->rgb_buffer = (uint8_t *)malloc(ctx->width * ctx->height * 3);
    if (!ctx->rgb_buffer) return -3;

    return 0;
}

int fb_capture_and_save(FBContext *ctx, const char *filename) {
    convert_rgb565_to_rgb888(ctx->fb_ptr, ctx->rgb_buffer, ctx->width * ctx->height);

    FILE *fp = fopen(filename, "wb");
    if (!fp) return -4;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    png_infop info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, ctx->width, ctx->height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    png_set_compression_level(png_ptr, 1);
    png_set_filter(png_ptr, 0, PNG_FILTER_SUB);

    printf("png_write_row\n");
    // 行ポインタ配列をあらかじめ作る
    png_bytep row_pointers[ctx->height];
    for (int y = 0; y < ctx->height; y++) {
        row_pointers[y] = ctx->rgb_buffer + y * ctx->width * 3;
    }

    // 一括書き込み（内部でループ）
    png_write_image(png_ptr, row_pointers);

    printf("png_write_end\n");
    png_write_end(png_ptr, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
}

void fb_cleanup(FBContext *ctx) {
    munmap(ctx->fb_ptr, ctx->screensize);
    close(ctx->fb_fd);
    free(ctx->rgb_buffer);
}
