/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_parser.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJ_PARSER_H
# define OBJ_PARSER_H

# include "types.h"
# include "vector.h"
# include "triangle.h"
# include "hittable_list.h"
# include "material.h"
# include <stdbool.h>

/*
** High-level OBJ parser API.
** Thin wrapper around vendor/wavefront library.
** All functions return true on success, false on failure.
*/

/* Parse OBJ file into a ray-tracer mesh (centered, normalized) */
bool	obj_parse_to_mesh(const char *path, t_mesh *mesh,
			t_material *mat, float target_size);

/* Parse OBJ and translate to position */
bool	obj_parse_to_mesh_at(const char *path, t_mesh *mesh,
			t_material *mat, float target_size,
			float px, float py, float pz);

/* Parse OBJ and add triangles directly to a hittable list */
bool	obj_parse_to_list(const char *path, t_hittable_list *list,
			t_material *mat, float target_size,
			float px, float py, float pz);

#endif
