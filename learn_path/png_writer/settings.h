/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   settings.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:26:56 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:26:56 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "types.h"

static inline void lodepng_compress_settings_init(LodePNGCompressSettings *s)
{
	s->btype = 2;
	s->use_lz77 = 1;
	s->windowsize = 2048;
	s->minmatch = 3;
	s->nicematch = 128;
	s->lazymatching = 1;
	s->custom_zlib = NULL;
	s->custom_deflate = NULL;
	s->custom_context = NULL;
}

static inline void lodepng_decompress_settings_init(LodePNGDecompressSettings *s)
{
	s->ignore_adler32 = 0;
	s->custom_zlib = NULL;
	s->custom_inflate = NULL;
	s->custom_context = NULL;
}

static inline void lodepng_color_profile_init(LodePNGColorProfile *p)
{
	p->colored = 0;
	p->alpha = 0;
	p->key = 0;
	p->key_r = p->key_g = p->key_b = 0;
	p->bits = 1;
	p->numpixels = 0;
	p->numcolors = 0;
}

static inline void lodepng_encoder_settings_init(LodePNGEncoderSettings *s)
{
	lodepng_compress_settings_init(&s->zlibsettings);
	s->filter_palette_zero = 1;
	s->filter_strategy = LFS_MINSUM;
	s->auto_convert = 1;
	s->force_palette = 0;
	s->text_compression = 1;
	s->add_id = 0;
}

#endif