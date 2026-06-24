/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color4.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:00:53 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:27:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "color.h"
#include "studio_config.h"

/*
** color_post_process — full HDR-to-display pipeline.
** Input:  raw linear radiance values (may exceed 1.0).
** Output: display-ready [0,1] values.
**
** Pipeline order:
**   1. Exposure multiply   (RT_EXPOSURE, linear space)
**   2. ACES tone mapping   (RT_TONE_MAP, linear→[0,1])
**   3. Gamma correction    (RT_COLOR_PROFILE)
**   4. Contrast S-curve    (RT_CONTRAST)
**   5. Saturation adjust   (RT_SATURATION, BT.709 luma)
*/
void	color_post_process(real_t *r, real_t *g, real_t *b)
{
	*r *= (real_t)RT_EXPOSURE;
	*g *= (real_t)RT_EXPOSURE;
	*b *= (real_t)RT_EXPOSURE;
	apply_tonemap(r, g, b);
	apply_gamma(r, g, b);
	apply_grade(r, g, b);
}

/* Write pixel color: apply gamma, then clamp, then convert to byte */
void	write_color(FILE *out, const t_vec3 *pixel)
{
	static const t_interval	intensity = {0.000, 0.999, true};
	real_t					r;
	real_t					g;
	real_t					b;
	int						rgb[3];

	r = pixel->x;
	g = pixel->y;
	b = pixel->z;
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);
	rgb[0] = component_to_byte(r, &intensity);
	rgb[1] = component_to_byte(g, &intensity);
	rgb[2] = component_to_byte(b, &intensity);
	fprintf(out, "%d %d %d\n", rgb[0], rgb[1], rgb[2]);
}

/* simple ray_color that uses hit_sphere for legacy tests (kept) */
t_vec3	ray_color_legacy(const t_ray *r)
{
	t_vec3	center;
	real_t	t;
	t_vec3	p;
	t_vec3	n;
	t_vec3	unit_dir;

	center = vec3_create((real_t)0, (real_t)0, (real_t)-1);
	t = hit_sphere(&center, (real_t)0.5, r);
	if (t > 0.0)
	{
		p = ray_at((t_ray *)r, (real_t)t);
		p = vec3_sub(&p, &center);
		n = unit_vector(&p);
		p = vec3_create((real_t)1, (real_t)1, (real_t)1);
		n = vec3_add(&n, &p);
		return (vec3_mul_scalar(&n, (real_t)0.5));
	}
	unit_dir = unit_vector(&r->dir);
	t = 0.5 * (unit_dir.y + (real_t)1.0);
	center = vec3_create(1.0, 1.0, 1.0);
	p = vec3_create(0.5, 0.7, 1.0);
	return (vec3_lerp(&center, &p, t));
}

/* depth-limited ray-color implemented recursively.
   If material exists and scatters, use it; otherwise return black. */
t_vec3	ray_color_depth(const t_ray *r, const t_hittable_list *world, int depth)
{
	t_hit_record	rec;
	t_ray			scattered;
	t_color			attenuation;
	t_vec3			a;
	real_t			t;

	if (depth <= 0)
		return (vec3_zero());
	if (hittable_list_hit(world, r, interval((real_t)1e-4, INFINITY), &rec))
	{
		if (rec.mat && rec.mat->scatter(&(t_scatter){rec.mat, r,
				&rec, &attenuation, &scattered}))
		{
			a = ray_color_depth(&scattered, world, depth - 1);
			return (vec3_mul_elem(&attenuation, &a));
		}
		return (vec3_zero());
	}
	a = unit_vector(&r->dir);
	t = (real_t)0.5 * (a.y + (real_t)1.0);
	a = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	attenuation = vec3_create((real_t)0.3, (real_t)0.5, (real_t)1.0);
	return (vec3_lerp(&a, &attenuation, t));
}

/* Public API: keep previous signature; use moderate recursion depth (25). */
t_vec3	ray_color_world(const t_ray *r, const t_hittable_list *world)
{
	return (ray_color_depth(r, world, 25));
}
