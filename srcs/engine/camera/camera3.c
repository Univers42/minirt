/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:11:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "material.h"
#include "camera_lights.h"
#include "studio_config.h"
#include "random.h"
#include "environment.h"

/*
** sky_peak — overall sky energy = brightest background component.
** Scaling the whole gradient by this keeps the sky tied to the scene
** ambient, so a dark scene gets a dark sky (no wash-out).
*/
static real_t	sky_peak(const t_color *background)
{
	real_t	peak;

	peak = background->x;
	if (background->y > peak)
		peak = background->y;
	if (background->z > peak)
		peak = background->z;
	return (peak);
}

/*
** bg_sky_color — physically-plausible sky dome for miss rays.
**
** Two tinted bands (studio_config.h): a brighter, slightly warm
** HORIZON easing up to a deeper, cooler ZENITH.  Both bands are
** scaled by the background peak, so the gradient never blows out
** dark scenes.  Shared by BOTH engines (direct + path tracer), so
** reflective/refractive rays see the same graded environment.
**
**   t ≈ 0  (looking level/down) → warm horizon haze
**   t ≈ 1  (looking up)         → deep blue zenith
*/
t_vec3	bg_sky_color(const t_ray *r, const t_color *background)
{
	t_vec3	unit_dir;
	real_t	t;
	real_t	peak;
	t_vec3	horizon;
	t_vec3	zenith;

	unit_dir = unit_vector(&r->dir);
	t = unit_dir.y;
	if (t < (real_t)0.0)
		t = (real_t)0.0;
	t = sqrt(t);
	peak = sky_peak(background) * (real_t)RT_SKY_HORIZON_GAIN;
	horizon = vec3_create((real_t)RT_SKY_HORIZON_R * peak,
			(real_t)RT_SKY_HORIZON_G * peak, (real_t)RT_SKY_HORIZON_B * peak);
	peak = peak * (real_t)RT_SKY_ZENITH_GAIN;
	zenith = vec3_create((real_t)RT_SKY_ZENITH_R * peak,
			(real_t)RT_SKY_ZENITH_G * peak, (real_t)RT_SKY_ZENITH_B * peak);
	return (vec3_lerp(&horizon, &zenith, t));
}

static t_color	compute_lighting(const t_hit_record *rec,
					const t_hittable_list *world, int depth,
					const t_color *bg, const t_color *att,
					const t_ray *scattered)
{
	t_color	direct;
	t_color	indirect;
	t_color	total;

	direct = vec3_zero();
#if RT_DIRECT_LIGHT_ENABLED
	direct = sample_direct_lights(rec, world);
	direct = vec3_mul_elem(att, &direct);
#endif
	indirect = ray_color_with_background(scattered, world,
			depth - 1, bg);
	indirect = vec3_mul_elem(att, &indirect);
	total = vec3_add(&direct, &indirect);
	return (total);
}

/*
** russian_roulette — probabilistic path termination for unbiased
** speedup.  After RT_RR_START_DEPTH bounces, paths with low
** attenuation have a proportional chance of being terminated.
** Surviving paths are boosted by 1/p_continue to keep the
** estimator unbiased.
**
** Returns: 1 if the path survives (att is scaled), 0 if terminated.
*/
static int	russian_roulette(int depth, int max_depth, t_color *att)
{
#if RT_RR_START_DEPTH > 0
	real_t	p_max;
	real_t	p_continue;
	int		bounces;

	bounces = max_depth - depth;
	if (bounces < RT_RR_START_DEPTH)
		return (1);
	p_max = att->x;
	if (att->y > p_max)
		p_max = att->y;
	if (att->z > p_max)
		p_max = att->z;
	p_continue = p_max;
	if (p_continue < (real_t)0.05)
		p_continue = (real_t)0.05;
	if (p_continue > (real_t)0.95)
		p_continue = (real_t)0.95;
	if (random_real() > p_continue)
		return (0);
	*att = vec3_div_scalar(att, p_continue);
#else
	(void)depth;
	(void)max_depth;
	(void)att;
#endif
	return (1);
}

t_vec3	ray_color_with_background(const t_ray *r,
			const t_hittable_list *world, int depth,
			const t_color *background)
{
	t_hit_record	rec;
	t_ray			scattered;
	t_color			attenuation;
	t_color			emission;
	t_color			lit;

	if (depth <= 0)
		return (vec3_zero());
	if (!hittable_list_hit(world, r, interval((real_t)1e-4, INFINITY), &rec))
	{
		if (get_scene_environment())
			return (bg_environment_color(r));
		if (background->x < 0.01 && background->y < 0.01
			&& background->z < 0.01)
			return (vec3_zero());
		return (bg_sky_color(r, background));
	}
	emission = vec3_zero();
	if (rec.mat && rec.mat->emitted)
		emission = rec.mat->emitted(rec.mat, rec.u, rec.v, &rec.p,
				rec.front_face);
	if (rec.mat && rec.mat->scatter(rec.mat, r, &rec, &attenuation,
			&scattered))
	{
		if (!russian_roulette(depth, RT_MAX_DEPTH, &attenuation))
			return (emission);
		lit = compute_lighting(&rec, world, depth, background,
				&attenuation, &scattered);
		return (vec3_add(&emission, &lit));
	}
	return (emission);
}

unsigned char	*write_color_to_buf_bin(unsigned char *dst,
				const t_vec3 *pixel)
{
	real_t					r;
	real_t					g;
	real_t					b;
	static const t_interval	intensity = {0.000, 0.999, true};

	r = pixel->x;
	g = pixel->y;
	b = pixel->z;
	color_post_process(&r, &g, &b);
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
