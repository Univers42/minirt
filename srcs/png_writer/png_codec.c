/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   png_codec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 17:30:32 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 17:30:32 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "png_codec.h"

typedef struct s_ctx
{
	unsigned char			**out;
	size_t					*outsize;
	const unsigned char		*image;
	unsigned				w;
	unsigned				h;
	t_png_color_type		color_type;
	unsigned				bit_depth;
}	t_ctx;

unsigned lodepng_encode_memory(t_ctx *out)
{
	unsigned	err;

	t_
}