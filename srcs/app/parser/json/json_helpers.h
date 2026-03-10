/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_helpers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JSON_HELPERS_H
# define JSON_HELPERS_H

# include "json_parser.h"
# include "../../core/math/vector.h"
# include "../../core/types/types.h"

/* ------------------------------------------------------------------ */
/*  Object field lookup                                               */
/* ------------------------------------------------------------------ */

/* Return the node for the given key, or NULL if not found / wrong type. */
t_json_node	*json_get(const t_json_node *obj, const char *key);

/* ------------------------------------------------------------------ */
/*  Type-safe value extraction                                        */
/* ------------------------------------------------------------------ */

/* Return the number value or `fallback` when the node is not a number. */
double		json_num(const t_json_node *n, double fallback);

/* Return the string pointer or `fallback` when not a string.           */
const char	*json_str(const t_json_node *n, const char *fallback);

/* Return the bool value or `fallback` when not a bool.                 */
bool		json_bool(const t_json_node *n, bool fallback);

/* ------------------------------------------------------------------ */
/*  Geometry helpers                                                  */
/* ------------------------------------------------------------------ */

/* Read a JSON array of 3 numbers into a t_vec3. Returns (0,0,0) on err */
t_vec3		json_to_vec3(const t_json_node *arr_node);

/* Read a JSON array of 3 numbers as a normalised colour (0-1 range).   */
t_color		json_to_color(const t_json_node *arr_node);

/* Read colour accepting both [r,g,b] 0-255  and  [r,g,b] 0-1 ranges.  */
t_color		json_to_color255(const t_json_node *arr_node);

/* ------------------------------------------------------------------ */
/*  Array iteration                                                   */
/* ------------------------------------------------------------------ */

/* Return item at index, or NULL if out of bounds or not an array.      */
t_json_node	*json_arr_at(const t_json_node *arr, size_t idx);

/* Return number of items (0 when not an array).                        */
size_t		json_arr_len(const t_json_node *arr);

#endif
