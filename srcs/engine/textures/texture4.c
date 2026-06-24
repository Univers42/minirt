/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture4.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:41:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "texture.h"

t_color	img_bilinear_interp(t_color cl[4], real_t fx, real_t fy)
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

t_color	img_px_linear(const t_image_texture *it, int cx, int cy)
{
	const unsigned char	*px;

	px = lode_image_pixel_rgb(&it->image, cx, cy);
	return (vec3_create(srgb_to_linear(px[0]),
			srgb_to_linear(px[1]), srgb_to_linear(px[2])));
}
