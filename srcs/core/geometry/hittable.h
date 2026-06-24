/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 19:37:39 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:10:25 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_H
# define HITTABLE_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "aabb.h"
# include <stdbool.h>

/* Forward declaration of material to avoid circular dependency */
typedef struct s_material		t_material;
/* Forward declaration of hit record for callback typedefs */
typedef struct s_hit_record		t_hit_record;

/* Generic wrapper callbacks */
typedef void					(*t_set_current_fn)(const void *obj);
typedef bool					(*t_hit_noobj_fn)(const t_ray *r,
		t_interval rayt, t_hit_record *rec);

/* Generic hittable wrapper (used by lists, BVH, transforms) */
typedef struct s_hittable_wrapper
{
	void				*object;
	bool				owned;
	t_set_current_fn	set_current;
	t_hit_noobj_fn		hit_noobj;
	t_aabb				bbox;
}	t_hittable_wrapper;

/* Trailing (size, height) pair for quadric constructors (keeps arg count
   within the 42-norm 4-arg cap).  "size" is radius / angle_deg / diameter
   depending on the primitive. */
typedef struct s_shape_dims
{
	real_t	size;
	real_t	height;
}	t_shape_dims;

/* Hit record: store intersection point, normal, material and t. */
struct s_hit_record
{
	t_vec3		p;
	t_vec3		normal;
	real_t		t;
	bool		front_face;
	t_vec3		albedo;
	real_t		u;
	real_t		v;
	t_material	*mat;
};

typedef struct s_translate_wrap
{
	t_hittable_wrapper	child;
	t_vec3				offset;
	t_aabb				bbox;
}	t_translate_wrap;

typedef struct s_rotate_y_wrap
{
	t_hittable_wrapper	child;
	real_t				sin_theta;
	real_t				cos_theta;
	t_aabb				bbox;
}	t_rotate_y_wrap;

void				set_face_normal(t_hit_record *hit, const t_ray *r,
						const t_vec3 *outward_normal);
t_rotate_y_wrap		*rotate_y_create(const t_hittable_wrapper *child,
						real_t angle_deg);
bool				rotate_y_hit_noobj(const t_ray *r, t_interval rayt,
						t_hit_record *rec);
void				set_current_rotate(const void *obj);
t_translate_wrap	*translate_create(const t_hittable_wrapper *child,
						const t_vec3 *offset);
bool				translate_hit_noobj(const t_ray *r, t_interval rayt,
						t_hit_record *rec);
void				set_current_translate(const void *obj);
t_vec3				rotate_y_vec(const t_vec3 *v, real_t sin_t, real_t cos_t);

#endif