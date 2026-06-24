/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh_flat3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh_flat_internal.h"
#include <stdlib.h>

/* Allocate the bvh shell plus the build scratch (prims) and node arrays. */
static bool	alloc_build(t_flat_bvh **bvh, t_build_ctx *ctx, size_t n)
{
	*bvh = (t_flat_bvh *)malloc(sizeof(t_flat_bvh));
	ctx->prims = (t_build_prim *)malloc(n * sizeof(t_build_prim));
	ctx->nodes = (t_flat_node *)malloc(2 * n * sizeof(t_flat_node));
	if (!*bvh || !ctx->prims || !ctx->nodes)
	{
		free(*bvh);
		free(ctx->prims);
		free(ctx->nodes);
		return (*bvh = NULL, false);
	}
	return (true);
}

static bool	copy_leaves(t_flat_bvh *bvh, const t_build_prim *prims, size_t n)
{
	size_t	i;

	bvh->leaves = (t_flat_leaf *)malloc(n * sizeof(t_flat_leaf));
	if (!bvh->leaves)
		return (false);
	i = 0;
	while (i < n)
	{
		bvh->leaves[i] = prims[i].leaf;
		i++;
	}
	bvh->leaf_count = (int)n;
	return (true);
}

t_flat_bvh	*flat_bvh_build(const t_hittable_list *world)
{
	t_flat_bvh	*bvh;
	t_build_ctx	ctx;
	size_t		i;

	if (!world || world->count == 0)
		return (NULL);
	if (!alloc_build(&bvh, &ctx, world->count))
		return (NULL);
	i = 0;
	while (i < world->count)
	{
		fill_prim(&ctx.prims[i], &world->objects[i]);
		i++;
	}
	ctx.node_count = 0;
	build_node(&ctx, 0, (int)world->count);
	bvh->nodes = ctx.nodes;
	bvh->node_count = ctx.node_count;
	if (!copy_leaves(bvh, ctx.prims, world->count))
		return (free(ctx.prims), flat_bvh_free(bvh), NULL);
	free(ctx.prims);
	return (bvh);
}

void	flat_bvh_free(t_flat_bvh *bvh)
{
	if (!bvh)
		return ;
	free(bvh->nodes);
	free(bvh->leaves);
	free(bvh);
}
