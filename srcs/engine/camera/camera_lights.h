/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera_lights.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 20:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 19:47:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_LIGHTS_H
# define CAMERA_LIGHTS_H

# include "types.h"
# include "vector.h"
# include "hittable_list.h"

# ifndef RT_MAX_LIGHTS
#  define RT_MAX_LIGHTS	16
# endif

typedef struct s_light_info
{
	t_point3	pos;
	t_color		emission;
	real_t		radius;
}	t_light_info;

/* Read-only light cache (populated once before render; shared with the
   deterministic shading engine). */
extern t_light_info	g_lights[RT_MAX_LIGHTS];
extern int			g_light_count;

void	set_scene_lights(const t_light_info *infos, int count);
t_color	sample_direct_lights(const t_hit_record *rec,
			const t_hittable_list *world);

#endif
