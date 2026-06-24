/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:41:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "texture.h"
#include <math.h>
#include <stdio.h>

static t_color	img_bilinear_sample(const t_image_texture *it,
				real_t u, real_t v)
{
	int		c[4];
	real_t	f[2];
	t_color	cl[4];

	f[0] = u * (real_t)it->image.w;
	f[1] = v * (real_t)it->image.h;
	c[0] = (int)floor(f[0]) % (int)it->image.w;
	c[1] = (int)floor(f[1]) % (int)it->image.h;
	c[2] = (c[0] + 1) % (int)it->image.w;
	c[3] = (c[1] + 1) % (int)it->image.h;
	cl[0] = img_px_linear(it, c[0], c[1]);
	cl[1] = img_px_linear(it, c[2], c[1]);
	cl[2] = img_px_linear(it, c[0], c[3]);
	cl[3] = img_px_linear(it, c[2], c[3]);
	f[0] = f[0] - floor(f[0]);
	f[1] = f[1] - floor(f[1]);
	return (img_bilinear_interp(cl, f[0], f[1]));
}

t_color	image_texture_value(const t_texture *tex, real_t u, real_t v,
			const t_point3 *p)
{
	const t_image_texture	*it;

	(void)p;
	it = (const t_image_texture *)tex->data;
	if (!it || it->image.w == 0 || it->image.h == 0)
		return (vec3_create(0.0, 1.0, 1.0));
	u = u - floor(u);
	v = 1.0 - (v - floor(v));
	return (img_bilinear_sample(it, u, v));
}

void	image_texture_destroy(t_texture *tex)
{
	t_image_texture	*it;

	if (!tex || !tex->data)
		return ;
	it = (t_image_texture *)tex->data;
	lode_image_cleanup(&it->image);
	free(it);
	tex->data = NULL;
}

t_texture	*image_texture_create_png(const char *filename)
{
	t_texture		*tex;
	t_image_texture	*it;
	unsigned int	err;

	tex = (t_texture *)malloc(sizeof(t_texture));
	if (!tex)
		return (NULL);
	it = (t_image_texture *)malloc(sizeof(t_image_texture));
	if (!it)
		return (free(tex), NULL);
	lode_image_init(&it->image);
	err = lode_image_load_png(&it->image, filename);
	if (err)
	{
		fprintf(stderr, "lodepng error %u: %s (%s)\n",
			err, lodepng_error_text(err), filename);
		lode_image_cleanup(&it->image);
		free(it);
		free(tex);
		return (NULL);
	}
	tex->data = it;
	tex->value = image_texture_value;
	tex->destroy = image_texture_destroy;
	return (tex);
}
