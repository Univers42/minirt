/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec3.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 16:52:46 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 16:52:46 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vec3.h"

t_vec3	vec3_init(double x, double y, double z)
{
	t_vec3	v;

	v.x = x;
	v.y = y;
	v.z = z;
	return (v);
}

t_vec3 vec3_one(double v)
{
	return (vec3_init(v, v, v));
}

t_vec3 vec3_copy(t_vec3 v)
{
	return (vec3_init(v.x, v.y, v.z));
}

t_vec3	vec3_add(t_vec3 v1, t_vec3 v2)
{
	return (vec3_init(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
}

t_vec3	vec3_sub(t_vec3 v1, t_vec3 v2)
{
	return (vec3_init(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
}

t_vec3	vec3_mul(t_vec3 v1, t_vec3 v2)
{
	return (vec3_init(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z));
}

t_vec3  vec3_div(t_vec3 v1, t_vec3 v2)
{
	return (vec3_init(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z));
}

t_vec3	vec3_neg(t_vec3 v)
{
	return (vec3_init(-v.x, -v.y, -v.z));
}

t_vec3	vec3_scale(t_vec3 v, double scalar)
{
	return (vec3_init(v.x * scalar, v.y * scalar, v.z * scalar));
}

double	vec3_length(t_vec3 v)
{
	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

t_vec3	vec3_normalize(t_vec3 v)
{
	double	len;

	len = vec3_length(v);
	if (len == 0)
		return (vec3_init(0, 0, 0));
	return (vec3_div(v, vec3_one(len)));
}

double	vec3_dot(t_vec3 v1, t_vec3 v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

t_vec3	vec3_cross(t_vec3 v1, t_vec3 v2)
{
	return (vec3_init(v1.y * v2.z - v1.z * v2.y,
					  v1.z * v2.x - v1.x * v2.z,
					  v1.x * v2.y - v1.y * v2.x));
}

void	vec3_print(t_vec3 v)
{
	printf("Vect3(%f, %f, %f)\n", v.x, v.y, v.z);
}

