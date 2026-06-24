/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   torus.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TORUS_H
# define TORUS_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "hittable.h"
# include "hittable_list.h"
# include "aabb.h"

/* Equation-based torus (donut).  Tube circle of radius r (minor) is swept
   around a circle of radius R (major) lying in the plane whose normal is
   `axis`.  In the local frame (u, v, axis) the implicit surface is
     (X^2 + Y^2 + Z^2 + R^2 - r^2)^2 = 4 R^2 (X^2 + Z^2)
   with Y measured along `axis` (the hole runs along axis).  An intersection
   is the smallest positive real root of the resulting quartic.  The bbox is
   a cube of half-extent (R + r) about the center, so the torus plugs into
   the BVH like any bounded primitive. */
typedef struct s_torus
{
	t_point3	center;
	t_vec3		axis;
	t_vec3		u;
	t_vec3		v;
	real_t		major;
	real_t		minor;
	t_material	*mat;
	t_aabb		bbox;
}	t_torus;

/* Quartic roots plus the ray interval, passed around so the root-selection
   helper stays inside the 4-argument / 25-line norm budget. */
typedef struct s_root_set
{
	real_t		roots[4];
	int			n;
	real_t		best;
	t_interval	rayt;
}	t_root_set;

t_torus	torus_create(const t_point3 *center, const t_vec3 *axis,
			real_t major, real_t minor, t_material *mat);
bool	torus_hit(const t_torus *to, const t_ray *r,
			t_interval rayt, t_hit_record *rec);
void	set_current_torus(const void *obj);
bool	torus_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec);
bool	hittable_list_add_torus(t_hittable_list *list, const t_torus *to);

#endif
