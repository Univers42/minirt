/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.c                                           :+:      :+:    :+:   */
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

t_matrix	identity(void)
{
	t_matrix	id;
	int			i;
	int			j;

	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			id.matrix[i][j] = (float)(i == j);
			j++;
		}
		i++;
	}
	return (id);
}

t_matrix	t_scale(float factor)
{
	t_matrix	scale;

	scale = identity();
	scale.matrix[0][0] = factor;
	scale.matrix[1][1] = factor;
	scale.matrix[2][2] = factor;
	return (scale);
}

t_matrix	t_scale_xyz(float factor_x, float factor_y, float factor_z)
{
	t_matrix	scale;

	scale = identity();
	scale.matrix[0][0] = factor_x;
	scale.matrix[1][1] = factor_y;
	scale.matrix[2][2] = factor_z;
	return (scale);
}

t_matrix	t_translate(float tx, float ty, float tz)
{
	t_matrix	translate;

	translate = identity();
	translate.matrix[0][3] = tx;
	translate.matrix[1][3] = ty;
	translate.matrix[2][3] = tz;
	return (translate);
}

t_matrix	t_rotate_x(float angle)
{
	t_matrix	rotation_x;

	rotation_x = identity();
	rotation_x.matrix[1][1] = cosf(angle);
	rotation_x.matrix[1][2] = -sinf(angle);
	rotation_x.matrix[2][1] = sinf(angle);
	rotation_x.matrix[2][2] = cosf(angle);
	return (rotation_x);
}
