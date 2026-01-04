/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/04 00:38:35 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/04 01:19:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../common.h"
#include "../quad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Forward declarations of scene functions */
extern void perlin_spheres(void);
void cornell_box(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Materials */
	t_material *red = lambertian_create(vec3_create(0.65, 0.05, 0.05));
	t_material *white = lambertian_create(vec3_create(0.73, 0.73, 0.73));
	t_material *green = lambertian_create(vec3_create(0.12, 0.45, 0.15));
	t_material *light = diffuse_light_create(vec3_create(15.0, 15.0, 15.0));

	if (!red || !white || !green || !light)
	{
		fprintf(stderr, "Failed to create materials\n");
		return;
	}

	/* Right wall (green) */
	t_point3 pos1 = point3_create(555.0, 0.0, 0.0);
	t_vec3 u1 = vec3_create(0.0, 555.0, 0.0);
	t_vec3 v1 = vec3_create(0.0, 0.0, 555.0);
	t_quad quad1 = quad_create(&pos1, &u1, &v1, green);
	t_quad *quad1_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad1_copy)
	{
		*quad1_copy = quad1;
		hittable_list_add_nonowned(&world, quad1_copy, set_current_quad,
								   quad_hit_noobj, &quad1.bbox);
	}

	/* Left wall (red) */
	t_point3 pos2 = point3_create(0.0, 0.0, 0.0);
	t_vec3 u2 = vec3_create(0.0, 555.0, 0.0);
	t_vec3 v2 = vec3_create(0.0, 0.0, 555.0);
	t_quad quad2 = quad_create(&pos2, &u2, &v2, red);
	t_quad *quad2_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad2_copy)
	{
		*quad2_copy = quad2;
		hittable_list_add_nonowned(&world, quad2_copy, set_current_quad,
								   quad_hit_noobj, &quad2.bbox);
	}

	/* Ceiling light */
	t_point3 pos3 = point3_create(343.0, 554.0, 332.0);
	t_vec3 u3 = vec3_create(-130.0, 0.0, 0.0);
	t_vec3 v3 = vec3_create(0.0, 0.0, -105.0);
	t_quad quad3 = quad_create(&pos3, &u3, &v3, light);
	t_quad *quad3_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad3_copy)
	{
		*quad3_copy = quad3;
		hittable_list_add_nonowned(&world, quad3_copy, set_current_quad,
								   quad_hit_noobj, &quad3.bbox);
	}

	/* Floor (white) */
	t_point3 pos4 = point3_create(0.0, 0.0, 0.0);
	t_vec3 u4 = vec3_create(555.0, 0.0, 0.0);
	t_vec3 v4 = vec3_create(0.0, 0.0, 555.0);
	t_quad quad4 = quad_create(&pos4, &u4, &v4, white);
	t_quad *quad4_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad4_copy)
	{
		*quad4_copy = quad4;
		hittable_list_add_nonowned(&world, quad4_copy, set_current_quad,
								   quad_hit_noobj, &quad4.bbox);
	}

	/* Ceiling (white) */
	t_point3 pos5 = point3_create(555.0, 555.0, 555.0);
	t_vec3 u5 = vec3_create(-555.0, 0.0, 0.0);
	t_vec3 v5 = vec3_create(0.0, 0.0, -555.0);
	t_quad quad5 = quad_create(&pos5, &u5, &v5, white);
	t_quad *quad5_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad5_copy)
	{
		*quad5_copy = quad5;
		hittable_list_add_nonowned(&world, quad5_copy, set_current_quad,
								   quad_hit_noobj, &quad5.bbox);
	}

	/* Back wall (white) */
	t_point3 pos6 = point3_create(0.0, 0.0, 555.0);
	t_vec3 u6 = vec3_create(555.0, 0.0, 0.0);
	t_vec3 v6 = vec3_create(0.0, 555.0, 0.0);
	t_quad quad6 = quad_create(&pos6, &u6, &v6, white);
	t_quad *quad6_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad6_copy)
	{
		*quad6_copy = quad6;
		hittable_list_add_nonowned(&world, quad6_copy, set_current_quad,
								   quad_hit_noobj, &quad6.bbox);
	}

	/* Add rotated/translated boxes */
	t_point3 box1_min = point3_create(0.0, 0.0, 0.0);
	t_point3 box1_max = point3_create(165.0, 330.0, 165.0);
	t_hittable_list box1_list;
	box_create_list(&box1_min, &box1_max, white, &box1_list);
	t_hittable_wrapper box1_wrap = hittable_list_wrapper(&box1_list);
	t_rotate_y_wrap *box1_rot = rotate_y_create(&box1_wrap, 15.0);
	t_hittable_wrapper box1_rot_wrap = {
		.object = box1_rot,
		.owned = true,
		.set_current = set_current_rotate,
		.hit_noobj = rotate_y_hit_noobj,
		.bbox = box1_rot ? box1_rot->bbox : aabb_empty()};
	t_vec3 box1_offset = vec3_create(265.0, 0.0, 295.0);
	t_translate_wrap *box1_trans = translate_create(&box1_rot_wrap, &box1_offset);
	if (box1_trans)
	{
		hittable_list_add_nonowned(&world, box1_trans, set_current_translate, translate_hit_noobj, &box1_trans->bbox);
	}

	t_point3 box2_min = point3_create(0.0, 0.0, 0.0);
	t_point3 box2_max = point3_create(165.0, 165.0, 165.0);
	t_hittable_list box2_list;
	box_create_list(&box2_min, &box2_max, white, &box2_list);
	t_hittable_wrapper box2_wrap = hittable_list_wrapper(&box2_list);
	t_rotate_y_wrap *box2_rot = rotate_y_create(&box2_wrap, -18.0);
	t_hittable_wrapper box2_rot_wrap = {
		.object = box2_rot,
		.owned = true,
		.set_current = set_current_rotate,
		.hit_noobj = rotate_y_hit_noobj,
		.bbox = box2_rot ? box2_rot->bbox : aabb_empty()};
	t_vec3 box2_offset = vec3_create(130.0, 0.0, 65.0);
	t_translate_wrap *box2_trans = translate_create(&box2_rot_wrap, &box2_offset);
	if (box2_trans)
	{
		hittable_list_add_nonowned(&world, box2_trans, set_current_translate, translate_hit_noobj, &box2_trans->bbox);
	}

	/* Setup camera */
	t_camera camera;
	camera.aspect_ratio = 1.0;
	camera.image_width = 600;
	camera.samples_per_pixel = 200.0;
	camera.max_depth = 50;
	camera.vfov = 40.0;
	camera.lookfrom = point3_create(278.0, 278.0, -800.0);
	camera.lookat = point3_create(278.0, 278.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.0;
	camera.background = vec3_create(0.0, 0.0, 0.0);

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render */
	camera_render(&camera, stdout, &world);

	/* Cleanup */
	red->destroy(red);
	free(red);
	white->destroy(white);
	free(white);
	green->destroy(green);
	free(green);
	light->destroy(light);
	free(light);
	hittable_list_clear(&world);
}

/* Scene: bouncing spheres with random materials */
static void bouncing_spheres(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Ground sphere */
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_material *ground_mat = lambertian_create(vec3_create(0.5, 0.5, 0.5));
	t_sphere ground = create_sphere(&ground_center, 1000.0, vec3_create(0.5, 0.5, 0.5), ground_mat);
	hittable_list_add_sphere(&world, &ground);

	/* Random small spheres */
	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			real_t choose_mat = random_real();
			t_point3 center = point3_create(
				(real_t)a + (real_t)0.9 * random_real(),
				(real_t)0.2,
				(real_t)b + (real_t)0.9 * random_real());

			/* Store temporary points in variables before taking address */
			t_point3 fixed_point = point3_create(4.0, 0.2, 0.0);
			t_vec3 diff = vec3_sub(&center, &fixed_point);
			if (vec3_length(&diff) > 0.9)
			{
				t_material *sphere_mat = NULL;
				if (choose_mat < 0.8)
				{
					/* Lambertian */
					t_vec3 rand1 = vec3_create(random_real(), random_real(), random_real());
					t_vec3 rand2 = vec3_create(random_real(), random_real(), random_real());
					t_color albedo = vec3_mul_elem(&rand1, &rand2);
					sphere_mat = lambertian_create(albedo);
				}
				else if (choose_mat < 0.95)
				{
					/* Metal */
					t_color albedo = vec3_create(
						random_real_interval(0.5, 1.0),
						random_real_interval(0.5, 1.0),
						random_real_interval(0.5, 1.0));
					real_t fuzz = random_real_interval(0.0, 0.5);
					sphere_mat = metal_create_fuzz(albedo, fuzz);
				}
				else
				{
					/* Glass */
					sphere_mat = dielectric_create(1.5);
				}

				t_sphere sphere = create_sphere(&center, 0.2, vec3_create(1.0, 1.0, 1.0), sphere_mat);
				hittable_list_add_sphere(&world, &sphere);
			}
		}
	}

	/* Three large spheres */
	t_material *mat1 = dielectric_create(1.5);
	t_point3 center1 = point3_create(0.0, 1.0, 0.0);
	t_sphere sphere1 = create_sphere(&center1, 1.0, vec3_create(1.0, 1.0, 1.0), mat1);
	hittable_list_add_sphere(&world, &sphere1);

	t_material *mat2 = lambertian_create(vec3_create(0.4, 0.2, 0.1));
	t_point3 center2 = point3_create(-4.0, 1.0, 0.0);
	t_sphere sphere2 = create_sphere(&center2, 1.0, vec3_create(1.0, 1.0, 1.0), mat2);
	hittable_list_add_sphere(&world, &sphere2);

	t_material *mat3 = metal_create_fuzz(vec3_create(0.7, 0.6, 0.5), 0.0);
	t_point3 center3 = point3_create(4.0, 1.0, 0.0);
	t_sphere sphere3 = create_sphere(&center3, 1.0, vec3_create(1.0, 1.0, 1.0), mat3);
	hittable_list_add_sphere(&world, &sphere3);

	/* Setup camera */
	t_camera camera;
	camera.aspect_ratio = 16.0 / 9.0;
	camera.image_width = 800;		  /* was 400 */
	camera.samples_per_pixel = 400.0; /* was 100 */
	camera.max_depth = 20;
	camera.vfov = 20.0;
	camera.lookfrom = point3_create(13.0, 2.0, 3.0);
	camera.lookat = point3_create(0.0, 0.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.6;
	camera.focus_dist = 10.0;
	camera.background = vec3_create(0.70, 0.80, 1.00);

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render */
	camera_render(&camera, stdout, &world);

	/* Cleanup */
	hittable_list_clear(&world);
}

/* Scene: checkered spheres */
static void checkered_spheres(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Create checker texture */
	t_texture *even = solid_color_create(vec3_create(0.2, 0.3, 0.1));
	t_texture *odd = solid_color_create(vec3_create(0.9, 0.9, 0.9));
	t_texture *checker = checker_texture_create(0.32, even, odd);

	t_material *ground_mat = lambertian_create_texture(checker);

	/* Ground sphere */
	t_point3 ground_center = point3_create(0.0, -10.0, 0.0);
	t_sphere ground = create_sphere(&ground_center, 10.0, vec3_create(1.0, 1.0, 1.0), ground_mat);
	hittable_list_add_sphere(&world, &ground);

	/* Upper sphere */
	t_point3 upper_center = point3_create(0.0, 10.0, 0.0);
	t_sphere upper = create_sphere(&upper_center, 10.0, vec3_create(1.0, 1.0, 1.0), ground_mat);
	hittable_list_add_sphere(&world, &upper);

	/* Setup camera */
	t_camera camera;
	camera.aspect_ratio = 16.0 / 9.0;
	camera.image_width = 800;		  /* was 400 */
	camera.samples_per_pixel = 400.0; /* was 100 */
	camera.max_depth = 50;
	camera.vfov = 20.0;
	camera.lookfrom = point3_create(13.0, 2.0, 3.0);
	camera.lookat = point3_create(0.0, 0.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.0;
	camera.background = vec3_create(0.70, 0.80, 1.00);

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render */
	camera_render(&camera, stdout, &world);

	/* Cleanup */
	ground_mat->destroy(ground_mat);
	free(ground_mat);
	hittable_list_clear(&world);
}

/* Scene: earth texture */
static void earth(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Load earth texture from PNG file (or use placeholder) */
	t_texture *earth_tex = image_texture_create_png("../assets/earthmap.png");
	if (!earth_tex)
	{
		fprintf(stderr, "Warning: Could not load earth texture, using solid color\n");
		earth_tex = solid_color_create(vec3_create(0.5, 0.7, 1.0));
	}

	t_material *earth_mat = lambertian_create_texture(earth_tex);

	/* Earth sphere */
	t_point3 center = point3_create(0.0, 0.0, 0.0);
	t_sphere earth_sphere = create_sphere(&center, 2.0, vec3_create(1.0, 1.0, 1.0), earth_mat);
	hittable_list_add_sphere(&world, &earth_sphere);

	/* Setup camera */
	t_camera camera;
	camera.aspect_ratio = 16.0 / 9.0;
	camera.image_width = 800;		  /* was 400 */
	camera.samples_per_pixel = 400.0; /* was 100 */
	camera.max_depth = 50;
	camera.vfov = 20.0;
	camera.lookfrom = point3_create(0.0, 0.0, 12.0);
	camera.lookat = point3_create(0.0, 0.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.0;
	camera.background = vec3_create(0.70, 0.80, 1.00);

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render */
	camera_render(&camera, stdout, &world);

	/* Cleanup */
	earth_mat->destroy(earth_mat);
	free(earth_mat);
	hittable_list_clear(&world);
}

/* Scene: quads with lights */
static void quads(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Create a diffuse light material */
	t_material *light_mat = diffuse_light_create(vec3_create(15.0, 15.0, 15.0));

	/* Create quads and add to world */
	t_point3 q = point3_create(-3.0, -2.0, -5.0);
	t_vec3 u = vec3_create(4.0, 0.0, 0.0);
	t_vec3 v = vec3_create(0.0, 4.0, 0.0);
	t_quad quad1 = quad_create(&q, &u, &v, light_mat);

	/* Add quad as non-owned object (quad is stack-allocated) */
	t_quad *quad_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad_copy)
	{
		*quad_copy = quad1;
		hittable_list_add_nonowned(&world, quad_copy, NULL,
								   (t_hit_noobj_fn)quad_hit, &quad1.bbox);
	}

	/* Ground sphere with diffuse material */
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_material *ground_mat = lambertian_create(vec3_create(0.3, 0.3, 0.3));
	t_sphere ground = create_sphere(&ground_center, 1000.0, vec3_create(1.0, 1.0, 1.0), ground_mat);
	hittable_list_add_sphere(&world, &ground);

	/* Setup camera */
	t_camera camera;
	camera.aspect_ratio = 1.0;
	camera.image_width = 800;		  /* was 400 */
	camera.samples_per_pixel = 400.0; /* was 100 */
	camera.max_depth = 50;
	camera.vfov = 80.0;
	camera.lookfrom = point3_create(0.0, 0.0, 9.0);
	camera.lookat = point3_create(0.0, 0.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.0;
	camera.background = vec3_create(0.70, 0.80, 1.00);

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render */
	camera_render(&camera, stdout, &world);

	/* Cleanup */
	light_mat->destroy(light_mat);
	free(light_mat);
	ground_mat->destroy(ground_mat);
	free(ground_mat);
	hittable_list_clear(&world);
}

/* Scene: simple light with quad lamp */
static void simple_light(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Create noise texture */
	t_texture *pertext = noise_texture_create((real_t)4.0);
	if (!pertext)
	{
		fprintf(stderr, "Failed to create noise texture\n");
		return;
	}

	/* Create lambertian material with noise texture */
	t_material *permat = lambertian_create_texture(pertext);
	if (!permat)
	{
		fprintf(stderr, "Failed to create lambertian material\n");
		pertext->destroy(pertext);
		free(pertext);
		return;
	}

	/* Ground and upper spheres */
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_sphere ground = create_sphere(&ground_center, 1000.0, vec3_create(1.0, 1.0, 1.0), permat);
	hittable_list_add_sphere(&world, &ground);

	t_point3 upper_center = point3_create(0.0, 2.0, 0.0);
	t_sphere upper = create_sphere(&upper_center, 2.0, vec3_create(1.0, 1.0, 1.0), permat);
	hittable_list_add_sphere(&world, &upper);

	/* Create diffuse light material */
	t_material *difflight = diffuse_light_create(vec3_create(4.0, 4.0, 4.0));
	if (!difflight)
	{
		fprintf(stderr, "Failed to create diffuse light material\n");
		permat->destroy(permat);
		free(permat);
		pertext->destroy(pertext);
		free(pertext);
		return;
	}

	/* Create quad lamp: position (3,1,-2), with u=(2,0,0) and v=(0,2,0) */
	t_point3 quad_pos = point3_create(3.0, 1.0, -2.0);
	t_vec3 quad_u = vec3_create(2.0, 0.0, 0.0);
	t_vec3 quad_v = vec3_create(0.0, 2.0, 0.0);
	t_quad quad = quad_create(&quad_pos, &quad_u, &quad_v, difflight);

	/* Add quad to world as non-owned object */
	t_quad *quad_copy = (t_quad *)malloc(sizeof(t_quad));
	if (quad_copy)
	{
		*quad_copy = quad;
		hittable_list_add_nonowned(&world, quad_copy, set_current_quad,
								   quad_hit_noobj, &quad.bbox);
	}

	/* Add glowing sphere above to illuminate more */
	t_point3 light_sphere_center = point3_create(0.0, 7.0, 0.0);
	t_sphere light_sphere = create_sphere(&light_sphere_center, 2.0, vec3_create(1.0, 1.0, 1.0), difflight);
	hittable_list_add_sphere(&world, &light_sphere);

	/* Setup camera */
	t_camera camera;
	camera.aspect_ratio = 16.0 / 9.0;
	camera.image_width = 800;
	camera.samples_per_pixel = 400.0;
	camera.max_depth = 50;
	camera.vfov = 20.0;
	camera.lookfrom = point3_create(26.0, 3.0, 6.0);
	camera.lookat = point3_create(0.0, 2.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.0;
	camera.background = vec3_create(0.0, 0.0, 0.0);

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render */
	camera_render(&camera, stdout, &world);

	/* Cleanup */
	difflight->destroy(difflight);
	free(difflight);
	permat->destroy(permat);
	free(permat);
	pertext->destroy(pertext);
	free(pertext);
	hittable_list_clear(&world);
}

/* Main: scene selector with numeric argument */
int main(int argc, char *argv[])
{
	int scene_num = 6; /* default: simple_light */

	if (argc > 1)
		scene_num = atoi(argv[1]);

	fprintf(stderr, "Rendering scene %d\n", scene_num);

	switch (scene_num)
	{
	case 1:
		bouncing_spheres();
		break;
	case 2:
		checkered_spheres();
		break;
	case 3:
		earth();
		break;
	case 4:
		perlin_spheres(); /* now uses improved settings */
		break;
	case 5:
		quads();
		break;
	case 6:
		simple_light();
		break;
	case 7:
		cornell_box();
		break;
	default:
		fprintf(stderr, "Unknown scene: %d\n", scene_num);
		fprintf(stderr, "Available scenes: 1 bouncing, 2 checkered, 3 earth, 4 perlin, 5 quads, 6 simple_light\n");
		return 1;
	}
	return 0;
}
