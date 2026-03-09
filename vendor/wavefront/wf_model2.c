/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_model2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 21:51:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include <stdlib.h>
#include <string.h>

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

bool	wf_model_add_norm(t_wf_model *m, t_wf_vec3 n)
{
	if (m->nnorms >= m->norm_cap)
	{
		if (!wf_grow_array((void **)&m->norms, &m->norm_cap,
				sizeof(t_wf_vec3)))
			return (false);
	}
	m->norms[m->nnorms++] = n;
	return (true);
}

bool	wf_model_add_texcoord(t_wf_model *m, t_wf_vec2 tc)
{
	if (m->ntexcoords >= m->texcoord_cap)
	{
		if (!wf_grow_array((void **)&m->texcoords, &m->texcoord_cap,
				sizeof(t_wf_vec2)))
			return (false);
	}
	m->texcoords[m->ntexcoords++] = tc;
	return (true);
}

bool	wf_model_ensure_group(t_wf_model *m)
{
	t_wf_group	*g;

	if (m->ngroups > 0)
		return (true);
	if (m->ngroups >= m->group_cap)
	{
		if (!wf_grow_array((void **)&m->groups, &m->group_cap,
				sizeof(t_wf_group)))
			return (false);
	}
	g = &m->groups[m->ngroups++];
	memset(g, 0, sizeof(*g));
	strncpy(g->name, "default", 63);
	return (true);
}

bool	wf_model_add_group(t_wf_model *m, const char *name)
{
	t_wf_group	*g;

	if (m->ngroups >= m->group_cap)
	{
		if (!wf_grow_array((void **)&m->groups, &m->group_cap,
				sizeof(t_wf_group)))
			return (false);
	}
	g = &m->groups[m->ngroups++];
	memset(g, 0, sizeof(*g));
	strncpy(g->name, name, 63);
	g->name[63] = '\0';
	return (true);
}
