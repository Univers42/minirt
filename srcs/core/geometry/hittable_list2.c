/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable_list2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:08:31 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:18:27 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable_list.h"

t_aabb	hittable_list_bounding_box(const t_hittable_list *list)
{
	if (!list)
		return (aabb_empty());
	return (list->bbox);
}

bool	hittable_list_hit(const t_hittable_list *list, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	bool						hit_anything;
	real_t						closest_so_far;
	t_hit_record				temp_rec;
	size_t						i;
	const t_hittable_wrapper	*w;

	if (list->fast_hit)
		return (list->fast_hit(list->fast, r, rayt, rec));
	hit_anything = false;
	closest_so_far = (real_t)rayt.max;
	i = 0;
	while (i < list->count)
	{
		w = &list->objects[i];
		if (w->set_current && w->hit_noobj)
		{
			w->set_current(w->object);
			if (w->hit_noobj(r, interval(rayt.min, closest_so_far), &temp_rec))
			{
				hit_anything = true;
				closest_so_far = (real_t)temp_rec.t;
				if (rec)
					*rec = temp_rec;
			}
		}
		i++;
	}
	return (hit_anything);
}

static __thread const t_hittable_list	*g_current_list = NULL;

void	set_current_hlist(const void *obj)
{
	g_current_list = (const t_hittable_list *)obj;
}

bool	hittable_list_hit_noobj(const t_ray *r, t_interval rayt,
		t_hit_record *rec)
{
	if (!g_current_list)
		return (false);
	return (hittable_list_hit(g_current_list, r, rayt, rec));
}

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
