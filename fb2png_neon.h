#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <string.h>
#include <png.h>

typedef struct {
    int fb_fd;
    int width, height, screensize;
    uint16_t *fb_ptr;
    uint8_t *rgb_buffer;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
} FBContext;

// 初期化（1回だけ）
int fb_init(FBContext *ctx, const char *fb_path);

// キャプチャしてPNG保存（何度でも呼べる）
int fb_capture_and_save(FBContext *ctx, const char *filename);

// 終了処理（最後に1回だけ）
void fb_cleanup(FBContext *ctx);
