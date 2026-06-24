/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_flat.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_FLAT_H
# define BVH_FLAT_H

# include "hittable.h"
# include "hittable_list.h"
# include "interval.h"
# include "aabb.h"
# include <stdbool.h>

/* Maximum interior-node depth pushed on the iterative traversal stack. */
# define FLAT_STACK_MAX 64

/* One leaf primitive: the original wrapper's two-step intersection trio.
   A leaf hit calls set_current(object) then hit_noobj(...) once -> the same
   double-precision routines as the wrapper path, so results match. */
typedef struct s_flat_leaf
{
	void				*object;
	t_set_current_fn	set_current;
	t_hit_noobj_fn		hit_noobj;
}	t_flat_leaf;

/* 32-byte cache-friendly node: float AABB + child/first index + count + axis.
   count == 0 -> interior (left child is index+1, right child is left_first).
   count  > 0 -> leaf (first primitive index is left_first). */
typedef struct s_flat_node
{
	float	bmin[3];
	float	bmax[3];
	int		left_first;
	int		count;
	int		axis;
}	t_flat_node;

typedef struct s_flat_bvh
{
	t_flat_node	*nodes;
	int			node_count;
	t_flat_leaf	*leaves;
	int			leaf_count;
}	t_flat_bvh;

/* bvh_flat.c */
t_flat_bvh	*flat_bvh_build(const t_hittable_list *world);
void		flat_bvh_free(t_flat_bvh *bvh);

/* bvh_flat2.c */
bool		flat_bvh_hit(const t_flat_bvh *bvh, const t_ray *r,
				t_interval rayt, t_hit_record *rec);

/* bvh_flat3.c */
bool		flat_bvh_hit_v(const void *bvh, const t_ray *r,
				t_interval rayt, t_hit_record *rec);
t_flat_bvh	*accel_attach_fast(t_hittable_list *accel,
				const t_hittable_list *world);

#endif
