/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:59:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 18:59:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh.h"

int	bvh_box_compare(const void *a, const void *b, int axis_index)
{
	const t_hittable_wrapper	*wa;
	const t_hittable_wrapper	*wb;
	real_t						a_min;
	real_t						b_min;

	wa = (const t_hittable_wrapper *)a;
	wb = (const t_hittable_wrapper *)b;
	a_min = aabb_axis_interval(&wa->bbox, axis_index)->min;
	b_min = aabb_axis_interval(&wb->bbox, axis_index)->min;
	if (a_min < b_min)
		return (-1);
	else if (a_min > b_min)
		return (1);
	return (0);
}

int	bvh_box_x_compare(const void *a, const void *b)
{
	return (bvh_box_compare(a, b, 0));
}

int	bvh_box_y_compare(const void *a, const void *b)
{
	return (bvh_box_compare(a, b, 1));
}

int	bvh_box_z_compare(const void *a, const void *b)
{
	return (bvh_box_compare(a, b, 2));
}

static __thread const t_bvh_node	*g_current_bvh = NULL;

void	set_current_bvh(const void *obj)
{
	g_current_bvh = (const t_bvh_node *)obj;
}

bool	bvh_node_hit(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_bvh_node	*node;
	t_interval			ray_t_copy;
	bool				hit_left;
	bool				hit_right;
	t_hit_record		temp_rec;

	node = g_current_bvh;
	if (!node)
		return (false);
	ray_t_copy = rayt;
	if (!aabb_hit(&node->bbox, r, &ray_t_copy))
		return (false);
	hit_left = false;
	if (node->left.hit_noobj && node->left.set_current)
	{
		node->left.set_current(node->left.object);
		hit_left = node->left.hit_noobj(r, rayt, &temp_rec);
		if (hit_left)
		{
			*rec = temp_rec;
			rayt.max = temp_rec.t;
		}
	}
	hit_right = false;
	if (node->right.hit_noobj && node->right.set_current)
	{
		node->right.set_current(node->right.object);
		hit_right = node->right.hit_noobj(r, rayt, &temp_rec);
		if (hit_right)
			*rec = temp_rec;
	}
	return (hit_left || hit_right);
}

t_bvh_node	*bvh_node_build(t_hittable_wrapper *objects, size_t start,
	size_t end)
{
	t_bvh_node			*node;
	size_t				object_span;
	t_aabb				span_bbox;
	int					axis;
	t_comparator_fn		comparator;
	size_t				i;
	t_hittable_wrapper	tmp;
	size_t				mid;
	t_bvh_node			*left_node;
	t_bvh_node			*right_node;

	if (!objects || start >= end)
		return (NULL);
	node = (t_bvh_node *)malloc(sizeof(t_bvh_node));
	if (!node)
		return (NULL);
	object_span = end - start;
	span_bbox = aabb_empty();
	i = start;
	while (i < end)
	{
		span_bbox = aabb_merge(&span_bbox, &objects[i].bbox);
		i++;
	}
	axis = aabb_longest_axis(&span_bbox);
	if (axis == 0)
		comparator = bvh_box_x_compare;
	else if (axis == 1)
		comparator = bvh_box_y_compare;
	else
		comparator = bvh_box_z_compare;
	if (object_span == 1)
	{
		node->left = objects[start];
		node->right.object = NULL;
		node->right.owned = false;
		node->right.set_current = NULL;
		node->right.hit_noobj = NULL;
		node->bbox = objects[start].bbox;
		return (node);
	}
	if (object_span == 2)
	{
		if (comparator(&objects[start], &objects[start + 1]) > 0)
		{
			tmp = objects[start];
			objects[start] = objects[start + 1];
			objects[start + 1] = tmp;
		}
		node->left = objects[start];
		node->right = objects[start + 1];
		node->bbox = aabb_merge(&objects[start].bbox,
				&objects[start + 1].bbox);
		return (node);
	}
	qsort(&objects[start], object_span, sizeof(t_hittable_wrapper),
		comparator);
	mid = start + object_span / 2;
	left_node = bvh_node_build(objects, start, mid);
	right_node = bvh_node_build(objects, mid, end);
	if (!left_node || !right_node)
	{
		free(node);
		if (left_node)
			free(left_node);
		if (right_node)
			free(right_node);
		return (NULL);
	}
	node->left.object = (void *)left_node;
	node->left.owned = true;
	node->left.set_current = set_current_bvh;
	node->left.hit_noobj = bvh_node_hit;
	node->left.bbox = left_node->bbox;
	node->right.object = (void *)right_node;
	node->right.owned = true;
	node->right.set_current = set_current_bvh;
	node->right.hit_noobj = bvh_node_hit;
	node->right.bbox = right_node->bbox;
	node->bbox = aabb_merge(&left_node->bbox, &right_node->bbox);
	return (node);
}

t_bvh_node	*bvh_node_create(t_hittable_list *world)
{
	t_hittable_wrapper	*objects;
	t_bvh_node			*root;
	size_t				bytes;
	size_t				i;

	if (!world || world->count == 0)
		return (NULL);
	bytes = world->count * sizeof(t_hittable_wrapper);
	objects = (t_hittable_wrapper *)malloc(bytes);
	if (!objects)
		return (NULL);
	memcpy(objects, world->objects, bytes);
	i = 0;
	while (i < world->count)
	{
		objects[i].owned = false;
		i++;
	}
	root = bvh_node_build(objects, 0, world->count);
	free(objects);
	return (root);
}

void	bvh_node_destroy(t_bvh_node *node)
{
	if (!node)
		return ;
	if (node->left.object && node->left.owned)
	{
		if (node->left.set_current == set_current_bvh)
			bvh_node_destroy((t_bvh_node *)node->left.object);
		else
			free(node->left.object);
	}
	if (node->right.object && node->right.owned)
	{
		if (node->right.set_current == set_current_bvh)
			bvh_node_destroy((t_bvh_node *)node->right.object);
		else
			free(node->right.object);
	}
	free(node);
}
