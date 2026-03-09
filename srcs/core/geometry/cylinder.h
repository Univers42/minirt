/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:59 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:58:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CYLINDER_H
#define CYLINDER_H

#include "types.h"
#include "vector.h"
#include "ray.h"
#include "point.h"
#include "hittable.h"
#include "interval.h"
#include "aabb.h"
#include <math.h>
#include <stdbool.h>

/* Forward declarations */
typedef struct s_material t_material;
typedef struct s_hittable_list t_hittable_list;

/* Finite cylinder: center at base, axis direction, radius, height */
typedef struct s_cylinder
{
	t_point3	base; /* center of bottom cap */
	t_vec3		axis;   /* unit direction from base to top */
	real_t		radius;
	real_t		height;
	t_material	*mat;
	t_aabb		bbox;
}	t_cylinder;

typedef struct s_cone
{
	t_point3	apex; /* tip of cone */
	t_vec3		axis;   /* unit direction from apex to base */
	real_t		angle;  /* half-angle in radians */
	real_t		height; /* distance from apex to base */
	t_material	*mat;
	t_aabb		bbox;
}	t_cone;

bool hittable_list_add_cone(t_hittable_list *list, const t_cone *cone);
bool cone_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec);
void set_current_cone(const void *obj);
bool cone_hit(const t_cone *cone, const t_ray *r,
							t_interval rayt, t_hit_record *rec);
t_cone cone_create(const t_point3 *apex, const t_vec3 *axis,
								 real_t angle_deg, real_t height, t_material *mat);
bool hittable_list_add_cylinder(t_hittable_list *list, const t_cylinder *cyl);
bool cylinder_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec);
void set_current_cylinder(const void *obj);
bool cylinder_hit(const t_cylinder *cyl, const t_ray *r,
								t_interval rayt, t_hit_record *rec);
void cylinder_get_uv(const t_cylinder *cyl, const t_vec3 *p,
								   real_t *u, real_t *v);
t_cylinder cylinder_create_y(const t_point3 *base, real_t radius,
										   real_t height, t_material *mat);
t_cylinder cylinder_create(const t_point3 *base, const t_vec3 *axis,
										 real_t radius, real_t height, t_material *mat);


#endif
