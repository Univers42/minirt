/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   triangle4.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 18:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:44:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "triangle.h"
#include "hittable_list.h"
#include <stdlib.h>

static __thread const t_triangle	*g_current_triangle = NULL;

void	set_current_triangle(const void *obj)
{
	g_current_triangle = (const t_triangle *)obj;
}

bool	triangle_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_triangle)
		return (false);
	return (triangle_hit(g_current_triangle, r, rayt, rec));
}

bool	hittable_list_add_triangle(t_hittable_list *list,
		const t_triangle *tri)
{
	t_triangle			*copy;
	t_hittable_wrapper	wrap;

	if (!list || !tri)
		return (false);
	copy = (t_triangle *)malloc(sizeof(t_triangle));
	if (!copy)
		return (false);
	*copy = *tri;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_triangle;
	wrap.hit_noobj = triangle_hit_noobj;
	wrap.bbox = tri->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
