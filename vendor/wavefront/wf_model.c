/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_model.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 21:51:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>

void	wf_model_init(t_wf_model *m)
{
	memset(m, 0, sizeof(*m));
	m->bounds_min = (t_wf_vec3){FLT_MAX, FLT_MAX, FLT_MAX};
	m->bounds_max = (t_wf_vec3){-FLT_MAX, -FLT_MAX, -FLT_MAX};
}

static void	wf_free_groups(t_wf_model *m)
{
	size_t	i;

	i = 0;
	while (i < m->ngroups)
	{
		free(m->groups[i].faces);
		i++;
	}
	free(m->groups);
}

void	wf_model_free(t_wf_model *m)
{
	if (!m)
		return ;
	free(m->verts);
	free(m->norms);
	free(m->texcoords);
	wf_free_groups(m);
	memset(m, 0, sizeof(*m));
}

static bool	wf_grow_array(void **arr, size_t *cap, size_t elem_size)
{
	size_t	newcap;
	void	*newarr;

	newcap = 64;
	if (*cap != 0)
		newcap = *cap * 2;
	newarr = realloc(*arr, newcap * elem_size);
	if (!newarr)
		return (false);
	*arr = newarr;
	*cap = newcap;
	return (true);
}

bool	wf_model_add_vert(t_wf_model *m, t_wf_vec3 v)
{
	if (m->nverts >= m->vert_cap)
	{
		if (!wf_grow_array((void **)&m->verts, &m->vert_cap,
				sizeof(t_wf_vec3)))
			return (false);
	}
	m->verts[m->nverts++] = v;
	if (v.x < m->bounds_min.x)
		m->bounds_min.x = v.x;
	if (v.y < m->bounds_min.y)
		m->bounds_min.y = v.y;
	if (v.z < m->bounds_min.z)
		m->bounds_min.z = v.z;
	if (v.x > m->bounds_max.x)
		m->bounds_max.x = v.x;
	if (v.y > m->bounds_max.y)
		m->bounds_max.y = v.y;
	if (v.z > m->bounds_max.z)
		m->bounds_max.z = v.z;
	return (true);
}
