/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bvh4.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 18:59:04 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 18:59:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bvh.h"

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

static void	bvh_destroy_child(t_hittable_wrapper *w)
{
	if (w->object && w->owned)
	{
		if (w->set_current == set_current_bvh)
			bvh_node_destroy((t_bvh_node *)w->object);
		else
			free(w->object);
	}
}

void	bvh_node_destroy(t_bvh_node *node)
{
	if (!node)
		return ;
	bvh_destroy_child(&node->left);
	bvh_destroy_child(&node->right);
	free(node);
}
