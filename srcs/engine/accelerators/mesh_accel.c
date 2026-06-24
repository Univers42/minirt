/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh_accel.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mesh_accel_internal.h"
#include <stdlib.h>

__thread int	g_mesh_sort_axis = 0;

int	mesh_ref_cmp(const void *a, const void *b)
{
	const t_tri_ref	*ra = (const t_tri_ref *)a;
	const t_tri_ref	*rb = (const t_tri_ref *)b;

	if (ra->c[g_mesh_sort_axis] < rb->c[g_mesh_sort_axis])
		return (-1);
	if (ra->c[g_mesh_sort_axis] > rb->c[g_mesh_sort_axis])
		return (1);
	return (0);
}

/* Span bbox over refs[start, end); also store it (as float) on node n. */
static t_aabb	range_box(t_mesh_build *ctx, int start, int end, t_mesh_node *n)
{
	t_aabb	box;
	int		i;

	box = aabb_empty();
	i = start;
	while (i < end)
	{
		box = aabb_merge(&box, &ctx->refs[i].box);
		i++;
	}
	n->bmin[0] = (float)box.x.min;
	n->bmin[1] = (float)box.y.min;
	n->bmin[2] = (float)box.z.min;
	n->bmax[0] = (float)box.x.max;
	n->bmax[1] = (float)box.y.max;
	n->bmax[2] = (float)box.z.max;
	return (box);
}

/* Emit a leaf: copy its triangles contiguously into the out array so a
   traversal leaf is a flat [first, first+count) range over tris. */
static void	make_leaf(t_mesh_build *ctx, int self, int start, int end)
{
	int	first;
	int	i;

	first = ctx->out_count;
	i = start;
	while (i < end)
	{
		ctx->out[ctx->out_count++] = ctx->src[ctx->refs[i].index];
		i++;
	}
	ctx->nodes[self].left_first = first;
	ctx->nodes[self].count = end - start;
	ctx->nodes[self].axis = 0;
}

int	mesh_build_node(t_mesh_build *ctx, int start, int end)
{
	int		self;
	int		axis;
	int		mid;
	t_aabb	box;

	self = ctx->node_count++;
	box = range_box(ctx, start, end, &ctx->nodes[self]);
	if (end - start <= 2)
		return (make_leaf(ctx, self, start, end), self);
	axis = aabb_longest_axis(&box);
	g_mesh_sort_axis = axis;
	qsort(&ctx->refs[start], (size_t)(end - start),
		sizeof(t_tri_ref), mesh_ref_cmp);
	mid = start + (end - start) / 2;
	ctx->nodes[self].count = 0;
	ctx->nodes[self].axis = axis;
	mesh_build_node(ctx, start, mid);
	ctx->nodes[self].left_first = mesh_build_node(ctx, mid, end);
	return (self);
}
