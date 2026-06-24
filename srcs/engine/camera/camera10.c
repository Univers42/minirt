/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera10.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:53:17 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:11:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include "material.h"
#include "studio_config.h"
#include "environment.h"

/* Colour for a ray that hits nothing: HDRI environment, black, or sky. */
static t_vec3	bg_on_miss(const t_ray *r, const t_color *background)
{
	if (get_scene_environment())
		return (bg_environment_color(r));
	if (background->x < 0.01 && background->y < 0.01
		&& background->z < 0.01)
		return (vec3_zero());
	return (bg_sky_color(r, background));
}

/* Emission + scattered (direct + indirect) contribution of one hit. */
static t_vec3	shade_hit(const t_li *li, const t_hit_record *rec,
				const t_ray *r, t_color *att)
{
	t_color	emission;
	t_color	lit;

	emission = vec3_zero();
	if (rec->mat && rec->mat->emitted)
		emission = rec->mat->emitted(&(t_emit){rec->mat, rec->u, rec->v,
				&rec->p, rec->front_face});
	if (rec->mat && rec->mat->scatter(&(t_scatter){rec->mat, r,
			rec, att, (t_ray *)li->scattered}))
	{
		if (!russian_roulette(li->depth, RT_MAX_DEPTH, att))
			return (emission);
		lit = compute_lighting(li);
		return (vec3_add(&emission, &lit));
	}
	return (emission);
}

t_vec3	ray_color_with_background(const t_ray *r,
			const t_hittable_list *world, int depth,
			const t_color *background)
{
	t_hit_record	rec;
	t_ray			scattered;
	t_color			attenuation;
	t_li			li;

	if (depth <= 0)
		return (vec3_zero());
	if (!hittable_list_hit(world, r, interval((real_t)1e-4, INFINITY), &rec))
		return (bg_on_miss(r, background));
	li = (t_li){&rec, world, background, &attenuation, &scattered, depth};
	return (shade_hit(&li, &rec, r, &attenuation));
}
