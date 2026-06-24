/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh5.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:59:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 18:59:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh.h"

t_aabb	bvh_span_bbox(t_hittable_wrapper *objects, size_t start, size_t end)
{
	t_aabb	span_bbox;
	size_t	i;

	span_bbox = aabb_empty();
	i = start;
	while (i < end)
	{
		span_bbox = aabb_merge(&span_bbox, &objects[i].bbox);
		i++;
	}
	return (span_bbox);
}

void	bvh_build_leaf(t_bvh_node *node, t_hittable_wrapper *objects,
		size_t start)
{
	node->left = objects[start];
	node->right.object = NULL;
	node->right.owned = false;
	node->right.set_current = NULL;
	node->right.hit_noobj = NULL;
	node->bbox = objects[start].bbox;
}

void	bvh_build_pair(t_bvh_node *node, t_hittable_wrapper *objects,
		size_t start, t_comparator_fn comparator)
{
	t_hittable_wrapper	tmp;

	if (comparator(&objects[start], &objects[start + 1]) > 0)
	{
		tmp = objects[start];
		objects[start] = objects[start + 1];
		objects[start + 1] = tmp;
	}
	node->left = objects[start];
	node->right = objects[start + 1];
	node->bbox = aabb_merge(&objects[start].bbox, &objects[start + 1].bbox);
}
