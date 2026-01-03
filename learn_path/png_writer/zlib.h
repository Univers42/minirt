/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zlib.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 23:25:04 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 23:25:04 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ZLIB_H
#define ZLIB_H

#include "inflate.h"
#include "deflate.h"

static unsigned zlib_decompress(unsigned char **out, size_t *outsize,
								const unsigned char *in, size_t insize,
								const LodePNGDecompressSettings *settings)
{
	unsigned error = 0;
	unsigned CMF, FLGS;
	if (insize < 2)
		return 53;
	CMF = in[0];
	FLGS = in[1];
	if ((CMF >> 4) != 8)
		return 24;
	if ((CMF & 15) > 7)
		return 25;
	if ((FLGS & 64) != 0)
		return 26;

	error = inflate(out, outsize, in + 2, insize - 2, settings);

	return error;
}

static unsigned zlib_compress(unsigned char **out, size_t *outsize,
							  const unsigned char *in, size_t insize,
							  const LodePNGCompressSettings *settings)
{
	unsigned error;
	unsigned char *deflatedata = 0;
	size_t deflatesize = 0;

	error = deflate(out, outsize, in, insize, settings);

	return error;
}

#endif