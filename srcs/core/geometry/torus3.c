/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   torus3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "torus.h"
#include <stdlib.h>

static __thread const t_torus	*g_current_torus;

void	set_current_torus(const void *obj)
{
	g_current_torus = (const t_torus *)obj;
}

bool	torus_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_torus)
		return (false);
	return (torus_hit(g_current_torus, r, rayt, rec));
}

bool	hittable_list_add_torus(t_hittable_list *list, const t_torus *to)
{
	t_torus				*copy;
	t_hittable_wrapper	wrap;

	if (!list || !to)
		return (false);
	copy = (t_torus *)malloc(sizeof(t_torus));
	if (!copy)
		return (false);
	*copy = *to;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_torus;
	wrap.hit_noobj = torus_hit_noobj;
	wrap.bbox = to->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
