/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_flat_internal.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BVH_FLAT_INTERNAL_H
# define BVH_FLAT_INTERNAL_H

# include "bvh_flat.h"

/* One build-time primitive: its leaf trio, bbox and centroid (per axis). */
typedef struct s_build_prim
{
	t_flat_leaf	leaf;
	t_aabb		box;
	real_t		c[3];
}	t_build_prim;

typedef struct s_build_ctx
{
	t_build_prim	*prims;
	t_flat_node		*nodes;
	int				node_count;
}	t_build_ctx;

/* Precomputed ray data: float origin + inverse direction + per-axis sign. */
typedef struct s_flat_ray
{
	float	orig[3];
	float	inv[3];
	int		neg[3];
}	t_flat_ray;

/* Mutable traversal state threaded through the iterative walk. */
typedef struct s_trav
{
	const t_ray		*r;
	t_interval		rayt;
	t_hit_record	*rec;
	bool			hit;
}	t_trav;

extern __thread int	g_flat_sort_axis;

int	build_prim_cmp(const void *a, const void *b);
int	build_node(t_build_ctx *ctx, int start, int end);
void	fill_prim(t_build_prim *p, const t_hittable_wrapper *w);

#endif
