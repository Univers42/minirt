/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_flat.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_flat_internal.h"
#include <stdlib.h>

__thread int	g_flat_sort_axis = 0;

int	build_prim_cmp(const void *a, const void *b)
{
	const t_build_prim	*pa = (const t_build_prim *)a;
	const t_build_prim	*pb = (const t_build_prim *)b;

	if (pa->c[g_flat_sort_axis] < pb->c[g_flat_sort_axis])
		return (-1);
	if (pa->c[g_flat_sort_axis] > pb->c[g_flat_sort_axis])
		return (1);
	return (0);
}

/* Span bbox over prims[start, end); also store it (as float) on node n. */
static t_aabb	range_box(t_build_ctx *ctx, int start, int end, t_flat_node *n)
{
	t_aabb	box;
	int		i;

	box = aabb_empty();
	i = start;
	while (i < end)
	{
		box = aabb_merge(&box, &ctx->prims[i].box);
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

static void	make_leaf(t_build_ctx *ctx, int self, int start, int end)
{
	ctx->nodes[self].left_first = start;
	ctx->nodes[self].count = end - start;
	ctx->nodes[self].axis = 0;
}

/* Recursively emit one node covering prims[start, end); returns its index. */
int	build_node(t_build_ctx *ctx, int start, int end)
{
	int		self;
	int		axis;
	int		mid;
	t_aabb	box;

	self = ctx->node_count++;
	box = range_box(ctx, start, end, &ctx->nodes[self]);
	if (end - start <= 1)
		return (make_leaf(ctx, self, start, end), self);
	axis = aabb_longest_axis(&box);
	g_flat_sort_axis = axis;
	qsort(&ctx->prims[start], (size_t)(end - start),
		sizeof(t_build_prim), build_prim_cmp);
	mid = start + (end - start) / 2;
	ctx->nodes[self].count = 0;
	ctx->nodes[self].axis = axis;
	build_node(ctx, start, mid);
	ctx->nodes[self].left_first = build_node(ctx, mid, end);
	return (self);
}

void	fill_prim(t_build_prim *p, const t_hittable_wrapper *w)
{
	p->leaf.object = w->object;
	p->leaf.set_current = w->set_current;
	p->leaf.hit_noobj = w->hit_noobj;
	p->box = w->bbox;
	p->c[0] = (w->bbox.x.min + w->bbox.x.max) * (real_t)0.5;
	p->c[1] = (w->bbox.y.min + w->bbox.y.max) * (real_t)0.5;
	p->c[2] = (w->bbox.z.min + w->bbox.z.max) * (real_t)0.5;
}
