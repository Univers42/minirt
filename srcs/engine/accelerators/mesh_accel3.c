/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mesh_accel3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mesh_accel_internal.h"
#include <stdlib.h>

/* Fill one build ref from triangle i (bbox + centroid). */
static void	fill_ref(t_tri_ref *ref, const t_triangle *tri, int i)
{
	ref->index = i;
	ref->box = tri->bbox;
	ref->c[0] = (tri->bbox.x.min + tri->bbox.x.max) * (real_t)0.5;
	ref->c[1] = (tri->bbox.y.min + tri->bbox.y.max) * (real_t)0.5;
	ref->c[2] = (tri->bbox.z.min + tri->bbox.z.max) * (real_t)0.5;
}

/* Allocate the accel shell, its reordered triangle array, node array and
   the build-scratch refs. On any failure frees all and returns false. */
static bool	alloc_accel(t_mesh_accel **ma, t_mesh_build *ctx, int n)
{
	*ma = (t_mesh_accel *)malloc(sizeof(t_mesh_accel));
	ctx->refs = (t_tri_ref *)malloc((size_t)n * sizeof(t_tri_ref));
	ctx->out = (t_triangle *)malloc((size_t)n * sizeof(t_triangle));
	ctx->nodes = (t_mesh_node *)malloc((size_t)(2 * n) * sizeof(t_mesh_node));
	if (!*ma || !ctx->refs || !ctx->out || !ctx->nodes)
	{
		free(*ma);
		free(ctx->refs);
		free(ctx->out);
		free(ctx->nodes);
		return (*ma = NULL, false);
	}
	return (true);
}

/* Build a mesh-local flat BVH over mesh's triangles. Steals mesh->triangles
   ownership is NOT taken: triangles are copied (reordered) into the accel,
   so the caller still frees the source mesh with mesh_clear(). */
t_mesh_accel	*mesh_accel_build(t_mesh *mesh)
{
	t_mesh_accel	*ma;
	t_mesh_build	ctx;
	int				i;

	if (!mesh || mesh->count == 0)
		return (NULL);
	if (!alloc_accel(&ma, &ctx, (int)mesh->count))
		return (NULL);
	ctx.src = mesh->triangles;
	ctx.node_count = 0;
	ctx.out_count = 0;
	i = -1;
	while (++i < (int)mesh->count)
		fill_ref(&ctx.refs[i], &mesh->triangles[i], i);
	mesh_build_node(&ctx, 0, (int)mesh->count);
	free(ctx.refs);
	ma->tris = ctx.out;
	ma->tri_count = ctx.out_count;
	ma->nodes = ctx.nodes;
	ma->node_count = ctx.node_count;
	ma->bbox = mesh->bbox;
	return (ma);
}

void	mesh_accel_free(t_mesh_accel *ma)
{
	if (!ma)
		return ;
	free(ma->tris);
	free(ma->nodes);
	free(ma);
}
