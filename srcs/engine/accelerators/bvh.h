/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 16:38:12 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:00:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_H
# define BVH_H

# include "types.h"
# include "aabb.h"
# include "hittable.h"
# include "hittable_list.h"
# include "interval.h"
# include "random.h"
# include <stdlib.h>
# include <string.h>

/* Forward declaration */
typedef struct s_bvh_node	t_bvh_node;

/* BVH node structure: stores children as wrappers and bounding box */
typedef struct s_bvh_node
{
	t_hittable_wrapper	left;
	t_hittable_wrapper	right;
	t_aabb				bbox;
}	t_bvh_node;

/* Comparator function type for qsort */
typedef int					(*t_comparator_fn)(const void *a, const void *b);

int				bvh_box_compare(const void *a, const void *b, int axis_index);
int				bvh_box_x_compare(const void *a, const void *b);
int				bvh_box_y_compare(const void *a, const void *b);
int				bvh_box_z_compare(const void *a, const void *b);
t_comparator_fn	bvh_axis_comparator(int axis);
void			set_current_bvh(const void *obj);
bool			bvh_node_hit(const t_ray *r, t_interval rayt,
					t_hit_record *rec);
t_bvh_node		*bvh_node_build(t_hittable_wrapper *objects, size_t start,
					size_t end);
t_bvh_node		*bvh_node_create(t_hittable_list *world);
void			bvh_node_destroy(t_bvh_node *node);
t_aabb			bvh_span_bbox(t_hittable_wrapper *objects, size_t start,
					size_t end);
void			bvh_build_leaf(t_bvh_node *node, t_hittable_wrapper *objects,
					size_t start);
void			bvh_build_pair(t_bvh_node *node, t_hittable_wrapper *objects,
					size_t start, t_comparator_fn comparator);

#endif
