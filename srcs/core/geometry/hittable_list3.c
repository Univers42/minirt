/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable_list3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable_list.h"

t_hittable_wrapper	hittable_list_wrapper(const t_hittable_list *list)
{
	t_hittable_wrapper	w;

	w.object = (void *)list;
	w.owned = false;
	w.set_current = set_current_hlist;
	w.hit_noobj = hittable_list_hit_noobj;
	if (list)
		w.bbox = list->bbox;
	else
		w.bbox = aabb_empty();
	return (w);
}
