/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   constant_medium.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:02:40 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:04:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "constant_medium.h"

/* Set current medium for hit_noobj callback */
static __thread const t_constant_medium	*g_current_medium = NULL;

void	set_current_medium(const void *obj)
{
	g_current_medium = (const t_constant_medium *)obj;
}

/* Hit function for constant medium */
bool	constant_medium_hit(const t_constant_medium *medium, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_hit_record	rec1;
	t_hit_record	rec2;
	real_t			ray_length;
	real_t			random_val;
	real_t			hit_distance;

	if (!medium || !rec)
		return (false);
	if (!medium->boundary.set_current || !medium->boundary.hit_noobj)
		return (false);
	medium->boundary.set_current(medium->boundary.object);
	if (!medium->boundary.hit_noobj(r, interval(-INFINITY, INFINITY), &rec1))
		return (false);
	medium->boundary.set_current(medium->boundary.object);
	if (!medium->boundary.hit_noobj(r, interval(rec1.t + (real_t)0.0001,
				INFINITY), &rec2))
		return (false);
	if (rec1.t < rayt.min)
		rec1.t = rayt.min;
	if (rec2.t > rayt.max)
		rec2.t = rayt.max;
	if (rec1.t >= rec2.t)
		return (false);
	if (rec1.t < (real_t)0.0)
		rec1.t = (real_t)0.0;
	ray_length = vec3_length(&r->dir);
	random_val = random_double();
	if (random_val <= (real_t)0.0)
		random_val = (real_t)1e-10;
	hit_distance = medium->neg_inv_density * (real_t)log((double)random_val);
	if (hit_distance > (rec2.t - rec1.t) * ray_length)
		return (false);
	rec->t = rec1.t + hit_distance / ray_length;
	rec->p = ray_at((t_ray *)r, rec->t);
	rec->normal = vec3_create((real_t)1.0, (real_t)0.0, (real_t)0.0);
	rec->front_face = true;
	rec->mat = medium->phase_function;
	rec->albedo = vec3_create((real_t)1.0, (real_t)1.0, (real_t)1.0);
	rec->u = (real_t)0.0;
	rec->v = (real_t)0.0;
	return (true);
}

/* Wrapper for hit_noobj callback */
bool	constant_medium_hit_noobj(const t_ray *r, t_interval rayt,
		t_hit_record *rec)
{
	if (!g_current_medium)
		return (false);
	return (constant_medium_hit(g_current_medium, r, rayt, rec));
}

/* Constructor: constant medium with texture */
t_constant_medium	*constant_medium_create_texture(
		const t_hittable_wrapper *boundary, real_t density, t_texture *tex)
{
	t_constant_medium	*medium;

	if (!boundary || density <= (real_t)0.0 || !tex)
		return (NULL);
	medium = (t_constant_medium *)malloc(sizeof(t_constant_medium));
	if (!medium)
		return (NULL);
	medium->boundary = *boundary;
	medium->neg_inv_density = (real_t)-1.0 / density;
	medium->phase_function = isotropic_create_texture(tex);
	if (!medium->phase_function)
	{
		free(medium);
		return (NULL);
	}
	medium->bbox = boundary->bbox;
	return (medium);
}

/* Constructor: constant medium with color albedo */
t_constant_medium	*constant_medium_create_color(
		const t_hittable_wrapper *boundary, real_t density, t_color albedo)
{
	t_constant_medium	*medium;

	if (!boundary || density <= (real_t)0.0)
		return (NULL);
	medium = (t_constant_medium *)malloc(sizeof(t_constant_medium));
	if (!medium)
		return (NULL);
	medium->boundary = *boundary;
	medium->neg_inv_density = (real_t)-1.0 / density;
	medium->phase_function = isotropic_create(albedo);
	if (!medium->phase_function)
	{
		free(medium);
		return (NULL);
	}
	medium->bbox = boundary->bbox;
	return (medium);
}

/* Get bounding box */
t_aabb	constant_medium_bounding_box(const t_constant_medium *medium)
{
	if (!medium)
		return (aabb_empty());
	return (medium->bbox);
}

/* Destructor: cleanup owned material (boundary is not owned) */
void	constant_medium_destroy(t_constant_medium *medium)
{
	if (!medium)
		return ;
	if (medium->phase_function)
	{
		medium->phase_function->destroy(medium->phase_function);
		free(medium->phase_function);
		medium->phase_function = NULL;
	}
	free(medium);
}
