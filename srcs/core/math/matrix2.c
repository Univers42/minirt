/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "transformations.h"
#include "vector.h"

t_matrix	t_rotate_y(float angle)
{
	t_matrix	rotation_y;

	rotation_y = identity();
	rotation_y.matrix[0][0] = cosf(angle);
	rotation_y.matrix[0][2] = sinf(angle);
	rotation_y.matrix[2][0] = -sinf(angle);
	rotation_y.matrix[2][2] = cosf(angle);
	return (rotation_y);
}

t_matrix	t_rotate_z(float angle)
{
	t_matrix	rotation_z;

	rotation_z = identity();
	rotation_z.matrix[0][0] = cosf(angle);
	rotation_z.matrix[0][1] = -sinf(angle);
	rotation_z.matrix[1][0] = sinf(angle);
	rotation_z.matrix[1][1] = cosf(angle);
	return (rotation_z);
}

t_matrix	mat_multiply(t_matrix m1, t_matrix m2)
{
	t_matrix	new_mat;
	int			i;
	int			j;
	int			k;

	new_mat = identity();
	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			k = 0;
			while (k < 4)
			{
				new_mat.matrix[i][j] += m1.matrix[i][k] * m2.matrix[k][j];
				k++;
			}
			j++;
		}
		i++;
	}
	return (new_mat);
}

t_matrix	quaternion_to_matrix(t_quaternion q)
{
	t_matrix	m;
	float		w;
	float		x;
	float		y;
	float		z;

	m = identity();
	w = q.w;
	x = q.x;
	y = q.y;
	z = q.z;
	m.matrix[0][0] = 1 - 2 * (y * y + z * z);
	m.matrix[0][1] = 2 * (x * y - z * w);
	m.matrix[0][2] = 2 * (x * z + y * w);
	m.matrix[1][0] = 2 * (x * y + z * w);
	m.matrix[1][1] = 1 - 2 * (x * x + z * z);
	m.matrix[1][2] = 2 * (y * z - x * w);
	m.matrix[2][0] = 2 * (x * z - y * w);
	m.matrix[2][1] = 2 * (y * z + x * w);
	m.matrix[2][2] = 1 - 2 * (x * x + y * y);
	return (m);
}

t_matrix	transpose(t_matrix m)
{
	return ((t_matrix){
		.matrix = {
			{m.matrix[0][0], m.matrix[1][0], m.matrix[2][0], m.matrix[3][0]},
			{m.matrix[0][1], m.matrix[1][1], m.matrix[2][1], m.matrix[3][1]},
			{m.matrix[0][2], m.matrix[1][2], m.matrix[2][2], m.matrix[3][2]},
			{m.matrix[0][3], m.matrix[1][3], m.matrix[2][3], m.matrix[3][3]}}
	});
}
