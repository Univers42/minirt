/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable_list.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 19:34:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 20:49:25 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <stdlib.h>
#include <string.h>
#include "hittable.h"
#include "object.h"

/* Wrapper stores object pointer, ownership and two per-type callbacks */
typedef void (*t_set_current_fn)(const void *obj);
typedef bool (*t_hit_noobj_fn)(const t_ray *r, real_t ray_tmin, real_t ray_tmax, t_hit_record *rec);

typedef struct s_hittable_wrapper
{
	void *object;
	bool owned;
	t_set_current_fn set_current;
	t_hit_noobj_fn hit_noobj;
} t_hittable_wrapper;

typedef struct s_hittable_list
{
	t_hittable_wrapper *objects;
	size_t count;
	size_t capacity;
} t_hittable_list;

/* Initialize an empty list */
static inline void hittable_list_init(t_hittable_list *list)
{
	list->objects = NULL;
	list->count = 0;
	list->capacity = 0;
}

/* Free internal storage and reset (frees owned objects) */
static inline void hittable_list_clear(t_hittable_list *list)
{
	if (!list)
		return;
	for (size_t i = 0; i < list->count; ++i)
		if (list->objects[i].owned && list->objects[i].object)
			free(list->objects[i].object);
	free(list->objects);
	list->objects = NULL;
	list->count = 0;
	list->capacity = 0;
}

/* Internal helper to grow/append a wrapper */
static inline bool hittable_list_add_wrapper(t_hittable_list *list, const t_hittable_wrapper *wrap)
{
	if (list->count + 1 > list->capacity)
	{
		size_t newcap = (list->capacity == 0) ? 4 : list->capacity * 2;
		t_hittable_wrapper *newarr = (t_hittable_wrapper *)realloc(list->objects, newcap * sizeof(t_hittable_wrapper));
		if (!newarr)
			return false;
		list->objects = newarr;
		list->capacity = newcap;
	}
	list->objects[list->count++] = *wrap;
	return true;
}

/* Convenience: add a sphere owned by the list (copy) */
static inline bool hittable_list_add_sphere(t_hittable_list *list, const t_sphere *s)
{
	t_sphere *copy = (t_sphere *)malloc(sizeof(t_sphere));
	if (!copy)
		return false;
	*copy = *s;
	t_hittable_wrapper wrap = {.object = copy, .owned = true, .set_current = set_current_sphere, .hit_noobj = sphere_hit_noobj};
	return hittable_list_add_wrapper(list, &wrap);
}

/* Append a pre-built wrapper (non-owned object). */
static inline bool hittable_list_add_nonowned(t_hittable_list *list, void *obj, t_set_current_fn set_current, t_hit_noobj_fn hit_noobj)
{
	t_hittable_wrapper wrap = {.object = obj, .owned = false, .set_current = set_current, .hit_noobj = hit_noobj};
	return hittable_list_add_wrapper(list, &wrap);
}

/* Iterate wrappers: bind object, call hit_noobj, track closest hit */
static inline bool hittable_list_hit(const t_hittable_list *list, const t_ray *r, real_t ray_tmin, real_t ray_tmax, t_hit_record *rec)
{
	bool hit_anything = false;
	double closest_so_far = (double)ray_tmax;
	t_hit_record temp_rec;

	for (size_t i = 0; i < list->count; ++i)
	{
		const t_hittable_wrapper *w = &list->objects[i];
		if (!w->set_current || !w->hit_noobj)
			continue;
		w->set_current(w->object);
		if (w->hit_noobj(r, ray_tmin, (real_t)closest_so_far, &temp_rec))
		{
			hit_anything = true;
			closest_so_far = (double)temp_rec.t;
			if (rec)
				*rec = temp_rec;
		}
	}
	return hit_anything;
}

#endif