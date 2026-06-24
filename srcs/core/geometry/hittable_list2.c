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

static bool	hit_one_wrapper(const t_hittable_wrapper *w, const t_ray *r,
		t_interval rayt, t_hit_record *out)
{
	if (!w->set_current || !w->hit_noobj)
		return (false);
	w->set_current(w->object);
	return (w->hit_noobj(r, rayt, out));
}

bool	hittable_list_hit(const t_hittable_list *list, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	bool			hit_anything;
	real_t			closest_so_far;
	size_t			i;
	t_hit_record	temp_rec;

	if (list->fast_hit)
		return (list->fast_hit(list->fast, r, rayt, rec));
	hit_anything = false;
	closest_so_far = (real_t)rayt.max;
	i = 0;
	while (i < list->count)
	{
		if (hit_one_wrapper(&list->objects[i], r,
				interval(rayt.min, closest_so_far), &temp_rec))
		{
			hit_anything = true;
			closest_so_far = (real_t)temp_rec.t;
			if (rec)
				*rec = temp_rec;
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
