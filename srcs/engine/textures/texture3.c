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

static t_color	img_bilinear_interp(t_color cl[4], real_t fx, real_t fy)
{
	t_color	dx0;
	t_color	dx1;
	t_color	c0;
	t_color	c1;
	t_color	dy;

	dx0 = vec3_sub(&cl[1], &cl[0]);
	dx1 = vec3_sub(&cl[3], &cl[2]);
	dx0 = vec3_mul_scalar(&dx0, fx);
	dx1 = vec3_mul_scalar(&dx1, fx);
	c0 = vec3_add(&cl[0], &dx0);
	c1 = vec3_add(&cl[2], &dx1);
	dy = vec3_sub(&c1, &c0);
	dy = vec3_mul_scalar(&dy, fy);
	return (vec3_add(&c0, &dy));
}

static t_color	img_bilinear_sample(const t_image_texture *it,
				real_t u, real_t v)
{
	real_t				x;
	real_t				y;
	int					c[4];
	real_t				f[2];
	const unsigned char	*px[4];
	t_color				cl[4];

	x = u * (real_t)it->image.w;
	y = v * (real_t)it->image.h;
	c[0] = (int)floor(x) % (int)it->image.w;
	c[1] = (int)floor(y) % (int)it->image.h;
	c[2] = (c[0] + 1) % (int)it->image.w;
	c[3] = (c[1] + 1) % (int)it->image.h;
	f[0] = x - floor(x);
	f[1] = y - floor(y);
	px[0] = lode_image_pixel_rgb(&it->image, c[0], c[1]);
	px[1] = lode_image_pixel_rgb(&it->image, c[2], c[1]);
	px[2] = lode_image_pixel_rgb(&it->image, c[0], c[3]);
	px[3] = lode_image_pixel_rgb(&it->image, c[2], c[3]);
	cl[0] = vec3_create(srgb_to_linear(px[0][0]),
			srgb_to_linear(px[0][1]), srgb_to_linear(px[0][2]));
	cl[1] = vec3_create(srgb_to_linear(px[1][0]),
			srgb_to_linear(px[1][1]), srgb_to_linear(px[1][2]));
	cl[2] = vec3_create(srgb_to_linear(px[2][0]),
			srgb_to_linear(px[2][1]), srgb_to_linear(px[2][2]));
	cl[3] = vec3_create(srgb_to_linear(px[3][0]),
			srgb_to_linear(px[3][1]), srgb_to_linear(px[3][2]));
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
	unsigned		err;

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
