/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   room_test_camera.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:46 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROOM_TEST_CAMERA_H
# define ROOM_TEST_CAMERA_H

# include "camera.h"
# include "vector.h"

/*
 * Camera preset for an interior room test scene.
 * Eye at (5,3,8) looking towards the origin, 60° fov.
 */
static inline void	room_test_camera(t_camera *cam, int width)
{
	cam->aspect_ratio = 16.0 / 9.0;
	cam->image_width = width;
	cam->samples_per_pixel = 200;
	cam->max_depth = 50;
	cam->vfov = 60.0;
	cam->lookfrom = vec3_create(5.0, 3.0, 8.0);
	cam->lookat = vec3_create(0.0, 1.0, 0.0);
	cam->vup = vec3_create(0.0, 1.0, 0.0);
	cam->defocus_angle = 0.3;
	cam->focus_dist = 10.0;
	cam->background = vec3_create(0.05, 0.05, 0.08);
	camera_init(cam, cam->aspect_ratio, cam->image_width);
}

#endif
