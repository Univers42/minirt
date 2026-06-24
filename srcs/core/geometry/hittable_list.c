/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable_list.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 19:08:31 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:18:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hittable_list.h"

void	hittable_list_init(t_hittable_list *list)
{
	list->objects = NULL;
	list->count = 0;
	list->capacity = 0;
	list->bbox = aabb_empty();
	list->fast = NULL;
	list->fast_hit = NULL;
}

void	hittable_list_clear(t_hittable_list *list)
{
	size_t	i;

	if (!list)
		return ;
	i = 0;
	while (i < list->count)
	{
		if (list->objects[i].owned && list->objects[i].object)
			free(list->objects[i].object);
		i++;
	}
	free(list->objects);
	list->objects = NULL;
	list->count = 0;
	list->capacity = 0;
	list->bbox = aabb_empty();
	list->fast = NULL;
	list->fast_hit = NULL;
}

bool	hittable_list_add_wrapper(t_hittable_list *list,
		const t_hittable_wrapper *wrap)
{
	size_t				newcap;
	t_hittable_wrapper	*newarr;

	if (list->count + 1 > list->capacity)
	{
		newcap = 4;
		if (list->capacity != 0)
			newcap = list->capacity * 2;
		newarr = (t_hittable_wrapper *)realloc(list->objects,
				newcap * sizeof(t_hittable_wrapper));
		if (!newarr)
			return (false);
		list->objects = newarr;
		list->capacity = newcap;
	}
	list->objects[list->count++] = *wrap;
	list->bbox = aabb_merge(&list->bbox, &wrap->bbox);
	return (true);
}

bool	hittable_list_add_sphere(t_hittable_list *list, const t_sphere *s)
{
	t_sphere			*copy;
	t_hittable_wrapper	wrap;

	copy = (t_sphere *)malloc(sizeof(t_sphere));
	if (!copy)
		return (false);
	*copy = *s;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_sphere;
	wrap.hit_noobj = sphere_hit_noobj;
	wrap.bbox = s->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}

bool	hittable_list_add_nonowned(t_hittable_list *list, void *obj,
		t_set_current_fn set_current, t_hit_noobj_fn hit_noobj,
		const t_aabb *bbox)
{
	t_hittable_wrapper	wrap;

	wrap.object = obj;
	wrap.owned = false;
	wrap.set_current = set_current;
	wrap.hit_noobj = hit_noobj;
	wrap.bbox = *bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
