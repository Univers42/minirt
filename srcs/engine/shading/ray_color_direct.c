/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ray_color_direct.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 03:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 03:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shading.h"
#include "camera.h"
#include "material.h"
#include "interval.h"
#include "studio_config.h"

/* Render-mode flag: written once before the OpenMP render begins, then
   read-only across threads (same contract as the g_lights cache). */
static t_engine_mode	g_engine_mode = ENGINE_DIRECT;

void	render_set_engine_mode(t_engine_mode mode)
{
	g_engine_mode = mode;
}

t_engine_mode	render_get_engine_mode(void)
{
	return (g_engine_mode);
}

/* Deterministic primary ray through the exact pixel centre (no jitter,
   no defocus) — gives byte-reproducible renders and stable expose redraws. */
t_ray	get_ray_center(const t_camera *cam, int i, int j)
{
	t_vec3	u;
	t_vec3	v;
	t_vec3	sample;
	t_vec3	dir;

	u = vec3_mul_scalar(&cam->pixel_delta_u, (real_t)i);
	v = vec3_mul_scalar(&cam->pixel_delta_v, (real_t)j);
	sample = vec3_add(&cam->pixel00_loc, &u);
	sample = vec3_add(&sample, &v);
	dir = vec3_sub(&sample, &cam->center);
	return (ray_create(cam->center, dir, (real_t)0.0));
}

/* Classify the surface by its scatter behaviour and shade accordingly. */
static t_vec3	shade(const t_shade_ctx *c)
{
	if (!c->rec->mat || !c->rec->mat->scatter)
		return (c->emission);
	if (c->rec->mat->scatter == diffuse_light_scatter)
		return (c->emission);
	if (c->rec->mat->scatter == metal_scatter
		|| c->rec->mat->scatter == glossy_scatter)
		return (shade_reflective(c));
	if (c->rec->mat->scatter == dielectric_scatter
		|| c->rec->mat->scatter == tinted_glass_scatter)
		return (shade_glass(c));
	return (shade_matte(c));
}

t_vec3	ray_color_direct(const t_ray *r, const t_hittable_list *world,
			int depth, const t_color *ambient)
{
	t_hit_record	rec;
	t_shade_ctx		c;

	if (depth <= 0)
		return (vec3_zero());
	if (!hittable_list_hit(world, r, interval((real_t)1e-4, INFINITY), &rec))
	{
		if (ambient->x < 0.01 && ambient->y < 0.01 && ambient->z < 0.01)
			return (*ambient);
		return (bg_sky_color(r, ambient));
	}
	c.r = r;
	c.rec = &rec;
	c.world = world;
	c.depth = depth;
	c.amb = ambient;
	c.emission = vec3_zero();
	if (rec.mat && rec.mat->emitted)
		c.emission = rec.mat->emitted(rec.mat, rec.u, rec.v, &rec.p,
				rec.front_face);
	return (shade(&c));
}
