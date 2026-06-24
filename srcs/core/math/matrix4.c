/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix4.c                                          :+:      :+:    :+:   */
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

static void	swap(float *a, float *b)
{
	float	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

static int	find_pivot(t_matrix *mat, int j)
{
	int	max;
	int	i;

	max = j;
	i = j;
	while (i < 4)
	{
		if (fabs(mat->matrix[i][j]) >= fabs(mat->matrix[max][j]))
			max = i;
		i++;
	}
	return (max);
}

int	pivot_phase(t_mat_pair *p)
{
	int	j;
	int	k;
	int	max;

	j = -1;
	while (++j < 4)
	{
		if (p->mat->matrix[j][j] != 0.0f)
			continue ;
		max = find_pivot(p->mat, j);
		if (max == j)
		{
			fprintf(stderr, "Singular matrix, couldn't invert\n");
			return (0);
		}
		k = -1;
		while (++k < 4)
		{
			swap(&p->mat->matrix[max][k], &p->mat->matrix[j][k]);
			swap(&p->inv->matrix[max][k], &p->inv->matrix[j][k]);
		}
	}
	return (1);
}

void	forward_elim(t_mat_pair *p)
{
	int		i;
	int		j;
	int		k;
	float	c;

	i = -1;
	while (++i < 3)
	{
		j = i;
		while (++j < 4)
		{
			if (p->mat->matrix[j][i] == 0.0f)
				continue ;
			c = p->mat->matrix[j][i] / p->mat->matrix[i][i];
			k = -1;
			while (++k < 4)
			{
				p->mat->matrix[j][k] -= p->mat->matrix[i][k] * c;
				p->inv->matrix[j][k] -= p->inv->matrix[i][k] * c;
			}
			p->mat->matrix[j][i] = 0.f;
		}
	}
}
