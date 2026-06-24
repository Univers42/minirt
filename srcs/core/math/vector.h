/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 22:17:01 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VECTOR_H
# define VECTOR_H

# include "types.h"
# include "settings.h"
# include <math.h>
# include <stdbool.h>

/* 3D vector type */
typedef struct s_vec3
{
	real_t	x;
	real_t	y;
	real_t	z;
}	t_vec3;

/* vector.c */
t_vec3	vec3_create(real_t x, real_t y, real_t z);
t_vec3	vec3_zero(void);
t_vec3	vec3_neg(const t_vec3 *v);
t_vec3	vec3_add(const t_vec3 *a, const t_vec3 *b);
t_vec3	vec3_sub(const t_vec3 *a, const t_vec3 *b);

/* vector2.c */
t_vec3	vec3_mul_elem(const t_vec3 *a, const t_vec3 *b);
t_vec3	vec3_mul_scalar(const t_vec3 *v, real_t t);
t_vec3	vec3_div_scalar(const t_vec3 *v, real_t t);
real_t	dot(const t_vec3 *a, const t_vec3 *b);
t_vec3	cross(const t_vec3 *a, const t_vec3 *b);

/* vector3.c */
real_t	vec3_length_squared(const t_vec3 *v);
real_t	vec3_length(const t_vec3 *v);
t_vec3	unit_vector(const t_vec3 *v);
t_vec3	vec3_lerp(const t_vec3 *a, const t_vec3 *b, real_t t);
bool	vec3_near_zero(const t_vec3 *v);

/* vector4.c */
t_vec3	vec3_reflect(const t_vec3 *v, const t_vec3 *n);
t_vec3	vec3_refract(const t_vec3 *uv, const t_vec3 *n, real_t etai_over_etat);
t_vec3	vec3_random(void);
t_vec3	vec3_random_interval(real_t min, real_t max);
t_vec3	random_in_unit_sphere(void);

/* vector5.c */
t_vec3	random_unit_vector(void);
t_vec3	random_on_hemisphere(const t_vec3 *normal);
t_vec3	random_in_unit_disk(void);
t_vec3	random_cosine_direction(const t_vec3 *normal);

#endif
