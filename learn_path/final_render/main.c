/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 01:21:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 15:27:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "material.h"

/* Helper: generate random color */
static inline t_vec3 random_color(void)
{
	return vec3_create(random_real(), random_real(), random_real());
}

/* Helper: generate random color in range [min, max] */
static inline t_vec3 random_color_interval(real_t min, real_t max)
{
	return vec3_create(random_real_interval(min, max),
					   random_real_interval(min, max),
					   random_real_interval(min, max));
}

int main(int argc, char **argv)
{
	/* Parse command-line arguments */
	int num_threads = 1;
	if (argc > 1)
	{
		num_threads = atoi(argv[1]);
		if (num_threads <= 0)
		{
			fprintf(stderr, "Usage: %s [num_threads]\n", argv[0]);
			fprintf(stderr, "num_threads must be positive, defaulting to 1\n");
			num_threads = 1;
		}
	}

	fprintf(stderr, "Starting render with %d thread(s)\n", num_threads);

	/* Create world */
	t_hittable_list world;
	hittable_list_init(&world);

	/* Ground material and sphere */
	t_material *ground_material = lambertian_create(vec3_create(0.5, 0.5, 0.5));
	if (!ground_material)
	{
		fprintf(stderr, "Error: failed to allocate ground material\n");
		return 1;
	}
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_sphere ground_sphere = create_sphere(&ground_center, 1000.0, vec3_create(0.5, 0.5, 0.5), ground_material);
	hittable_list_add_sphere(&world, &ground_sphere);

	/* Procedurally generate small random spheres in a grid */
	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			real_t choose_mat = random_real();
			real_t rand_offset_x = 0.9 * random_real();
			real_t rand_offset_z = 0.9 * random_real();
			real_t center_x = (real_t)a + rand_offset_x;
			real_t center_z = (real_t)b + rand_offset_z;
			t_point3 center = point3_create(center_x, 0.2, center_z);

			/* Check distance from center sphere position (4, 0.2, 0) */
			t_vec3 offset = vec3_sub(&center, &((t_point3){4.0, 0.2, 0.0}));
			real_t dist = vec3_length(&offset);

			if (dist > 0.9)
			{
				t_material *sphere_material = NULL;

				if (choose_mat < 0.8)
				{
					/* Diffuse (Lambertian) */
					t_vec3 albedo1 = random_color();
					t_vec3 albedo2 = random_color();
					t_vec3 albedo = vec3_mul_elem(&albedo1, &albedo2);
					sphere_material = lambertian_create(albedo);
				}
				else if (choose_mat < 0.95)
				{
					/* Metal */
					t_vec3 albedo = random_color_interval(0.5, 1.0);
					real_t fuzz = random_real_interval(0.0, 0.5);
					sphere_material = metal_create_fuzz(albedo, fuzz);
				}
				else
				{
					/* Glass (Dielectric) */
					sphere_material = dielectric_create(1.5);
				}

				if (sphere_material)
				{
					t_sphere s = create_sphere(&center, 0.2, vec3_create(1.0, 1.0, 1.0), sphere_material);
					hittable_list_add_sphere(&world, &s);
				}
			}
		}
	}

	/* Three prominent spheres */
	/* Sphere 1: Glass (Dielectric) at (0, 1, 0) */
	t_material *material1 = dielectric_create(1.5);
	if (!material1)
	{
		fprintf(stderr, "Error: failed to allocate material1\n");
		hittable_list_clear(&world);
		ground_material->destroy(ground_material);
		free(ground_material);
		return 1;
	}
	t_point3 p1 = point3_create(0.0, 1.0, 0.0);
	t_sphere sphere1 = create_sphere(&p1, 1.0, vec3_create(1.0, 1.0, 1.0), material1);
	hittable_list_add_sphere(&world, &sphere1);

	/* Sphere 2: Diffuse (Lambertian) at (-4, 1, 0) */
	t_material *material2 = lambertian_create(vec3_create(0.4, 0.2, 0.1));
	if (!material2)
	{
		fprintf(stderr, "Error: failed to allocate material2\n");
		hittable_list_clear(&world);
		ground_material->destroy(ground_material);
		material1->destroy(material1);
		free(ground_material);
		free(material1);
		return 1;
	}
	t_point3 p2 = point3_create(-4.0, 1.0, 0.0);
	t_sphere sphere2 = create_sphere(&p2, 1.0, vec3_create(0.4, 0.2, 0.1), material2);
	hittable_list_add_sphere(&world, &sphere2);

	/* Sphere 3: Metal at (4, 1, 0) */
	t_material *material3 = metal_create_fuzz(vec3_create(0.7, 0.6, 0.5), 0.0);
	if (!material3)
	{
		fprintf(stderr, "Error: failed to allocate material3\n");
		hittable_list_clear(&world);
		ground_material->destroy(ground_material);
		material1->destroy(material1);
		material2->destroy(material2);
		free(ground_material);
		free(material1);
		free(material2);
		return 1;
	}
	t_point3 p3 = point3_create(4.0, 1.0, 0.0);
	t_sphere sphere3 = create_sphere(&p3, 1.0, vec3_create(0.7, 0.6, 0.5), material3);
	hittable_list_add_sphere(&world, &sphere3);

	/* Initialize camera */
	t_camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 1200;
	cam.samples_per_pixel = 10;
	cam.max_depth = 50;
	cam.num_threads = num_threads;

	cam.vfov = 20.0;
	cam.lookfrom = point3_create(13.0, 2.0, 3.0);
	cam.lookat = point3_create(0.0, 0.0, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);

	cam.defocus_angle = 0.6;
	cam.focus_dist = 10.0;

	/* Initialize basis vectors to avoid uninitialized values */
	cam.u = vec3_zero();
	cam.v = vec3_zero();
	cam.w = vec3_zero();
	cam.defocus_disk_u = vec3_zero();
	cam.defocus_disk_v = vec3_zero();

	srand(42);
	camera_init(&cam, cam.aspect_ratio, cam.image_width);

	/* Render to file */
	FILE *out = fopen("output.ppm", "w");
	if (!out)
	{
		fprintf(stderr, "Error: failed to open output file\n");
		hittable_list_clear(&world);
		ground_material->destroy(ground_material);
		material1->destroy(material1);
		material2->destroy(material2);
		material3->destroy(material3);
		free(ground_material);
		free(material1);
		free(material2);
		free(material3);
		return 1;
	}

	camera_render(&cam, out, &world);
	fclose(out);

	/* Cleanup */
	hittable_list_clear(&world);
	ground_material->destroy(ground_material);
	material1->destroy(material1);
	material2->destroy(material2);
	material3->destroy(material3);
	free(ground_material);
	free(material1);
	free(material2);
	free(material3);

	return 0;
}