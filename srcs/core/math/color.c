/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:00:53 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 20:27:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "color.h"
#include "studio_config.h"

t_vec3	color_create(real_t x, real_t y, real_t z)
{
	return (vec3_create(x, y, z));
}

real_t	vec3_max_component(const t_vec3 *v)
{
	real_t	m;

	m = v->x;
	if (v->y > m)
		m = v->y;
	if (v->z > m)
		m = v->z;
	return (m);
}

/*
** linear_to_gamma — profile-aware gamma transfer function.
** Selected at compile time by RT_COLOR_PROFILE (see settings.h).
**   CIE     : pow(v, 1/2.2)  — deepest blacks, best perceptual contrast
**   sRGB    : IEC 61966-2-1 piecewise (linear toe + pow 1/2.4)
**   GAMMA20 : sqrt(v)        — book default
*/
real_t	linear_to_gamma(real_t v)
{
	if (v <= (real_t)0.0)
		return ((real_t)0.0);
#if RT_COLOR_PROFILE == RT_COLOR_SRGB
	if (v <= (real_t)0.0031308)
		return ((real_t)(12.92 * v));
	return ((real_t)(1.055 * pow((double)v, 1.0 / 2.4) - 0.055));
#elif RT_COLOR_PROFILE == RT_COLOR_GAMMA20
	return ((real_t)sqrt((double)v));
#else
	return ((real_t)pow((double)v, 1.0 / 2.2));
#endif
}

/*
** aces_tonemap — Narkowicz 2015 ACES filmic tone-mapping curve.
** Maps HDR linear values to [0,1] display range with graceful
** highlight rolloff and preserved shadow detail.
** Formula: (v*(2.51v+0.03)) / (v*(2.43v+0.59)+0.14)
*/
static real_t	aces_tonemap(real_t v)
{
	real_t	num;
	real_t	den;

	if (v < (real_t)0.0)
		v = (real_t)0.0;
	num = v * ((real_t)2.51 * v + (real_t)0.03);
	den = v * ((real_t)2.43 * v + (real_t)0.59) + (real_t)0.14;
	v = num / den;
	if (v > (real_t)1.0)
		v = (real_t)1.0;
	return (v);
}

/* Convert a [0,1] component to byte [0,255] with clamping (no gamma here) */
int	component_to_byte(real_t v, const t_interval *intensity)
{
	v = clamp(v, intensity->min, intensity->max);
	return ((int)(256.0 * v));
}

/* S-curve contrast: pivot at 0.5, scale deviation by 'c'. */
static real_t	apply_contrast(real_t v, real_t c)
{
	v = ((real_t)0.5) + (v - (real_t)0.5) * c;
	if (v < (real_t)0.0)
		v = (real_t)0.0;
	if (v > (real_t)1.0)
		v = (real_t)1.0;
	return (v);
}

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
	real_t	luma;

	*r *= (real_t)RT_EXPOSURE;
	*g *= (real_t)RT_EXPOSURE;
	*b *= (real_t)RT_EXPOSURE;
#if RT_TONE_MAP == 1
	*r = aces_tonemap(*r);
	*g = aces_tonemap(*g);
	*b = aces_tonemap(*b);
#endif
#if !(RT_TONE_MAP == 1 && RT_ACES_GAMMA_BAKED)
	*r = linear_to_gamma(*r);
	*g = linear_to_gamma(*g);
	*b = linear_to_gamma(*b);
#endif
	*r = apply_contrast(*r, (real_t)RT_CONTRAST);
	*g = apply_contrast(*g, (real_t)RT_CONTRAST);
	*b = apply_contrast(*b, (real_t)RT_CONTRAST);
	luma = (real_t)0.2126 * (*r) + (real_t)0.7152 * (*g)
		+ (real_t)0.0722 * (*b);
	*r = luma + (real_t)RT_SATURATION * (*r - luma);
	*g = luma + (real_t)RT_SATURATION * (*g - luma);
	*b = luma + (real_t)RT_SATURATION * (*b - luma);
	if (*r < (real_t)0.0)
		*r = (real_t)0.0;
	if (*g < (real_t)0.0)
		*g = (real_t)0.0;
	if (*b < (real_t)0.0)
		*b = (real_t)0.0;
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
		if (rec.mat && rec.mat->scatter(rec.mat, r, &rec, &attenuation,
				&scattered))
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
