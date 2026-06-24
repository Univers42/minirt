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
# define CYLINDER_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "point.h"
# include "hittable.h"
# include "interval.h"
# include "aabb.h"
# include <math.h>
# include <stdbool.h>

/* Forward declarations */
typedef struct s_material		t_material;
typedef struct s_hittable_list	t_hittable_list;

/* Finite cylinder: center at base, axis direction, radius, height */
typedef struct s_cylinder
{
	t_point3	base;
	t_vec3		axis;
	real_t		radius;
	real_t		height;
	t_material	*mat;
	t_aabb		bbox;
}	t_cylinder;

typedef struct s_cyl_cap
{
	real_t	t;
	real_t	capv;
}	t_cyl_cap;

typedef struct s_cyl_caps
{
	t_interval		rayt;
	real_t			*closest_t;
	t_hit_record	*rec;
}	t_cyl_caps;

typedef struct s_cone
{
	t_point3	apex;
	t_vec3		axis;
	real_t		angle;
	real_t		height;
	t_material	*mat;
	t_aabb		bbox;
}	t_cone;

/* Shared scratch state threaded through the cone intersection helpers. */
typedef struct s_cone_hit
{
	const t_cone	*cone;
	const t_ray		*r;
	t_interval		rayt;
	real_t			trig[2];
	real_t			dv[2];
	real_t			closest_t;
	t_vec3			closest_normal;
	bool			hit_anything;
}	t_cone_hit;

bool		hittable_list_add_cone(t_hittable_list *list, const t_cone *cone);
bool		cone_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec);
void		set_current_cone(const void *obj);
bool		cone_hit(const t_cone *cone, const t_ray *r,
				t_interval rayt, t_hit_record *rec);
bool		cone_coeffs(t_cone_hit *ctx, real_t *abc, real_t *roots);
void		cone_test_cap(t_cone_hit *ctx);
void		cone_set_record(t_cone_hit *ctx, t_hit_record *rec);
t_cone		cone_create(const t_point3 *apex, const t_vec3 *axis,
				const t_shape_dims *dims, t_material *mat);
bool		hittable_list_add_cylinder(t_hittable_list *list,
				const t_cylinder *cyl);
bool		cyl_hit_caps(const t_cylinder *cyl, const t_ray *r,
				t_cyl_caps *ctx);
bool		cylinder_hit_noobj(const t_ray *r, t_interval rayt,
				t_hit_record *rec);
void		set_current_cylinder(const void *obj);
bool		cylinder_hit(const t_cylinder *cyl, const t_ray *r,
				t_interval rayt, t_hit_record *rec);
void		cylinder_get_uv(const t_cylinder *cyl, const t_vec3 *p,
				real_t *u, real_t *v);
t_cylinder	cylinder_create_y(const t_point3 *base, real_t radius,
				real_t height, t_material *mat);
t_cylinder	cylinder_create(const t_point3 *base, const t_vec3 *axis,
				const t_shape_dims *dims, t_material *mat);

#endif
