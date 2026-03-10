/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_helpers2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "json_helpers.h"

/* ------------------------------------------------------------------ */
/*  Geometry extraction                                               */
/* ------------------------------------------------------------------ */

t_vec3	json_to_vec3(const t_json_node *arr_node)
{
	t_vec3	v;

	v.x = 0;
	v.y = 0;
	v.z = 0;
	if (!arr_node || arr_node->type != JSON_ARRAY)
		return (v);
	if (arr_node->val.arr.count < 3)
		return (v);
	v.x = (real_t)json_num(arr_node->val.arr.items[0], 0.0);
	v.y = (real_t)json_num(arr_node->val.arr.items[1], 0.0);
	v.z = (real_t)json_num(arr_node->val.arr.items[2], 0.0);
	return (v);
}

t_color	json_to_color(const t_json_node *arr_node)
{
	return (json_to_vec3(arr_node));
}

t_color	json_to_color255(const t_json_node *arr_node)
{
	t_color	c;

	c = json_to_vec3(arr_node);
	if (c.x > 1.0 || c.y > 1.0 || c.z > 1.0)
	{
		c.x /= 255.0;
		c.y /= 255.0;
		c.z /= 255.0;
	}
	return (c);
}

/* ------------------------------------------------------------------ */
/*  Array helpers                                                     */
/* ------------------------------------------------------------------ */

t_json_node	*json_arr_at(const t_json_node *arr, size_t idx)
{
	if (!arr || arr->type != JSON_ARRAY)
		return (NULL);
	if (idx >= arr->val.arr.count)
		return (NULL);
	return (arr->val.arr.items[idx]);
}

size_t	json_arr_len(const t_json_node *arr)
{
	if (!arr || arr->type != JSON_ARRAY)
		return (0);
	return (arr->val.arr.count);
}
