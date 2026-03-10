/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_scene.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JSON_SCENE_H
# define JSON_SCENE_H

# include "json_parser.h"
# include "../rt/rt_parser.h"

/*
 * Parse a JSON scene file and populate the shared t_scene struct.
 * Returns true on success.
 */
bool	json_parse_scene(const char *filepath, t_scene *scene);

#endif
