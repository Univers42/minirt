/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 15:19:05 by marvin            #+#    #+#             */
/*   Updated: 2025/12/28 15:19:05 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "camera.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

static void camera_build(const t_point3 eye, const t_vec3 lookat, double fovy, t_camera *cam)
{
    /* camera geometry */
    cam->distance = 1.0;
    cam->eye = eye;
    cam->look_at = lookat;
    cam->direction = vec3_normalize(vec3_init(lookat.x - eye.x, lookat.y - eye.y, lookat.z - eye.z));
    cam->aspect = (double)CAMERA_WIDTH / (double)CAMERA_HEIGHT;
    cam->fov = fovy;

    /* camera axis: n = normalize(eye - look_at) ; u = normalize(cross(up, n)) ; v = cross(n, u) */
    cam->n = vec3_normalize(vec3_init(eye.x - lookat.x, eye.y - lookat.y, eye.z - lookat.z));
    /* default up if zero */
    if (cam->up.x == 0.0 && cam->up.y == 0.0 && cam->up.z == 0.0)
        cam->up = vec3_init(0.0, 1.0, 0.0);
    cam->u = vec3_normalize(vec3_cross(cam->up, cam->n));
    cam->v = vec3_cross(cam->n, cam->u);

    /* viewport size at distance = 1 */
    double theta = (cam->fov * M_PI) / 180.0;
    double half_height = tan(theta * 0.5) * cam->distance;
    double half_width  = cam->aspect * half_height;

    /* center of image plane (as vec3) */
    t_vec3 center = vec3_scale(cam->direction, cam->distance);

    /* upper-left corner: eye + center - u*half_width + v*half_height */
    t_vec3 eye_v = vec3_init(cam->eye.x, cam->eye.y, cam->eye.z);
    t_vec3 uc = vec3_add(eye_v, center);
    uc = vec3_add(uc, vec3_sub(vec3_scale(cam->v, half_height), vec3_scale(cam->u, half_width)));

    cam->upper_corner = point3_from_vec3(&uc);

    /* full horizontal and vertical vectors across the image plane */
    cam->horizontal = vec3_scale(cam->u, 2.0 * half_width);
    cam->vertical   = vec3_scale(cam->v, 2.0 * half_height);
}

t_camera *camera_new(t_point3 lookfrom, t_vec3 lookat, double fov)
{
	t_camera *cam = (t_camera *)malloc(sizeof(*cam));
	if (!cam)
		return NULL;
	cam->up = vec3_init(0.0, 1.0, 0.0);
	camera_build(lookfrom, lookat, fov, cam);
	return cam;
}

void camera_init(t_camera *cam, t_point3 lookfrom, t_vec3 lookat, double fov)
{
	if (!cam) return;
	cam->up = vec3_init(0.0, 1.0, 0.0);
	camera_build(lookfrom, lookat, fov, cam);
}

t_camera *camera_copy(const t_camera *src)
{
	if (!src) return NULL;
	t_camera *dst = (t_camera *)malloc(sizeof(*dst));
	if (!dst) return NULL;
	*dst = *src;
	return dst;
}

void camera_free(t_camera *cam)
{
	if (cam) free(cam);
}

t_ray camera_construct_ray(const t_camera *cam, int i, int j)
{
    double u = ((double)i + 0.5) / (double)CAMERA_WIDTH;
    double v = ((double)j + 0.5) / (double)CAMERA_HEIGHT;

    t_vec3 uc = vec3_init(cam->upper_corner.x, cam->upper_corner.y, cam->upper_corner.z);
    t_vec3 offs_h = vec3_scale(cam->horizontal, u);
    t_vec3 offs_v = vec3_scale(cam->vertical, v);
    t_vec3 pixel = vec3_sub(vec3_add(uc, offs_h), offs_v);

    t_vec3 eye_v = vec3_init(cam->eye.x, cam->eye.y, cam->eye.z);
    t_vec3 dir = vec3_normalize(vec3_sub(pixel, eye_v));

    t_ray ray;
    ray.origin = cam->eye;
    ray.direction = dir;
    return ray;
}