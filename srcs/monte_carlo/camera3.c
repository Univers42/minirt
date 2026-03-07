/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "material.h"

static t_vec3	bg_sky_color(const t_ray *r, const t_color *background)
{
	t_vec3	unit_dir;
	real_t	a;
	t_vec3	white;

	unit_dir = unit_vector(&r->dir);
	a = (real_t)0.5 * (unit_dir.y + (real_t)1.0);
	white = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	return (vec3_lerp(&white, background, a));
}

t_vec3	ray_color_with_background(const t_ray *r,
			const t_hittable_list *world, int depth,
			const t_color *background)
{
	t_hit_record	rec;
	t_ray			scattered;
	t_color			attenuation;
	t_color			emission;
	t_vec3			scattered_col;

	if (depth <= 0)
		return (vec3_zero());
	if (!hittable_list_hit(world, r, interval((real_t)1e-4, INFINITY), &rec))
	{
		if (background->x < 0.01 && background->y < 0.01
			&& background->z < 0.01)
			return (vec3_zero());
		return (bg_sky_color(r, background));
	}
	emission = vec3_zero();
	if (rec.mat && rec.mat->emitted)
		emission = rec.mat->emitted(rec.mat, rec.u, rec.v, &rec.p);
	if (rec.mat && rec.mat->scatter(rec.mat, r, &rec, &attenuation,
			&scattered))
	{
		scattered_col = ray_color_with_background(&scattered, world,
				depth - 1, background);
		attenuation = vec3_mul_elem(&attenuation, &scattered_col);
		return (vec3_add(&emission, &attenuation));
	}
	return (emission);
}

unsigned char	*write_color_to_buf_bin(unsigned char *dst,
				const t_vec3 *pixel)
{
	real_t				r;
	real_t				g;
	real_t				b;
	static const t_interval	intensity = {0.000, 0.999, true};

	r = linear_to_gamma(pixel->x);
	g = linear_to_gamma(pixel->y);
	b = linear_to_gamma(pixel->z);
	*dst++ = (unsigned char)component_to_byte(r, &intensity);
	*dst++ = (unsigned char)component_to_byte(g, &intensity);
	*dst++ = (unsigned char)component_to_byte(b, &intensity);
	return (dst);
}

void	format_time(double seconds, char *buf, size_t bufsize)
{
	int	h;
	int	m;
	int	s;

	if (seconds < 0.0)
	{
		snprintf(buf, bufsize, "--:--:--");
		return ;
	}
	h = (int)(seconds / 3600.0);
	m = (int)((seconds - h * 3600) / 60.0);
	s = (int)(seconds - h * 3600 - m * 60);
	if (h > 0)
		snprintf(buf, bufsize, "%02d:%02d:%02d", h, m, s);
	else
		snprintf(buf, bufsize, "%02d:%02d", m, s);
}
