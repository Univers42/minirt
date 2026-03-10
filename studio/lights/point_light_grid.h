/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   point_light_grid.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:47 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POINT_LIGHT_GRID_H
# define POINT_LIGHT_GRID_H

# include "hittable_list.h"
# include "sphere.h"
# include "material.h"
# include "vector.h"

/*
 * Place a grid of small emissive spheres as point lights.
 * `origin`   – bottom-left corner of the grid
 * `nx`, `nz` – count along X and Z
 * `spacing`  – distance between lights
 * `color`    – emission colour (not clamped; intensity encoded in mag)
 */
static inline bool	point_light_grid(t_hittable_list *world,
		const t_point3 *origin, int nx, int nz,
		real_t spacing, t_color color)
{
	int			ix;
	int			iz;
	t_material	*mat;
	t_point3	pos;
	t_sphere	s;

	iz = 0;
	while (iz < nz)
	{
		ix = 0;
		while (ix < nx)
		{
			mat = diffuse_light_create(color);
			if (!mat)
				return (false);
			pos = vec3_create(origin->x + ix * spacing,
					origin->y, origin->z + iz * spacing);
			s = create_sphere(&pos, 0.15, color, mat);
			if (!hittable_list_add_sphere(world, &s))
				return (false);
			ix++;
		}
		iz++;
	}
	return (true);
}

#endif
