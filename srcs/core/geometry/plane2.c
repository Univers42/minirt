/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   plane2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 04:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 04:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "plane.h"
#include <stdlib.h>

/* Per-thread "current object" for the two-step wrapper dispatch. */
static __thread const t_plane	*g_current_plane;

void	set_current_plane(const void *obj)
{
	g_current_plane = (const t_plane *)obj;
}

bool	plane_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_plane)
		return (false);
	return (plane_hit(g_current_plane, r, rayt, rec));
}

bool	hittable_list_add_plane(t_hittable_list *list, const t_plane *plane)
{
	t_plane				*copy;
	t_hittable_wrapper	wrap;

	if (!list || !plane)
		return (false);
	copy = (t_plane *)malloc(sizeof(t_plane));
	if (!copy)
		return (false);
	*copy = *plane;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_plane;
	wrap.hit_noobj = plane_hit_noobj;
	wrap.bbox = plane->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
