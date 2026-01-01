#include <stdio.h>
#include <stdint.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

typedef struct s_color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} t_color;

/**
 * the pixels are written out in rows
 * every rows of pixels are wirtten out from top to bottom
 * These rows are written out from top to bottom
 * by convention, each of the red/green/blue components are represented internally by real valued
 * variables that range from 0.0 to 1.0. These must be scaled to integer values between
 * 0 to 255 before we print them out.
 * Red goes from fully off (black) to fully on (bright red) from left to right
 * and green goes from fylly off at the top (black) on at the bottom (bright green). Addign red
 * and green light together yellow so we should expect the bottom right corner to be yellow
 */
int main(int argc, char **argv)
{
    int i;
    int j;
    const char *out_path = (argc > 1) ? argv[1] : "output.ppm";
    FILE *out = fopen(out_path, "w");
    if (!out)
    {
        perror("fopen");
        return 1;
    }

    /* P3 = ASCII color PPM */
    fprintf(out, "P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    for (j = 0; j < IMAGE_HEIGHT; ++j)
    {
        int remaining = IMAGE_HEIGHT - j - 1;
        if (remaining < 0)
            remaining = 0;
        fprintf(stderr, "\rScanlines remaining: %d ", remaining);
        fflush(stderr);
        for (i = 0; i < IMAGE_WIDTH; ++i)
        {
            double r = (double)i / (IMAGE_WIDTH - 1);
            double g = (double)j / (IMAGE_HEIGHT - 1);
            double b = 0.0;
            int ir = (int)(255.999 * r);
            int ig = (int)(255.999 * g);
            int ib = (int)(255.999 * b);
            fprintf(out, "%d %d %d\n", ir, ig, ib);
        }
    }
    fprintf(stderr, "\nDone.\n");
    fclose(out);
    return 0;
}