/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_test7.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 16:18:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** "The Cathedral" — A Cornell-box–inspired room with towering columns,
** stained-glass spheres, and a single powerful warm light from above.
** Dramatic shadows and color bleeding from the tinted glass objects
** paint vivid caustics on the matte walls.
*/

#include "common.h"
#include "cylinder.h"
#include "bvh.h"

static bool	add_quad(t_hittable_list *w, t_point3 q,
		t_vec3 u, t_vec3 v, t_material *m)
{
	t_quad	*qp;

	qp = (t_quad *)malloc(sizeof(t_quad));
	if (!qp)
		return (false);
	*qp = quad_create(&q, &u, &v, m);
	return (hittable_list_add_nonowned(w, qp,
			set_current_quad, quad_hit_noobj, &qp->bbox));
}

/* ------------------------------------------------------------------ */
/*  Room walls — large Cornell box                                   */
/* ------------------------------------------------------------------ */
static void	build_room(t_hittable_list *w)
{
	t_material	*white;
	t_material	*red;
	t_material	*blue;

	white = lambertian_create(vec3_create(0.73, 0.73, 0.73));
	red = lambertian_create(vec3_create(0.65, 0.05, 0.05));
	blue = lambertian_create(vec3_create(0.12, 0.15, 0.6));
	/* Floor */
	add_quad(w, vec3_create(0, 0, 0), vec3_create(12, 0, 0),
		vec3_create(0, 0, 12), white);
	/* Ceiling */
	add_quad(w, vec3_create(0, 10, 0), vec3_create(12, 0, 0),
		vec3_create(0, 0, 12), white);
	/* Back wall */
	add_quad(w, vec3_create(0, 0, 0), vec3_create(12, 0, 0),
		vec3_create(0, 10, 0), white);
	/* Left wall — red */
	add_quad(w, vec3_create(0, 0, 0), vec3_create(0, 0, 12),
		vec3_create(0, 10, 0), red);
	/* Right wall — blue */
	add_quad(w, vec3_create(12, 0, 0), vec3_create(0, 0, 12),
		vec3_create(0, 10, 0), blue);
}

/* ------------------------------------------------------------------ */
/*  Ceiling light — warm, powerful, rectangular                      */
/* ------------------------------------------------------------------ */
static void	build_ceiling_light(t_hittable_list *w)
{
	t_material	*l;

	l = diffuse_light_create_scaled(vec3_create(1.0, 0.9, 0.7), 30.0);
	add_quad(w, vec3_create(3.0, 9.99, 3.0),
		vec3_create(6.0, 0.0, 0.0), vec3_create(0.0, 0.0, 6.0), l);
}

/* ------------------------------------------------------------------ */
/*  Marble columns — cylinders + cones                               */
/* ------------------------------------------------------------------ */
static void	build_columns(t_hittable_list *w)
{
	t_cylinder	cyl;
	t_cone		cn;
	t_point3	base;
	t_material	*marble;

	marble = lambertian_create_texture(noise_texture_create_turb(4.0, 7));
	/* Left column */
	base = point3_create(2.5, 0.0, 4.0);
	cyl = cylinder_create_y(&base, 0.4, 7.0, marble);
	hittable_list_add_cylinder(w, &cyl);
	base = point3_create(2.5, 7.0, 4.0);
	cn = cone_create(&base, &(t_vec3){0, -1, 0}, 22.0, 0.8,
			lambertian_create(vec3_create(0.85, 0.82, 0.75)));
	hittable_list_add_cone(w, &cn);
	/* Right column */
	marble = lambertian_create_texture(noise_texture_create_turb(4.0, 7));
	base = point3_create(9.5, 0.0, 4.0);
	cyl = cylinder_create_y(&base, 0.4, 7.0, marble);
	hittable_list_add_cylinder(w, &cyl);
	base = point3_create(9.5, 7.0, 4.0);
	cn = cone_create(&base, &(t_vec3){0, -1, 0}, 22.0, 0.8,
			lambertian_create(vec3_create(0.85, 0.82, 0.75)));
	hittable_list_add_cone(w, &cn);
	/* Center back column */
	marble = lambertian_create_texture(noise_texture_create_turb(4.0, 7));
	base = point3_create(6.0, 0.0, 1.5);
	cyl = cylinder_create_y(&base, 0.5, 8.0, marble);
	hittable_list_add_cylinder(w, &cyl);
	base = point3_create(6.0, 8.0, 1.5);
	cn = cone_create(&base, &(t_vec3){0, -1, 0}, 25.0, 1.0,
			lambertian_create(vec3_create(0.85, 0.82, 0.75)));
	hittable_list_add_cone(w, &cn);
}

/* ------------------------------------------------------------------ */
/*  Stained glass spheres — tinted + emissive core combos            */
/* ------------------------------------------------------------------ */
static void	build_stained_glass(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;

	/* Red stained glass on pedestal (left) */
	c = point3_create(3.5, 1.5, 8.0);
	s = create_sphere(&c, 1.0, vec3_create(1, 1, 1),
			tinted_glass_create(1.5, vec3_create(0.95, 0.1, 0.1)));
	hittable_list_add_sphere(w, &s);
	/* Warm emissive core inside */
	c = point3_create(3.5, 1.5, 8.0);
	s = create_sphere(&c, 0.3, vec3_create(1, 1, 1),
			diffuse_light_create_scaled(
				vec3_create(1.0, 0.4, 0.1), 12.0));
	hittable_list_add_sphere(w, &s);
	/* Blue stained glass (right) */
	c = point3_create(8.5, 1.5, 8.0);
	s = create_sphere(&c, 1.0, vec3_create(1, 1, 1),
			tinted_glass_create(1.5, vec3_create(0.1, 0.2, 0.95)));
	hittable_list_add_sphere(w, &s);
	c = point3_create(8.5, 1.5, 8.0);
	s = create_sphere(&c, 0.3, vec3_create(1, 1, 1),
			diffuse_light_create_scaled(
				vec3_create(0.2, 0.4, 1.0), 12.0));
	hittable_list_add_sphere(w, &s);
	/* Green glass (center front) */
	c = point3_create(6.0, 1.0, 10.0);
	s = create_sphere(&c, 0.8, vec3_create(1, 1, 1),
			tinted_glass_create(1.5, vec3_create(0.1, 0.9, 0.2)));
	hittable_list_add_sphere(w, &s);
	c = point3_create(6.0, 1.0, 10.0);
	s = create_sphere(&c, 0.25, vec3_create(1, 1, 1),
			diffuse_light_create_scaled(
				vec3_create(0.2, 1.0, 0.3), 10.0));
	hittable_list_add_sphere(w, &s);
	/* Gold mirror altar piece */
	c = point3_create(6.0, 3.5, 1.0);
	s = create_sphere(&c, 1.2, vec3_create(1, 1, 1),
			metal_create_fuzz(vec3_create(0.95, 0.82, 0.45), 0.0));
	hittable_list_add_sphere(w, &s);
}

int	main(void)
{
	t_hittable_list	world;
	t_camera		cam;

	hittable_list_init(&world);
	build_room(&world);
	build_ceiling_light(&world);
	build_columns(&world);
	build_stained_glass(&world);
	cam.aspect_ratio = 1.0;
	cam.image_width = 800;
	cam.samples_per_pixel = 300;
	cam.max_depth = 50;
	cam.vfov = 55.0;
	cam.lookfrom = point3_create(6.0, 4.0, 11.8);
	cam.lookat = point3_create(6.0, 3.0, 3.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	cam.focus_dist = 10.0;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);
	hittable_list_clear(&world);
	return (0);
}
