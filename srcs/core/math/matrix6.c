/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix6.c                                          :+:      :+:    :+:   */
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

static void	print_mat(t_matrix m)
{
	int	i;
	int	j;

	i = 0;
	while (i < 4)
	{
		j = 0;
		while (j < 4)
		{
			printf("%0.4f ", m.matrix[i][j]);
			j++;
		}
		printf("\n");
		i++;
	}
	printf("\n");
}

void	transform_vector(t_matrix transform, t_vector3 *vector)
{
	float	x;
	float	y;
	float	z;

	if (vector == NULL)
	{
		fprintf(stderr, "NULL vector\n");
		return ;
	}
	x = vector->x;
	y = vector->y;
	z = vector->z;
	vector->x = transform.matrix[0][0] * x + transform.matrix[0][1] * y
		+ transform.matrix[0][2] * z;
	vector->y = transform.matrix[1][0] * x + transform.matrix[1][1] * y
		+ transform.matrix[1][2] * z;
	vector->z = transform.matrix[2][0] * x + transform.matrix[2][1] * y
		+ transform.matrix[2][2] * z;
}

void	transform_point(t_matrix transform, t_vector3 *vector)
{
	float	x;
	float	y;
	float	z;

	if (vector == NULL)
	{
		fprintf(stderr, "NULL vector\n");
		return ;
	}
	x = vector->x;
	y = vector->y;
	z = vector->z;
	vector->x = transform.matrix[0][0] * x + transform.matrix[0][1] * y
		+ transform.matrix[0][2] * z + transform.matrix[0][3];
	vector->y = transform.matrix[1][0] * x + transform.matrix[1][1] * y
		+ transform.matrix[1][2] * z + transform.matrix[1][3];
	vector->z = transform.matrix[2][0] * x + transform.matrix[2][1] * y
		+ transform.matrix[2][2] * z + transform.matrix[2][3];
}
