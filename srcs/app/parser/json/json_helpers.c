/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "json_helpers.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/*  Object field lookup                                               */
/* ------------------------------------------------------------------ */

t_json_node	*json_get(const t_json_node *obj, const char *key)
{
	size_t	i;

	if (!obj || obj->type != JSON_OBJECT || !key)
		return (NULL);
	i = 0;
	while (i < obj->val.obj.count)
	{
		if (strncmp(obj->val.obj.pairs[i].key, key, JSON_STR_MAX) == 0)
			return (obj->val.obj.pairs[i].value);
		i++;
	}
	return (NULL);
}

/* ------------------------------------------------------------------ */
/*  Scalar extraction                                                 */
/* ------------------------------------------------------------------ */

double	json_num(const t_json_node *n, double fallback)
{
	if (!n || n->type != JSON_NUMBER)
		return (fallback);
	return (n->val.num);
}

const char	*json_str(const t_json_node *n, const char *fallback)
{
	if (!n || n->type != JSON_STRING)
		return (fallback);
	return (n->val.str);
}

bool	json_bool(const t_json_node *n, bool fallback)
{
	if (!n || n->type != JSON_BOOL)
		return (fallback);
	return (n->val.boolean);
}
