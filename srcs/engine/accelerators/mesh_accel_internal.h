/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh_accel_internal.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESH_ACCEL_INTERNAL_H
# define MESH_ACCEL_INTERNAL_H

# include "mesh_accel.h"

/* Build-time per-triangle reference: index into the triangle array, its
   bbox and per-axis centroid (sorted to partition the BVH). */
typedef struct s_tri_ref
{
	int		index;
	t_aabb	box;
	real_t	c[3];
}	t_tri_ref;

typedef struct s_mesh_build
{
	t_tri_ref	*refs;
	t_triangle	*src;
	t_triangle	*out;
	t_mesh_node	*nodes;
	int			node_count;
	int			out_count;
}	t_mesh_build;

/* Precomputed ray data: float origin + inverse direction + per-axis sign. */
typedef struct s_mesh_ray
{
	float	orig[3];
	float	inv[3];
	int		neg[3];
}	t_mesh_ray;

/* Mutable traversal state threaded through the iterative walk. */
typedef struct s_mesh_trav
{
	const t_ray		*r;
	t_interval		rayt;
	t_hit_record	*rec;
	bool			hit;
}	t_mesh_trav;

extern __thread int	g_mesh_sort_axis;

int			mesh_ref_cmp(const void *a, const void *b);
int			mesh_build_node(t_mesh_build *ctx, int start, int end);
t_mesh_ray	mesh_make_ray(const t_ray *r);
void		mesh_accel_walk(const t_mesh_accel *m, const t_mesh_ray *mr,
				t_mesh_trav *tv);

#endif
