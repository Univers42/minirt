/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix5.c                                          :+:      :+:    :+:   */
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

int	normalize_diag(t_mat_pair *p)
{
	int		i;
	int		j;
	float	c;

	i = -1;
	while (++i < 4)
	{
		if (p->mat->matrix[i][i] == 0.0f)
		{
			fprintf(stderr, "Singular matrix, couldn't invert\n");
			return (0);
		}
		c = 1 / (float)p->mat->matrix[i][i];
		j = -1;
		while (++j < 4)
		{
			p->mat->matrix[i][j] *= c;
			p->inv->matrix[i][j] *= c;
		}
	}
	return (1);
}

static void	back_subst(t_mat_pair *p)
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
			c = p->mat->matrix[i][j];
			k = -1;
			while (++k < 4)
			{
				p->mat->matrix[i][k] -= p->mat->matrix[j][k] * c;
				p->inv->matrix[i][k] -= p->inv->matrix[j][k] * c;
			}
			p->mat->matrix[i][j] = 0.f;
		}
	}
}

t_matrix	inverse(t_matrix mat)
{
	t_matrix	inv;
	t_mat_pair	p;

	inv = identity();
	p.mat = &mat;
	p.inv = &inv;
	if (!pivot_phase(&p))
		return (identity());
	forward_elim(&p);
	if (!normalize_diag(&p))
		return (identity());
	back_subst(&p);
	return (inv);
}
