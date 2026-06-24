/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hittable_list.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 19:34:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:20:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HITTABLE_LIST_H
# define HITTABLE_LIST_H

# include <stdlib.h>
# include <string.h>
# include "interval.h"
# include "hittable.h"
# include "aabb.h"
# include "sphere.h"

/* Optional fast-path override: when fast_hit is set, hittable_list_hit
   dispatches to it instead of looping wrappers (used by the flat SAH BVH).
   Stored as opaque void* so core/geometry stays engine-agnostic. */
typedef bool	(*t_fast_hit_fn)(const void *fast, const t_ray *r,
					t_interval rayt, t_hit_record *rec);

typedef struct s_hittable_list
{
	t_hittable_wrapper	*objects;
	size_t				count;
	size_t				capacity;
	t_aabb				bbox;
	const void			*fast;
	t_fast_hit_fn		fast_hit;
}	t_hittable_list;

void				hittable_list_init(t_hittable_list *list);
void				hittable_list_clear(t_hittable_list *list);
bool				hittable_list_add_wrapper(t_hittable_list *list,
						const t_hittable_wrapper *wrap);
bool				hittable_list_add_sphere(t_hittable_list *list,
						const t_sphere *s);
bool				hittable_list_add_nonowned(t_hittable_list *list,
						void *obj, t_set_current_fn set_current,
						t_hit_noobj_fn hit_noobj, const t_aabb *bbox);
t_aabb				hittable_list_bounding_box(const t_hittable_list *list);
bool				hittable_list_hit(const t_hittable_list *list,
						const t_ray *r, t_interval rayt, t_hit_record *rec);
void				set_current_hlist(const void *obj);
bool				hittable_list_hit_noobj(const t_ray *r,
						t_interval rayt, t_hit_record *rec);
t_hittable_wrapper	hittable_list_wrapper(const t_hittable_list *list);

#endif