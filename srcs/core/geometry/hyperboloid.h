/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hyperboloid.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HYPERBOLOID_H
# define HYPERBOLOID_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "hittable.h"
# include "hittable_list.h"
# include "aabb.h"

/* Equation-based hyperboloid of one sheet (cooling-tower waist) along axis.
   Surface F = (|w|^2 - y^2)/a^2 - y^2/c^2 - 1 = 0 with w = p - center and
   y = dot(w, axis), clamped to the axial band -height/2 <= y <= height/2.
   a = diameter/2 is the waist radius at y = 0; c = height/2 sets the flare so
   the rim widens to a*sqrt(1 + (h/2)^2/c^2) at the caps.  The bbox spans the
   widest rim swept over the full height, so it plugs into the BVH like any
   bounded primitive. */
typedef struct s_hyperboloid
{
	t_point3	center;
	t_vec3		axis;
	real_t		inv_a2;
	real_t		inv_c2;
	real_t		half_h;
	t_material	*mat;
	t_aabb		bbox;
}	t_hyperboloid;

t_hyperboloid	hyperboloid_create(const t_point3 *center, const t_vec3 *axis,
					real_t diameter, real_t height, t_material *mat);
bool			hyperboloid_hit(const t_hyperboloid *hy, const t_ray *r,
					t_interval rayt, t_hit_record *rec);
void			set_current_hyperboloid(const void *obj);
bool			hyperboloid_hit_noobj(const t_ray *r, t_interval rayt,
					t_hit_record *rec);
bool			hittable_list_add_hyperboloid(t_hittable_list *list,
					const t_hyperboloid *hy);

#endif
