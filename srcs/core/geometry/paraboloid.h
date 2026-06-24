/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paraboloid.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARABOLOID_H
# define PARABOLOID_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "hittable.h"
# include "hittable_list.h"
# include "aabb.h"

/* Equation-based paraboloid (bowl) opening along +axis from the vertex.
   Surface F = |w|^2 - dot(w,axis)^2 - k*dot(w,axis) = 0 with w = p - vertex,
   clamped to the axial band 0 <= dot(w,axis) <= height.  k = (d/2)^2 / height
   so the rim radius equals diameter/2 at y = height.  bbox spans the rim disk
   swept from the vertex to the top, so it plugs into the BVH like any
   bounded primitive. */
typedef struct s_paraboloid
{
	t_point3	vertex;
	t_vec3		axis;
	real_t		k;
	real_t		height;
	t_material	*mat;
	t_aabb		bbox;
}	t_paraboloid;

t_paraboloid	paraboloid_create(const t_point3 *vertex, const t_vec3 *axis,
					const t_shape_dims *dims, t_material *mat);
bool			paraboloid_hit(const t_paraboloid *pb, const t_ray *r,
					t_interval rayt, t_hit_record *rec);
void			set_current_paraboloid(const void *obj);
bool			paraboloid_hit_noobj(const t_ray *r, t_interval rayt,
					t_hit_record *rec);
bool			hittable_list_add_paraboloid(t_hittable_list *list,
					const t_paraboloid *pb);

#endif
