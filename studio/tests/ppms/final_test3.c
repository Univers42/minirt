/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_test3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 16:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 16:18:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** "The Radiant Gallery" — improved showcase
**
** Pitch-black environment.  All illumination comes from emissive objects
** with high intensity and varied colors so light bleeds onto neighbours.
** Mountain-relief checker ground via displaced large spheres.
** All objects carefully spaced — no collisions.
*/

#include "common.h"
#include "constant_medium.h"
#include "cylinder.h"
#include "bvh.h"

/* ------------------------------------------------------------------ */
/*  Helper: heap-alloc a quad and add it                              */
/* ------------------------------------------------------------------ */
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
/*  Helper: add a rotated+translated box (heap-allocated list)       */
/* ------------------------------------------------------------------ */
static void	add_box_rt(t_hittable_list *w, t_point3 a, t_point3 b,
		t_material *mat, real_t deg, t_vec3 off)
{
	t_hittable_list		*bx;
	t_hittable_wrapper	wrap;
	t_rotate_y_wrap		*rot;
	t_translate_wrap	*tr;
	t_hittable_wrapper	rw;
	t_hittable_wrapper	tw;

	bx = (t_hittable_list *)malloc(sizeof(t_hittable_list));
	if (!bx)
		return ;
	hittable_list_init(bx);
	box_create_list(&a, &b, mat, bx);
	wrap = hittable_list_wrapper(bx);
	rot = rotate_y_create(&wrap, deg);
	if (!rot)
		return ;
	rw = (t_hittable_wrapper){.object = rot, .owned = true,
		.set_current = set_current_rotate,
		.hit_noobj = rotate_y_hit_noobj, .bbox = rot->bbox};
	tr = translate_create(&rw, &off);
	if (!tr)
		return ;
	tw = (t_hittable_wrapper){.object = tr, .owned = true,
		.set_current = set_current_translate,
		.hit_noobj = translate_hit_noobj, .bbox = tr->bbox};
	hittable_list_add_wrapper(w, &tw);
}

/* ------------------------------------------------------------------ */
/*  Mountain-relief ground: large spheres displaced by sin/cos       */
/* ------------------------------------------------------------------ */
static real_t	terrain_height(real_t x, real_t z)
{
	real_t	h;

	h = 0.35 * sin(x * 0.4) * cos(z * 0.3);
	h += 0.15 * sin(x * 1.1 + 2.0) * cos(z * 0.9 - 1.0);
	h += 0.08 * sin(x * 2.5) * sin(z * 2.2);
	return (h);
}

static void	build_terrain(t_hittable_list *w)
{
	int			ix;
	int			iz;
	real_t		x;
	real_t		z;
	t_material	*mat;
	t_sphere	s;
	t_point3	c;

	ix = -12;
	while (ix < 12)
	{
		iz = -12;
		while (iz < 12)
		{
			x = (real_t)ix * 1.0;
			z = (real_t)iz * 1.0;
			if ((ix + iz) % 2 == 0)
				mat = lambertian_create(vec3_create(0.08, 0.08, 0.08));
			else
				mat = lambertian_create(vec3_create(0.03, 0.03, 0.03));
			if ((ix * 7 + iz * 13) % 11 == 0)
				mat = diffuse_light_create_scaled(
						vec3_create(1.0, 0.6, 0.15), 4.0);
			else if ((ix * 3 + iz * 17) % 13 == 0)
				mat = diffuse_light_create_scaled(
						vec3_create(0.15, 0.4, 1.0), 3.5);
			c = point3_create(x, terrain_height(x, z) - 50.0, z);
			s = create_sphere(&c, 50.0,
					vec3_create(1.0, 1.0, 1.0), mat);
			hittable_list_add_sphere(w, &s);
			iz++;
		}
		ix++;
	}
}

/* ------------------------------------------------------------------ */
/*  Powerful colored area lights — varied hue & intensity            */
/* ------------------------------------------------------------------ */
static void	build_lights(t_hittable_list *w)
{
	t_material	*l;
	t_sphere	s;
	t_point3	c;

	l = diffuse_light_create_scaled(vec3_create(1.0, 0.85, 0.6), 25.0);
	add_quad(w, vec3_create(-4.0, 15.0, -4.0),
		vec3_create(8.0, 0.0, 0.0), vec3_create(0.0, 0.0, 8.0), l);
	l = diffuse_light_create_scaled(vec3_create(0.2, 0.8, 1.0), 18.0);
	add_quad(w, vec3_create(-14.0, 1.0, -6.0),
		vec3_create(0.0, 8.0, 0.0), vec3_create(0.0, 0.0, 12.0), l);
	l = diffuse_light_create_scaled(vec3_create(1.0, 0.15, 0.6), 18.0);
	add_quad(w, vec3_create(14.0, 1.0, -6.0),
		vec3_create(0.0, 8.0, 0.0), vec3_create(0.0, 0.0, 12.0), l);
	c = point3_create(0.0, 9.0, 0.0);
	l = diffuse_light_create_scaled(vec3_create(1.0, 0.65, 0.2), 30.0);
	s = create_sphere(&c, 1.5, vec3_create(1.0, 1.0, 1.0), l);
	hittable_list_add_sphere(w, &s);
	c = point3_create(-8.0, 0.6, -8.0);
	l = diffuse_light_create_scaled(vec3_create(0.1, 1.0, 0.3), 20.0);
	s = create_sphere(&c, 0.6, vec3_create(1.0, 1.0, 1.0), l);
	hittable_list_add_sphere(w, &s);
	c = point3_create(8.0, 0.6, -8.0);
	l = diffuse_light_create_scaled(vec3_create(1.0, 0.15, 0.1), 20.0);
	s = create_sphere(&c, 0.6, vec3_create(1.0, 1.0, 1.0), l);
	hittable_list_add_sphere(w, &s);
	c = point3_create(0.0, 0.5, 10.0);
	l = diffuse_light_create_scaled(vec3_create(0.2, 0.3, 1.0), 22.0);
	s = create_sphere(&c, 0.5, vec3_create(1.0, 1.0, 1.0), l);
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Hero spheres — well separated (no collisions)                    */
/* ------------------------------------------------------------------ */
static void	build_heroes(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;
	t_material	*mat;

	c = point3_create(0.0, 2.2, 0.0);
	mat = dielectric_create(1.5);
	s = create_sphere(&c, 2.2, vec3_create(1.0, 1.0, 1.0), mat);
	hittable_list_add_sphere(w, &s);
	c = point3_create(0.0, 2.2, 0.0);
	mat = dielectric_create(1.5);
	s = create_sphere(&c, -2.0, vec3_create(1.0, 1.0, 1.0), mat);
	hittable_list_add_sphere(w, &s);
	c = point3_create(-5.5, 1.6, 1.5);
	mat = metal_create_fuzz(vec3_create(0.95, 0.8, 0.4), 0.0);
	s = create_sphere(&c, 1.6, vec3_create(0.95, 0.8, 0.4), mat);
	hittable_list_add_sphere(w, &s);
	c = point3_create(5.5, 1.4, 1.5);
	mat = glossy_create(vec3_create(0.06, 0.06, 0.65), 0.02, 0.95);
	s = create_sphere(&c, 1.4, vec3_create(0.06, 0.06, 0.65), mat);
	hittable_list_add_sphere(w, &s);
	c = point3_create(-3.0, 1.0, -4.5);
	mat = tinted_glass_create(1.5, vec3_create(0.95, 0.1, 0.1));
	s = create_sphere(&c, 1.0, vec3_create(0.95, 0.1, 0.1), mat);
	hittable_list_add_sphere(w, &s);
	c = point3_create(3.0, 1.0, -4.5);
	mat = lambertian_create_texture(noise_texture_create_turb(6.0, 7));
	s = create_sphere(&c, 1.0, vec3_create(1.0, 1.0, 1.0), mat);
	hittable_list_add_sphere(w, &s);
}

/* ------------------------------------------------------------------ */
/*  Metal gradient + Glossy gradient rows                            */
/* ------------------------------------------------------------------ */
static void	build_metal_line(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;
	int			i;

	i = 0;
	while (i < 5)
	{
		c = point3_create(-6.0 + (real_t)i * 3.0, 0.7, -7.5);
		s = create_sphere(&c, 0.7, vec3_create(1, 1, 1),
				metal_create_fuzz(vec3_create(
						0.95 - (real_t)i * 0.05,
						0.85 - (real_t)i * 0.1,
						0.55 + (real_t)i * 0.05),
					(real_t)i * 0.12));
		hittable_list_add_sphere(w, &s);
		i++;
	}
}

static void	build_glossy_line(t_hittable_list *w)
{
	t_point3	c;
	t_sphere	s;
	int			i;

	i = 0;
	while (i < 5)
	{
		c = point3_create(-6.0 + (real_t)i * 3.0, 0.7, 6.5);
		s = create_sphere(&c, 0.7, vec3_create(1, 1, 1),
				glossy_create(vec3_create(
						0.1 + (real_t)i * 0.2,
						0.05 + (real_t)i * 0.05,
						0.75 - (real_t)i * 0.12),
					0.02 + (real_t)i * 0.07,
					0.95 - (real_t)i * 0.15));
		hittable_list_add_sphere(w, &s);
		i++;
	}
}

/* ------------------------------------------------------------------ */
/*  Structures: boxes, cylinders, cones — well separated             */
/* ------------------------------------------------------------------ */
static void	build_structures(t_hittable_list *w)
{
	t_cylinder	cyl;
	t_cone		cn;
	t_point3	base;

	add_box_rt(w, point3_create(0, 0, 0), point3_create(1.5, 3.5, 1.5),
		metal_create_fuzz(vec3_create(0.95, 0.93, 0.88), 0.0),
		-25.0, vec3_create(-10.0, 0.0, -2.0));
	add_box_rt(w, point3_create(0, 0, 0), point3_create(1.5, 2.5, 1.5),
		glossy_create(vec3_create(0.7, 0.3, 0.1), 0.06, 0.9),
		30.0, vec3_create(9.0, 0.0, 3.0));
	base = point3_create(-10.0, 0.0, 5.0);
	cyl = cylinder_create_y(&base, 0.5, 4.0,
			metal_create_fuzz(vec3_create(0.92, 0.78, 0.35), 0.01));
	hittable_list_add_cylinder(w, &cyl);
	base = point3_create(-10.0, 4.5, 5.0);
	cn = cone_create(&base, &(t_vec3){0, -1, 0}, 30.0, 1.5,
			glossy_create(vec3_create(0.85, 0.1, 0.1), 0.04, 0.9));
	hittable_list_add_cone(w, &cn);
	base = point3_create(10.0, 0.0, -5.0);
	cyl = cylinder_create_y(&base, 0.6, 3.5, dielectric_create(1.5));
	hittable_list_add_cylinder(w, &cyl);
}

/* ------------------------------------------------------------------ */
/*  Fog volume around glass hero                                     */
/* ------------------------------------------------------------------ */
static void	build_fog(t_hittable_list *w)
{
	t_sphere			bs;
	t_sphere			*sp;
	t_hittable_wrapper	bw;
	t_constant_medium	*fog;
	t_point3			c;

	c = point3_create(0.0, 2.5, 0.0);
	bs = create_sphere(&c, 3.5, vec3_create(1, 1, 1),
			lambertian_create(vec3_create(1, 1, 1)));
	sp = (t_sphere *)malloc(sizeof(t_sphere));
	if (!sp)
		return ;
	*sp = bs;
	bw = (t_hittable_wrapper){.object = sp, .owned = false,
		.set_current = set_current_sphere,
		.hit_noobj = sphere_hit_noobj, .bbox = bs.bbox};
	fog = constant_medium_create_color(&bw, 0.03,
			vec3_create(0.9, 0.9, 1.0));
	if (fog)
		hittable_list_add_nonowned(w, fog,
			set_current_medium, constant_medium_hit_noobj, &fog->bbox);
}

/* ------------------------------------------------------------------ */
/*  Scattered small spheres — collision-safe placement               */
/* ------------------------------------------------------------------ */
static bool	too_close(t_point3 c, t_point3 *placed, int n, real_t mn)
{
	int		k;
	t_vec3	d;

	k = 0;
	while (k < n)
	{
		d = vec3_sub(&c, &placed[k]);
		if (vec3_length(&d) < mn)
			return (true);
		k++;
	}
	return (false);
}

static t_material	*pick_scatter_mat(void)
{
	real_t	ch;

	ch = random_double();
	if (ch < 0.25)
		return (lambertian_create(vec3_mul_elem(
					&(t_vec3){random_double(), random_double(),
					random_double()},
					&(t_vec3){random_double(), random_double(),
					random_double()})));
	if (ch < 0.42)
		return (metal_create_fuzz(vec3_random_interval(0.5, 1.0),
				random_real_interval(0.0, 0.25)));
	if (ch < 0.54)
		return (dielectric_create(random_real_interval(1.3, 2.4)));
	if (ch < 0.68)
		return (glossy_create(vec3_random_interval(0.2, 1.0),
				random_real_interval(0.02, 0.3),
				random_real_interval(0.3, 1.0)));
	if (ch < 0.78)
		return (tinted_glass_create(1.5,
				vec3_random_interval(0.1, 1.0)));
	if (ch < 0.88)
		return (diffuse_light_create_scaled(
				vec3_random_interval(0.4, 1.0),
				random_real_interval(5.0, 15.0)));
	return (lambertian_create_texture(
			noise_texture_create(random_real_interval(3.0, 10.0))));
}

static void	build_scatter(t_hittable_list *w)
{
	t_point3	placed[200];
	int			count;
	int			tries;
	t_point3	c;
	t_sphere	s;

	count = 0;
	tries = 0;
	while (count < 120 && tries < 2000)
	{
		c = point3_create(random_real_interval(-11.0, 11.0), 0.2,
				random_real_interval(-11.0, 11.0));
		if (!too_close(c, placed, count, 0.6))
		{
			s = create_sphere(&c, 0.2,
					vec3_create(1, 1, 1), pick_scatter_mat());
			hittable_list_add_sphere(w, &s);
			placed[count] = c;
			count++;
		}
		tries++;
	}
}

/* ================================================================== */
/*  MAIN                                                              */
/* ================================================================== */
int	main(void)
{
	t_hittable_list	world;
	t_camera		cam;

	hittable_list_init(&world);
	build_terrain(&world);
	build_lights(&world);
	build_heroes(&world);
	build_metal_line(&world);
	build_glossy_line(&world);
	build_structures(&world);
	build_fog(&world);
	build_scatter(&world);
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1200;
	cam.samples_per_pixel = 200;
	cam.max_depth = 50;
	cam.vfov = 32.0;
	cam.lookfrom = point3_create(20.0, 7.0, 16.0);
	cam.lookat = point3_create(0.0, 1.5, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);
	cam.defocus_angle = 0.0;
	cam.focus_dist = 10.0;
	cam.background = vec3_create(0.0, 0.0, 0.0);
	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	camera_render(&cam, stdout, &world);
	hittable_list_clear(&world);
	return (0);
}
