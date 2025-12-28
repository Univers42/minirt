/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 15:18:52 by marvin            #+#    #+#             */
/*   Updated: 2025/12/28 15:18:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
# define CAMERA_H

# include "ray.h"
# include "point3.h"
# include "vec3.h"
# include <stdlib.h>

#ifndef CAMERA_WIDTH
# define CAMERA_WIDTH 800
#endif
#ifndef CAMERA_HEIGHT
# define CAMERA_HEIGHT 800
#endif

typedef struct s_camera
{
	double	distance;
	// camera position and orientation
	t_point3	eye;
	t_vec3		up;
	t_vec3		look_at;
	t_vec3		direction;
	double		fov;
	double		aspect;
	t_point3	upper_corner;
	// the horizontal and vertical size of the viewport (vectors)
	t_vec3		horizontal;
	t_vec3		vertical;
	// unit vector for camera axis
	t_vec3 		u;
	t_vec3		v;
	t_vec3		n;
}	t_camera;

/* constructors / lifecycle */
t_camera	*camera_new(const t_point3 *lookfrom, const t_vec3 *lookat, double fov);
void		camera_init(t_camera *cam, const t_point3 *lookfrom, const t_vec3 *lookat, double fov);
t_camera	*camera_copy(const t_camera *src);
void		camera_free(t_camera *cam);

/* ray construction */
t_ray		camera_construct_ray(const t_camera *cam, int i, int j);

#endif