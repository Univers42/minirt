/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   room.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROOM_H
# define ROOM_H

# include "common.h"
# include "quad.h"
# include "material.h"

/* ------------------------------------------------------------------ */
/*  Simple room (6 quads: floor, ceiling, 4 walls)                    */
/* ------------------------------------------------------------------ */

void	build_room(t_hittable_list *world,
			const t_point3 *corner, real_t w, real_t h, real_t d,
			t_material *floor_mat, t_material *wall_mat,
			t_material *ceil_mat);

/* Cornell box classic: 555×555×555, red/green side walls, white others */
void	build_cornell_box(t_hittable_list *world);

#endif
