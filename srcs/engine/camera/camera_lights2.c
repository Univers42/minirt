/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_lights2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 23:33:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera_lights.h"
#include "ray.h"
#include "interval.h"
#include "studio_config.h"
#include <string.h>

/* set_scene_lights — populate the read-only light cache once, before the
   parallel render begins. */
void	set_scene_lights(const t_light_info *infos, int count)
{
	if (count > RT_MAX_LIGHTS)
		count = RT_MAX_LIGHTS;
	g_light_count = count;
	if (count > 0)
		memcpy(g_lights, infos, (size_t)count * sizeof(t_light_info));
}

/* shadow_ray_blocked — true if a shadow ray hits anything before max_dist. */
int	shadow_ray_blocked(const t_point3 *origin, const t_vec3 *dir,
		real_t max_dist, const t_hittable_list *world)
{
	t_ray	shadow;

	shadow = ray_create(*origin, *dir, (real_t)0.0);
	return (hittable_list_hit(world, &shadow,
			interval((real_t)0.001, max_dist), NULL));
}
