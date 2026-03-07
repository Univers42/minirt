/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sphere.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:52:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:21:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPHERE_H
# define SPHERE_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "point.h"
# include "hittable.h"
# include "interval.h"
# include "aabb.h"
# include <math.h>
# include <stdbool.h>

typedef struct s_material	t_material;

typedef struct s_moving_sphere
{
	t_vec3	center1;
	t_vec3	center_velocity;
}	t_moving_sphere;

typedef struct s_sphere
{
	t_moving_sphere	center;
	real_t			radius;
	t_vec3			albedo;
	t_aabb			bbox;
	t_material		*mat;
}	t_sphere;

t_vec3		sphere_center_at(const t_sphere *s, real_t time);
void		sphere_get_uv(const t_vec3 *p, real_t *u, real_t *v);
t_sphere	create_sphere(const t_point3 *center, real_t radius,
				t_vec3 albedo, t_material *mat);
t_sphere	create_sphere_moving(const t_point3 *center1,
				const t_point3 *center2, real_t radius,
				t_vec3 albedo, t_material *mat);
t_sphere	create_sphere_default(const t_point3 *center, real_t radius);
real_t		hit_sphere(const t_vec3 *center, real_t radius, const t_ray *r);
void		set_current_sphere(const void *obj);
bool		sphere_hit_noobj(const t_ray *r, t_interval rayt,
				t_hit_record *rec);

#endif