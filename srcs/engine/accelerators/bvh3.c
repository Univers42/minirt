/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh3.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:59:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 18:59:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh.h"

static void	bvh_link_child(t_hittable_wrapper *w, t_bvh_node *child)
{
	w->object = (void *)child;
	w->owned = true;
	w->set_current = set_current_bvh;
	w->hit_noobj = bvh_node_hit;
	w->bbox = child->bbox;
}

static t_bvh_node	*bvh_build_internal(t_bvh_node *node,
		t_hittable_wrapper *objects, size_t start, size_t end)
{
	size_t		mid;
	t_bvh_node	*child[2];

	mid = start + (end - start) / 2;
	child[0] = bvh_node_build(objects, start, mid);
	child[1] = bvh_node_build(objects, mid, end);
	if (!child[0] || !child[1])
		return (free(node), free(child[0]), free(child[1]), NULL);
	bvh_link_child(&node->left, child[0]);
	bvh_link_child(&node->right, child[1]);
	node->bbox = aabb_merge(&child[0]->bbox, &child[1]->bbox);
	return (node);
}

t_bvh_node	*bvh_node_build(t_hittable_wrapper *objects, size_t start,
	size_t end)
{
	t_bvh_node		*node;
	t_aabb			span_bbox;
	t_comparator_fn	comparator;

	if (!objects || start >= end)
		return (NULL);
	node = (t_bvh_node *)malloc(sizeof(t_bvh_node));
	if (!node)
		return (NULL);
	span_bbox = bvh_span_bbox(objects, start, end);
	comparator = bvh_axis_comparator(aabb_longest_axis(&span_bbox));
	if (end - start == 1)
		return (bvh_build_leaf(node, objects, start), node);
	if (end - start == 2)
		return (bvh_build_pair(node, objects, start, comparator), node);
	qsort(&objects[start], end - start, sizeof(t_hittable_wrapper),
		comparator);
	return (bvh_build_internal(node, objects, start, end));
}
