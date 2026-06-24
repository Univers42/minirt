/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:59:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 18:59:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh.h"

static __thread const t_bvh_node	*g_current_bvh = NULL;

void	set_current_bvh(const void *obj)
{
	g_current_bvh = (const t_bvh_node *)obj;
}

static bool	bvh_hit_child(const t_hittable_wrapper *child, const t_ray *r,
		t_interval rayt, t_hit_record *rec)
{
	t_hit_record	temp_rec;

	if (!child->hit_noobj || !child->set_current)
		return (false);
	child->set_current(child->object);
	if (child->hit_noobj(r, rayt, &temp_rec))
	{
		*rec = temp_rec;
		return (true);
	}
	return (false);
}

bool	bvh_node_hit(const t_ray *r, t_interval rayt, t_hit_record *rec)
{
	const t_bvh_node	*node;
	t_interval			ray_t_copy;
	bool				hit_left;
	bool				hit_right;

	node = g_current_bvh;
	if (!node)
		return (false);
	ray_t_copy = rayt;
	if (!aabb_hit(&node->bbox, r, &ray_t_copy))
		return (false);
	hit_left = bvh_hit_child(&node->left, r, rayt, rec);
	if (hit_left)
		rayt.max = rec->t;
	hit_right = bvh_hit_child(&node->right, r, rayt, rec);
	return (hit_left || hit_right);
}
