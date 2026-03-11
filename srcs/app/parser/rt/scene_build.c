/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scene_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:11:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "studio_config.h"
#include "sphere.h"
#include "cylinder.h"
#include "quad.h"
#include "triangle.h"
#include "material.h"
#include "camera.h"
#include "camera_lights.h"
#include "bvh.h"
#include "settings.h"
#include <stdlib.h>
#include <math.h>

/* scene_build2.c */
t_material	*create_material(const t_mat_spec *ms, t_color clr);
bool		build_quad_obj(t_hittable_list *world, const t_rt_object *obj);
bool		build_mesh_obj(t_hittable_list *world, const t_rt_object *obj);

/* ------------------------------------------------------------------ */
/*  Build a sphere engine object from parsed data                     */
/* ------------------------------------------------------------------ */

static bool	build_sphere(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_sphere	s;

	mat = create_material(&obj->mat, obj->data.sphere.color);
	if (!mat)
		return (false);
	s = create_sphere(&obj->data.sphere.center,
			obj->data.sphere.diameter / 2.0, obj->data.sphere.color, mat);
	return (hittable_list_add_sphere(world, &s));
}

/* ------------------------------------------------------------------ */
/*  Build a plane as a very large quad from parsed data                */
/*  No plane primitive — synthesize from point + normal               */
/*  Tangent vectors computed via cross product with a non-parallel     */
/*  reference vector.                                                 */
/* ------------------------------------------------------------------ */

static t_vec3	compute_tangent(const t_vec3 *normal)
{
	t_vec3	ref;
	t_vec3	tangent;
	double	len;

	if (fabs(normal->x) < 0.9)
		ref = vec3_create(1.0, 0.0, 0.0);
	else
		ref = vec3_create(0.0, 1.0, 0.0);
	tangent = cross(normal, &ref);
	len = vec3_length(&tangent);
	if (len < 1e-10)
		return (vec3_create(1.0, 0.0, 0.0));
	return (vec3_mul_scalar(&tangent, 1.0 / len));
}

static bool	build_plane(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_vec3		n;
	t_vec3		u_dir;
	t_vec3		v_dir;
	t_vec3		u_vec;
	t_vec3		v_vec;
	t_point3	corner;
	t_quad		q;
	t_quad		*qp;

	mat = create_material(&obj->mat, obj->data.plane.color);
	if (!mat)
		return (false);
	n = unit_vector(&obj->data.plane.normal);
	u_dir = compute_tangent(&n);
	v_dir = cross(&n, &u_dir);
	u_vec = vec3_mul_scalar(&u_dir, RT_PLANE_SIZE);
	v_vec = vec3_mul_scalar(&v_dir, RT_PLANE_SIZE);
	corner = vec3_sub(&obj->data.plane.point, &u_vec);
	corner = vec3_sub(&corner, &v_vec);
	u_vec = vec3_mul_scalar(&u_dir, RT_PLANE_SIZE * 2.0);
	v_vec = vec3_mul_scalar(&v_dir, RT_PLANE_SIZE * 2.0);
	q = quad_create(&corner, &u_vec, &v_vec, mat);
	qp = (t_quad *)malloc(sizeof(t_quad));
	if (!qp)
		return (false);
	*qp = q;
	return (hittable_list_add_nonowned(world, qp,
			set_current_quad, quad_hit_noobj, &q.bbox));
}

/* ------------------------------------------------------------------ */
/*  Build a cylinder from parsed data                                 */
/*  .rt gives CENTER of cylinder — engine wants BASE                  */
/*  base = center − (height / 2) · axis_unit                          */
/*  radius = diameter / 2                                              */
/* ------------------------------------------------------------------ */

static bool	build_cylinder(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_vec3		axis_unit;
	t_vec3		offset;
	t_point3	base;
	t_cylinder	c;

	mat = create_material(&obj->mat, obj->data.cylinder.color);
	if (!mat)
		return (false);
	axis_unit = unit_vector(&obj->data.cylinder.axis);
	offset = vec3_mul_scalar(&axis_unit, obj->data.cylinder.height / 2.0);
	base = vec3_sub(&obj->data.cylinder.center, &offset);
	c = cylinder_create(&base, &axis_unit,
			obj->data.cylinder.diameter / 2.0,
			obj->data.cylinder.height, mat);
	return (hittable_list_add_cylinder(world, &c));
}

/* ------------------------------------------------------------------ */
/*  Build a cone from parsed data (BONUS)                             */
/*  cone_create(apex, axis, angle_deg, height, mat)                   */
/*  Translate diameter → half-angle:                                   */
/*  angle_deg = atan(radius / height) * 180 / π                       */
/* ------------------------------------------------------------------ */

static bool	build_cone(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_vec3		axis_unit;
	double		radius;
	double		angle_deg;
	t_cone		c;

	mat = create_material(&obj->mat, obj->data.cone.color);
	if (!mat)
		return (false);
	axis_unit = unit_vector(&obj->data.cone.axis);
	radius = obj->data.cone.diameter / 2.0;
	angle_deg = atan(radius / obj->data.cone.height) * (180.0 / PI);
	c = cone_create(&obj->data.cone.apex, &axis_unit,
			angle_deg, obj->data.cone.height, mat);
	return (hittable_list_add_cone(world, &c));
}

static bool	build_object(t_hittable_list *world, const t_rt_object *obj);

/* ------------------------------------------------------------------ */
/*  Build a triangle from parsed data                                 */
/* ------------------------------------------------------------------ */

static bool	build_triangle_obj(t_hittable_list *world, const t_rt_object *obj)
{
	t_material	*mat;
	t_triangle	t;

	mat = create_material(&obj->mat, obj->data.triangle.color);
	if (!mat)
		return (false);
	t = triangle_create(&obj->data.triangle.v0,
			&obj->data.triangle.v1,
			&obj->data.triangle.v2, mat);
	return (hittable_list_add_triangle(world, &t));
}

/* ------------------------------------------------------------------ */
/*  Build all parsed objects into the engine hittable_list             */
/* ------------------------------------------------------------------ */

static bool	build_object(t_hittable_list *world, const t_rt_object *obj)
{
	if (obj->type == OBJ_SPHERE)
		return (build_sphere(world, obj));
	if (obj->type == OBJ_PLANE)
		return (build_plane(world, obj));
	if (obj->type == OBJ_CYLINDER)
		return (build_cylinder(world, obj));
	if (obj->type == OBJ_CONE)
		return (build_cone(world, obj));
	if (obj->type == OBJ_QUAD)
		return (build_quad_obj(world, obj));
	if (obj->type == OBJ_MESH)
		return (build_mesh_obj(world, obj));
	if (obj->type == OBJ_TRIANGLE)
		return (build_triangle_obj(world, obj));
	return (true);
}

bool	build_scene_objects(t_scene *scene)
{
	int	i;

	i = 0;
	while (i < scene->object_count)
	{
		if (!build_object(&scene->world, &scene->objects[i]))
			return (false);
		i++;
	}
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Setup engine camera from parsed scene data                        */
/* ------------------------------------------------------------------ */

/* scene_build3.c */
void	setup_cam_quality(t_camera *cam, const t_scene *sc, int width);
void	setup_cam_orient(t_camera *cam, const t_scene *sc);

void	setup_camera(t_camera *cam, const t_scene *sc, int width)
{
	setup_cam_quality(cam, sc, width);
	setup_cam_orient(cam, sc);
	camera_init(cam, cam->aspect_ratio, cam->image_width);
}

/* ------------------------------------------------------------------ */
/*  Populate the direct-lighting cache from parsed lights              */
/* ------------------------------------------------------------------ */

static void	populate_light_cache(const t_scene *sc)
{
	t_light_info	infos[RT_MAX_LIGHTS];
	int				i;

	i = 0;
	while (i < sc->light_count && i < RT_MAX_LIGHTS)
	{
		infos[i].pos = sc->lights[i].pos;
		infos[i].emission = vec3_mul_scalar(&sc->lights[i].color,
				sc->lights[i].brightness * RT_LIGHT_SCALE);
		infos[i].radius = RT_LIGHT_RADIUS;
		i++;
	}
	set_scene_lights(infos, sc->light_count);
}

/* ------------------------------------------------------------------ */
/*  Add point lights as small emissive spheres to the world           */
/* ------------------------------------------------------------------ */

bool	add_scene_lights(t_hittable_list *world, const t_scene *sc)
{
	t_material	*mat;
	t_sphere	s;
	t_color		emit;
	int			i;

	i = 0;
	while (i < sc->light_count)
	{
		emit = vec3_mul_scalar(&sc->lights[i].color,
				sc->lights[i].brightness * RT_LIGHT_SCALE);
		mat = diffuse_light_create(emit);
		if (!mat)
			return (false);
		s = create_sphere(&sc->lights[i].pos, RT_LIGHT_RADIUS,
				sc->lights[i].color, mat);
		if (!hittable_list_add_sphere(world, &s))
			return (false);
		i++;
	}
	populate_light_cache(sc);
	return (true);
}
