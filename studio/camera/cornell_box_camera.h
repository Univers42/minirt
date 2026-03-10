/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cornell_box_camera.h                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CORNELL_BOX_CAMERA_H
# define CORNELL_BOX_CAMERA_H

# include "camera.h"
# include "vector.h"

/*
 * Initialise camera for the classic Cornell-box scene.
 * 278×278×800 view, 40° vfov, 600 spp.
 */
static inline void	cornell_box_camera(t_camera *cam, int width)
{
	cam->aspect_ratio = 1.0;
	cam->image_width = width;
	cam->samples_per_pixel = 600;
	cam->max_depth = 50;
	cam->vfov = 40.0;
	cam->lookfrom = vec3_create(278.0, 278.0, -800.0);
	cam->lookat = vec3_create(278.0, 278.0, 0.0);
	cam->vup = vec3_create(0.0, 1.0, 0.0);
	cam->defocus_angle = 0.0;
	cam->focus_dist = 10.0;
	cam->background = vec3_create(0.0, 0.0, 0.0);
	camera_init(cam, cam->aspect_ratio, cam->image_width);
}

#endif
