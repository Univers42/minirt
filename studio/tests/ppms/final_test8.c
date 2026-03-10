/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_test8.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 16:18:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** "Crystalline Cavern" — A dark cave with glowing crystals (cones),
** stalagmites (cylinders), a reflective underground lake (mirror floor),
** and bioluminescent fog.  Crystals emit vivid colored light illuminating
** the cavern walls.
*/

#include "common.h"
#include "constant_medium.h"
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
/*  Underground lake — dark reflective floor                         */
/* ------------------------------------------------------------------ */
static void	build_lake(t_hittable_list *w)
{
	add_quad(w, vec3_create(-25.0, 0.0, -25.0),
		vec3_create(50.0, 0.0, 0.0), vec3_create(0.0, 0.0, 50.0),
		metal_create_fuzz(vec3_create(0.06, 0.08, 0.12), 0.01));
}

/* ------------------------------------------------------------------ */
/*  Cave ceiling — dark matte dome (huge inverted sphere)            */
/* ------------------------------------------------------------------ */
static void	build_ceiling(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;

	c = point3_create(0.0, 0.0, 0.0);
	s = create_sphere(&c, 30.0, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(0.04, 0.03, 0.03)));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Glowing crystal cluster (emissive cones)                         */
/* ------------------------------------------------------------------ */
static void	add_crystal(t_hittable_list *w, t_point3 base,
		t_vec3 axis, real_t angle, real_t h, t_color col, real_t emit)
{
	t_cone		cn;
	t_cylinder	stem;

	cn = cone_create(&base, &axis, angle, h,
			diffuse_light_create_scaled(col, emit));
	hittable_list_add_cone(w, &cn);
	stem = cylinder_create_y(&base, h * 0.08, h * 0.4,
			tinted_glass_create(1.8, col));
	hittable_list_add_cylinder(w, &stem);
}

static void	build_crystals(t_hittable_list *w)
{
	/* Central amethyst cluster */
	add_crystal(w, point3_create(0, 5.0, 0),
		(t_vec3){0, -1, 0}, 12.0, 4.5,
		vec3_create(0.6, 0.15, 0.9), 25.0);
	add_crystal(w, point3_create(1.2, 4.0, 0.8),
		(t_vec3){0.1, -1, 0.05}, 10.0, 3.5,
		vec3_create(0.7, 0.2, 1.0), 20.0);
	add_crystal(w, point3_create(-0.8, 3.5, 1.0),
		(t_vec3){-0.05, -1, 0.08}, 11.0, 3.0,
		vec3_create(0.5, 0.1, 0.8), 18.0);
	/* Emerald cluster — left */
	add_crystal(w, point3_create(-6.0, 3.0, -3.0),
		(t_vec3){0, -1, 0}, 14.0, 2.8,
		vec3_create(0.1, 0.9, 0.3), 22.0);
	add_crystal(w, point3_create(-5.0, 2.5, -2.0),
		(t_vec3){0.08, -1, 0.05}, 12.0, 2.2,
		vec3_create(0.15, 1.0, 0.4), 18.0);
	/* Ruby cluster — right */
	add_crystal(w, point3_create(5.5, 3.5, -2.0),
		(t_vec3){0, -1, 0}, 13.0, 3.2,
		vec3_create(1.0, 0.1, 0.15), 24.0);
	add_crystal(w, point3_create(6.5, 2.8, -1.0),
		(t_vec3){-0.05, -1, 0.1}, 11.0, 2.5,
		vec3_create(0.9, 0.15, 0.1), 19.0);
	/* Sapphire — behind */
	add_crystal(w, point3_create(2.0, 4.0, -7.0),
		(t_vec3){0, -1, 0}, 15.0, 3.8,
		vec3_create(0.1, 0.3, 1.0), 26.0);
	/* Citrine — far right */
	add_crystal(w, point3_create(9.0, 2.5, 3.0),
		(t_vec3){0, -1, 0}, 13.0, 2.5,
		vec3_create(1.0, 0.8, 0.1), 20.0);
}

/* ------------------------------------------------------------------ */
/*  Stalagmites — matte rock cylinders rising from the floor         */
/* ------------------------------------------------------------------ */
static void	build_stalagmites(t_hittable_list *w)
{
	t_cylinder	cyl;
	t_cone		tip;
	t_point3	base;

	base = point3_create(-3.0, 0.0, 5.0);
	cyl = cylinder_create_y(&base, 0.4, 2.5,
			lambertian_create(vec3_create(0.25, 0.2, 0.15)));
	hittable_list_add_cylinder(w, &cyl);
	tip = cone_create(&(t_point3){-3.0, 2.5, 5.0},
		&(t_vec3){0, 1, 0}, 20.0, 1.0,
		lambertian_create(vec3_create(0.2, 0.18, 0.12)));
	hittable_list_add_cone(w, &tip);
	base = point3_create(4.0, 0.0, 6.0);
	cyl = cylinder_create_y(&base, 0.35, 1.8,
			lambertian_create(vec3_create(0.2, 0.18, 0.14)));
	hittable_list_add_cylinder(w, &cyl);
	tip = cone_create(&(t_point3){4.0, 1.8, 6.0},
		&(t_vec3){0, 1, 0}, 18.0, 0.8,
		lambertian_create(vec3_create(0.18, 0.15, 0.1)));
	hittable_list_add_cone(w, &tip);
	base = point3_create(-7.0, 0.0, -5.0);
	cyl = cylinder_create_y(&base, 0.5, 3.0,
			lambertian_create(vec3_create(0.22, 0.18, 0.12)));
	hittable_list_add_cylinder(w, &cyl);
}

/* ------------------------------------------------------------------ */
/*  Reflective mineral deposits on the floor                         */
/* ------------------------------------------------------------------ */
static void	build_minerals(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;

	/* Glossy obsidian */
	c = point3_create(-4.0, 0.5, 2.0);
	s = create_sphere(&c, 0.5, vec3_create(1, 1, 1),
			glossy_create(vec3_create(0.05, 0.05, 0.08), 0.01, 0.98));
	hittable_list_add_sphere(w, &s);
	/* Quartz */
	c = point3_create(3.0, 0.6, 3.0);
	s = create_sphere(&c, 0.6, vec3_create(1, 1, 1),
			dielectric_create(1.55));
	hittable_list_add_sphere(w, &s);
	c = point3_create(3.0, 0.6, 3.0);
	s = create_sphere(&c, -0.5, vec3_create(1, 1, 1),
			dielectric_create(1.55));
	hittable_list_add_sphere(w, &s);
	/* Gold nugget */
	c = point3_create(7.0, 0.4, 0.0);
	s = create_sphere(&c, 0.4, vec3_create(1, 1, 1),
			metal_create_fuzz(vec3_create(0.95, 0.8, 0.35), 0.15));
	hittable_list_add_sphere(w, &s);
	/* Moonstone */
	c = point3_create(-8.0, 0.5, 1.0);
	s = create_sphere(&c, 0.5, vec3_create(1, 1, 1),
			glossy_create(vec3_create(0.7, 0.7, 0.85), 0.08, 0.4));
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Bioluminescent cave fog                                          */
/* ------------------------------------------------------------------ */
static void	build_cave_fog(t_hittable_list *w)
{
	t_sphere			bs;
	t_sphere			*sp;
	t_hittable_wrapper	bw;
	t_constant_medium	*fog;
	t_point3			c;

	c = point3_create(0.0, 4.0, 0.0);
	bs = create_sphere(&c, 12.0, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(1, 1, 1)));
	sp = (t_sphere *)malloc(sizeof(t_sphere));
	if (!sp)
		return ;
	*sp = bs;
	bw = (t_hittable_wrapper){.object = sp, .owned = false,
		.set_current = set_current_sphere,
		.hit_noobj = sphere_hit_noobj, .bbox = bs.bbox};
	fog = constant_medium_create_color(&bw, 0.008,
			vec3_create(0.5, 0.6, 0.9));
	if (fog)
		hittable_list_add_nonowned(w, fog,
			set_current_medium, constant_medium_hit_noobj, &fog->bbox);
}

int	main(void)
{
	t_hittable_list	world;
	t_camera		cam;

	hittable_list_init(&world);
	build_lake(&world);
	build_ceiling(&world);
	build_crystals(&world);
	build_stalagmites(&world);
	build_minerals(&world);
	build_cave_fog(&world);
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1200;
	cam.samples_per_pixel = 250;
	cam.max_depth = 50;
	cam.vfov = 60.0;
	cam.lookfrom = point3_create(0.0, 2.5, 14.0);
	cam.lookat = point3_create(0.0, 2.5, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	cam.focus_dist = 10.0;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);
	hittable_list_clear(&world);
	return (0);
}
