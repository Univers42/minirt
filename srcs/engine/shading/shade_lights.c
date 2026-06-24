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

typedef struct s_lctx
{
	const t_hit_record		*rec;
	const t_vec3			*view;
	const t_hittable_list	*world;
}	t_lctx;

static int	shadow_blocked(const t_point3 *o, const t_vec3 *dir,
				real_t maxd, const t_hittable_list *world)
{
	t_ray	s;

	if (maxd < (real_t)0.002)
		maxd = (real_t)0.002;
	s = ray_create(*o, *dir, (real_t)0.0);
	return (hittable_list_hit(world, &s, interval((real_t)0.001, maxd), NULL));
}

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
	real_t	s;
	t_color	tmp;

	l = vec3_sub(&g_lights[i].pos, &lc->rec->p);
	d = vec3_length(&l);
	if (d < (real_t)1e-6)
		return ;
	l = vec3_div_scalar(&l, d);
	nl = dot(&lc->rec->normal, &l);
	if (nl <= (real_t)0.0)
		return ;
	if (shadow_blocked(&lc->rec->p, &l,
			d - g_lights[i].radius - (real_t)0.01, lc->world))
		return ;
	tmp = vec3_mul_scalar(&g_lights[i].emission, nl / d);
	acc[0] = vec3_add(&acc[0], &tmp);
	s = blinn_phong(&lc->rec->normal, &l, lc->view) * RT_SPECULAR_KS / d;
	tmp = vec3_mul_scalar(&g_lights[i].emission, s);
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
