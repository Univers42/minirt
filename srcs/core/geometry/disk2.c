/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disk2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "disk.h"
#include <stdlib.h>

/* Per-thread "current object" for the two-step wrapper dispatch. */
static __thread const t_disk	*g_current_disk;

void	set_current_disk(const void *obj)
{
	g_current_disk = (const t_disk *)obj;
}

bool	disk_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	if (!g_current_disk)
		return (false);
	return (disk_hit(g_current_disk, r, rayt, rec));
}

bool	hittable_list_add_disk(t_hittable_list *list, const t_disk *disk)
{
	t_disk				*copy;
	t_hittable_wrapper	wrap;

	if (!list || !disk)
		return (false);
	copy = (t_disk *)malloc(sizeof(t_disk));
	if (!copy)
		return (false);
	*copy = *disk;
	wrap.object = copy;
	wrap.owned = true;
	wrap.set_current = set_current_disk;
	wrap.hit_noobj = disk_hit_noobj;
	wrap.bbox = disk->bbox;
	return (hittable_list_add_wrapper(list, &wrap));
}
