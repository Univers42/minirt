/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shade_lights.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 03:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 03:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shading.h"
#include "camera_lights.h"
#include "ray.h"
#include "interval.h"
#include "studio_config.h"
#include <math.h>

/* Blinn-Phong specular weight for one light (white-tinted highlight). */
static real_t	blinn_phong(const t_vec3 *n, const t_vec3 *l, const t_vec3 *v)
{
	t_vec3	h;
	real_t	nh;

	h = vec3_add(l, v);
	h = unit_vector(&h);
	nh = dot(n, &h);
	return (pow(fmax((real_t)0.0, nh), RT_SHININESS));
}

/* Accumulate one light's diffuse term into acc[0] and specular into acc[1]
   (a single shadow ray serves both).  acc[0] is later tinted by albedo. */
static void	add_light(const t_lctx *lc, int i, t_color acc[2])
{
	t_vec3	l;
	real_t	d;
	real_t	nl;
	real_t	vis;
	t_color	tmp;

	l = vec3_sub(&g_lights[i].pos, &lc->rec->p);
	d = vec3_length(&l);
	if (d < (real_t)1e-6)
		return ;
	l = vec3_div_scalar(&l, d);
	nl = dot(&lc->rec->normal, &l);
	if (nl <= (real_t)0.0)
		return ;
	vis = soft_shadow_visibility(&lc->rec->p, &l, i, lc->world);
	if (vis <= (real_t)0.0)
		return ;
	tmp = vec3_mul_scalar(&g_lights[i].emission, vis * nl / d);
	acc[0] = vec3_add(&acc[0], &tmp);
	nl = blinn_phong(&lc->rec->normal, &l, lc->view) * RT_SPECULAR_KS * vis / d;
	tmp = vec3_mul_scalar(&g_lights[i].emission, nl);
	acc[1] = vec3_add(&acc[1], &tmp);
}

t_color	direct_lighting(const t_hit_record *rec, const t_hittable_list *world,
			const t_vec3 *view, const t_color *albedo)
{
	t_lctx	lc;
	t_color	acc[2];
	t_color	diff;
	int		i;

	lc.rec = rec;
	lc.view = view;
	lc.world = world;
	acc[0] = vec3_zero();
	acc[1] = vec3_zero();
	i = 0;
	while (i < g_light_count)
	{
		add_light(&lc, i, acc);
		i++;
	}
	diff = vec3_mul_elem(&acc[0], albedo);
	return (vec3_add(&diff, &acc[1]));
}
