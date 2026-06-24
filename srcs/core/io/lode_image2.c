/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lode_image2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 07:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 07:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Native PNG output (the vendored encoder was left unimplemented/broken, so
** we write the PNG directly with zlib — explicitly allowed by the subject for
** image formats).  A truecolour 8-bit PNG is signature + IHDR + IDAT(zlib of
** the no-filter scanlines) + IEND, each chunk length-prefixed and CRC-suffixed.
*/

#include "lode_image.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* zlib, declared locally: the build's -I path carries the vendored
   png_writer/zlib.h which would shadow the real system <zlib.h>. */
#define Z_OK 0

extern unsigned long	crc32(unsigned long crc, const unsigned char *buf,
							unsigned int len);
extern int				compress(unsigned char *dst, unsigned long *dlen,
							const unsigned char *src, unsigned long slen);
extern unsigned long	compressBound(unsigned long slen);

static void	png_u32(unsigned char *p, unsigned int v)
{
	p[0] = (unsigned char)(v >> 24);
	p[1] = (unsigned char)(v >> 16);
	p[2] = (unsigned char)(v >> 8);
	p[3] = (unsigned char)v;
}

static void	png_chunk(FILE *f, const char *type, const unsigned char *data,
				unsigned int len)
{
	unsigned char	buf[8];
	unsigned long	crc;

	png_u32(buf, len);
	memcpy(buf + 4, type, 4);
	fwrite(buf, 1, 8, f);
	if (len)
		fwrite(data, 1, len, f);
	crc = crc32(0L, (const unsigned char *)type, 4);
	if (len)
		crc = crc32(crc, data, len);
	png_u32(buf, (unsigned int)crc);
	fwrite(buf, 1, 4, f);
}

/* Pack scanlines, each with a leading 0 ("none") filter byte — PNG raw form. */
static unsigned char	*png_filtered(const unsigned char *rgb, int w, int h,
							size_t *flen)
{
	unsigned char	*out;
	size_t			row;
	int				y;

	row = (size_t)w * 3;
	*flen = (row + 1) * (size_t)h;
	out = malloc(*flen);
	if (!out)
		return (NULL);
	y = 0;
	while (y < h)
	{
		out[(size_t)y * (row + 1)] = 0;
		memcpy(out + (size_t)y * (row + 1) + 1, rgb + (size_t)y * row, row);
		y++;
	}
	return (out);
}

static void	png_header(FILE *f, int w, int h)
{
	unsigned char				ihdr[13];
	static const unsigned char	sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};

	fwrite(sig, 1, 8, f);
	png_u32(ihdr, (unsigned int)w);
	png_u32(ihdr + 4, (unsigned int)h);
	ihdr[8] = 8;
	ihdr[9] = 2;
	ihdr[10] = 0;
	ihdr[11] = 0;
	ihdr[12] = 0;
	png_chunk(f, "IHDR", ihdr, 13);
}

unsigned int	lode_image_save_png(const char *path,
					const unsigned char *rgb, int w, int h)
{
	FILE			*f;
	unsigned char	*filt;
	unsigned char	*comp;
	size_t			flen;
	unsigned long	clen;

	f = fopen(path, "wb");
	filt = png_filtered(rgb, w, h, &flen);
	if (!f || !filt)
		return (free(filt), (void)(f && fclose(f)), 1);
	clen = compressBound((unsigned long)flen);
	comp = malloc(clen);
	if (!comp || compress(comp, &clen, filt, (unsigned long)flen) != Z_OK)
		return (free(filt), free(comp), fclose(f), 1);
	png_header(f, w, h);
	png_chunk(f, "IDAT", comp, (unsigned int)clen);
	png_chunk(f, "IEND", NULL, 0);
	return (free(filt), free(comp), fclose(f), 0);
}
