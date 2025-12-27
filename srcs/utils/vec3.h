/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 16:52:49 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 16:52:49 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VEC3_H
# define VEC3_H

#include <stdio.h>
#include <math.h>

typedef struct s_vec3
{
	double	x;
	double	y;
	double	z;
}	t_vec3;

// initializer
t_vec3	vec3_init(double x, double y, double z);
t_vec3  vec3_one(double v);
t_vec3  vec3_copy(t_vec3 v);

// basic ops
t_vec3	vec3_add(t_vec3 v1, t_vec3 v2);
t_vec3	vec3_sub(t_vec3 v1, t_vec3 v2);
t_vec3	vec3_mul(t_vec3 v1, t_vec3 v2);
t_vec3  vec3_div(t_vec3 v1, t_vec3 v2);
t_vec3	vec3_neg(t_vec3 v);
t_vec3	vec3_scale(t_vec3 v, double scalar);

// Geometry
double	vec3_length(t_vec3 v);
t_vec3	vec3_normalize(t_vec3 v);
double	vec3_dot(t_vec3 v1, t_vec3 v2);
t_vec3	vec3_cross(t_vec3 v1, t_vec3 v2);

// Debug
void	vec3_print(t_vec3 v);

#endif	// VEC3_H
