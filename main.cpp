#include "fb2png_neon.h"

int main() {
    FBContext ctx;
    if (fb_init(&ctx, "/dev/fb0") != 0) {
        fprintf(stderr, "fb_init failed\n");
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        char filename[64];
        snprintf(filename, sizeof(filename), "frame_%03d.png", i);
        printf("Saving %s\n", filename);
        fb_capture_and_save(&ctx, filename);
        usleep(100000); // 100ms 待機
    }

    fb_cleanup(&ctx);
    return 0;
}
