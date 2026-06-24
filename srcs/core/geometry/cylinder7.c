/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cylinder7.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cylinder.h"
#include "hittable_list.h"
#include <stdlib.h>
#include <stddef.h>

static __thread const t_cone	*g_current_cone = NULL;

void	set_current_cone(const void *obj)
{
	g_current_cone = (const t_cone *)obj;
}

bool	cone_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_cone)
		return (false);
	return (cone_hit(g_current_cone, r, rayt, rec));
}

bool	hittable_list_add_cone(t_hittable_list *list, const t_cone *cone)
{
	t_cone				*copy;
	t_hittable_wrapper	wrap;

	if (!list || !cone)
		return (false);
	copy = (t_cone *)malloc(sizeof(t_cone));
	if (!copy)
		return (false);
	*copy = *cone;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_cone;
	wrap.hit_noobj = cone_hit_noobj;
	wrap.bbox = cone->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
