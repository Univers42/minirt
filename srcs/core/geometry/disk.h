/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disk.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISK_H
# define DISK_H

# include "types.h"
# include "vector.h"
# include "ray.h"
# include "hittable.h"
# include "hittable_list.h"
# include "aabb.h"

/* Finite flat disk (equation-based): a ray-plane intersection accepted only
   where length(p - center) <= radius.  bbox is the box of center +/- radius
   in every axis, so it integrates with the BVH like any bounded primitive. */
typedef struct s_disk
{
	t_point3	center;
	t_vec3		normal;
	real_t		radius;
	t_material	*mat;
	t_aabb		bbox;
}	t_disk;

t_disk	disk_create(const t_point3 *center, const t_vec3 *normal,
			real_t radius, t_material *mat);
bool	disk_hit(const t_disk *disk, const t_ray *r, t_interval rayt,
			t_hit_record *rec);
void	set_current_disk(const void *obj);
bool	disk_hit_noobj(const t_ray *r, t_interval rayt, t_hit_record *rec);
bool	hittable_list_add_disk(t_hittable_list *list, const t_disk *disk);

#endif
