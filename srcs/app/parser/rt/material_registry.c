/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material_registry.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 03:45:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 03:45:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Per-run material registry.
**
** Materials are heap-allocated by create_material()/obj_make_material() and
** then shared (by pointer) among many hittables.  Ownership therefore cannot
** live on the hittable wrappers (that would double-free shared materials).
** Instead every created material is recorded here once and released together
** in scene_cleanup() via its own destroy() callback.
*/

#include "rt_parser.h"
#include "material.h"
#include <stdlib.h>

#define MAT_REG_MAX 1024

static t_material	*g_mat_reg[MAT_REG_MAX];
static int			g_mat_reg_n;

void	mat_registry_reset(void)
{
	g_mat_reg_n = 0;
}

void	mat_registry_add(t_material *m)
{
	int	i;

	if (!m)
		return ;
	i = 0;
	while (i < g_mat_reg_n)
	{
		if (g_mat_reg[i] == m)
			return ;
		i++;
	}
	if (g_mat_reg_n < MAT_REG_MAX)
		g_mat_reg[g_mat_reg_n++] = m;
}

void	mat_registry_free_all(void)
{
	int	i;

	i = 0;
	while (i < g_mat_reg_n)
	{
		if (g_mat_reg[i] && g_mat_reg[i]->destroy)
			g_mat_reg[i]->destroy(g_mat_reg[i]);
		i++;
	}
	g_mat_reg_n = 0;
}
