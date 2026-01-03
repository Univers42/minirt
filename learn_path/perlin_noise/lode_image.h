#ifndef RTW_LODE_IMAGE_H
#define RTW_LODE_IMAGE_H

#ifndef LODEPNG_COMPILE_DISK
#define LODEPNG_COMPILE_DISK
#endif

#include <stdlib.h>

/* Optional project umbrella header: include only if requested or available.
   - Define USE_ALL or HAVE_ALL_H on the compiler command line to force inclusion.
   - If the compiler supports __has_include and the file exists, include it automatically.
   - Otherwise skip quietly (../all.h is optional). */
#if defined(USE_ALL) || defined(HAVE_ALL_H)
#include "../all.h"
#elif defined(__has_include)
#if __has_include("../all.h")
#include "../all.h"
#endif
#endif

/* Make lodepng optional: if the build defines USE_LODEPNG or the header is present,
   include it. Otherwise provide minimal prototypes so we don't get implicit-declaration
   warnings when lodepng isn't used. */
#if defined(USE_LODEPNG)
#include "lodepng.h"
#elif defined(__has_include)
#if __has_include("lodepng.h")
#include "lodepng.h"
#else
/* minimal lodepng prototypes used by this module */
extern void lodepng_free(void *ptr);
extern unsigned lodepng_decode32_file(unsigned char **out, unsigned *w, unsigned *h, const char *filename);
extern const char *lodepng_error_text(unsigned code);
#endif
#else
/* No __has_include: declare minimal prototypes to avoid implicit decl warnings */
extern void lodepng_free(void *ptr);
extern unsigned lodepng_decode32_file(unsigned char **out, unsigned *w, unsigned *h, const char *filename);
extern const char *lodepng_error_text(unsigned code);
#endif

#include "../error.h"

/* Owned RGBA8 image */
typedef struct s_lode_image
{
	unsigned char *rgba; /* owned */
	unsigned w;
	unsigned h;
	unsigned stride; /* bytes per row = w*4 */
} t_lode_image;

static inline void lode_image_init(t_lode_image *img)
{
	if (!img)
		return;
	img->rgba = NULL;
	img->w = 0;
	img->h = 0;
	img->stride = 0;
}

static inline void lode_image_cleanup(t_lode_image *img)
{
	if (!img)
		return;
	if (img->rgba)
		lodepng_free(img->rgba);
	lode_image_init(img);
}

static inline unsigned lode_image_load_png(t_lode_image *img, const char *filename)
{
	unsigned char *data = NULL;
	unsigned w = 0, h = 0;
	unsigned err;

	if (!img || !filename)
		return 48;

	err = lodepng_decode32_file(&data, &w, &h, filename);
	if (err)
		return err;

	lode_image_cleanup(img);
	img->rgba = data;
	img->w = w;
	img->h = h;
	img->stride = w * 4u;
	return 0;
}

/* returns pointer to RGB bytes (first 3 bytes of RGBA) */
static inline const unsigned char *lode_image_pixel_rgb(const t_lode_image *img, int x, int y)
{
	static unsigned char magenta[3] = {255, 0, 255};

	if (!img || !img->rgba || img->w == 0 || img->h == 0)
		return magenta;

	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x >= (int)img->w)
		x = (int)img->w - 1;
	if (y >= (int)img->h)
		y = (int)img->h - 1;

	return img->rgba + (size_t)y * (size_t)img->stride + (size_t)x * 4u;
}

#endif /* RTW_LODE_IMAGE_H */
