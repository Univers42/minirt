/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cornell_box_lights.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:46 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CORNELL_BOX_LIGHTS_H
# define CORNELL_BOX_LIGHTS_H

# include "hittable_list.h"
# include "quad.h"
# include "material.h"
# include "vector.h"

/*
 * Add the classic Cornell-box ceiling area light (quad emitter).
 * Centre of ceiling at y = 554, spanning 213-343 on x and 227-332 on z.
 */
static inline bool	cornell_box_lights(t_hittable_list *world)
{
	t_material	*light_mat;
	t_quad		light;
	t_quad		*lp;
	t_vec3		q;
	t_vec3		u;
	t_vec3		v;

	light_mat = diffuse_light_create(vec3_create(15.0, 15.0, 15.0));
	if (!light_mat)
		return (false);
	q = vec3_create(213.0, 554.0, 227.0);
	u = vec3_create(130.0, 0.0, 0.0);
	v = vec3_create(0.0, 0.0, 105.0);
	light = quad_create(&q, &u, &v, light_mat);
	lp = (t_quad *)malloc(sizeof(t_quad));
	if (!lp)
		return (false);
	*lp = light;
	return (hittable_list_add_nonowned(world, &(t_nonowned){lp, set_current_quad, quad_hit_noobj, &light.bbox}));
}

#endif
