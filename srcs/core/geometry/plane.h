/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 04:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 04:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PLANE_H
# define PLANE_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "hittable.h"
# include "hittable_list.h"
# include "aabb.h"

/* True infinite plane (equation-based): all points p with
   dot(normal, p - q) == 0.  Carries a precomputed tangent basis for UVs.
   Its bounding box is the whole space, so it is always tested (never culled)
   by the BVH — correct, since the plane is everywhere. */
typedef struct s_plane
{
	t_point3	q;
	t_vec3		normal;
	t_vec3		u_axis;
	t_vec3		v_axis;
	t_material	*mat;
	t_aabb		bbox;
}	t_plane;

t_plane	plane_create(const t_point3 *point, const t_vec3 *normal,
			t_material *mat);
bool	plane_hit(const t_plane *plane, const t_ray *r, t_interval rayt,
			t_hit_record *rec);
void	set_current_plane(const void *obj);
bool	plane_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec);
bool	hittable_list_add_plane(t_hittable_list *list, const t_plane *plane);

#endif
