/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 01:21:08 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 16:45:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"
#include "material.h"
#include "bvh.h"

/* simple helper to free all allocated materials */
static void cleanup_materials(t_material **arr, int count)
{
	for (int i = 0; i < count; ++i)
	{
		if (arr[i])
		{
			arr[i]->destroy(arr[i]);
			free(arr[i]);
		}
	}
}

int main(void)
{
	t_hittable_list world;
	t_material *materials[1024];
	int mat_count = 0;

	hittable_list_init(&world);
	srand(42);

	/* ground */
	t_material *ground_material = lambertian_create(vec3_create(0.5, 0.5, 0.5));
	if (!ground_material || mat_count >= 1024)
	{
		fprintf(stderr, "Error: failed to allocate ground material\n");
		cleanup_materials(materials, mat_count);
		return 1;
	}
	materials[mat_count++] = ground_material;
	t_point3 ground_center = point3_create(0.0, -1000.0, 0.0);
	t_sphere ground = create_sphere(&ground_center, 1000.0, vec3_create(0.5, 0.5, 0.5), ground_material);
	if (!hittable_list_add_sphere(&world, &ground))
	{
		fprintf(stderr, "Error: failed to add ground sphere\n");
		hittable_list_clear(&world);
		cleanup_materials(materials, mat_count);
		return 1;
	}

	/* random small spheres */
	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			real_t choose_mat = random_double();
			t_point3 center = point3_create(a + (real_t)0.9 * random_double(), 0.2, b + (real_t)0.9 * random_double());

			t_vec3 diff = vec3_sub(&center, &(t_point3){4.0, 0.2, 0.0});
			if (vec3_length(&diff) <= (real_t)0.9)
				continue;

			t_material *mat = NULL;
			t_vec3 albedo;
			real_t fuzz = 0.0;

			if (choose_mat < 0.8)
			{
				/* diffuse: create moving sphere */
				albedo = vec3_mul_elem(&(t_vec3){random_real(), random_real(), random_real()},
									   &(t_vec3){random_real(), random_real(), random_real()});
				mat = lambertian_create(albedo);

				/* center2 = center + vec3(0, random_double(0, 0.5), 0) */
				t_point3 center2 = point3_create(center.x, center.y + random_real_interval((real_t)0.0, (real_t)0.5), center.z);

				if (!mat || mat_count >= 1024)
				{
					fprintf(stderr, "Error: failed to allocate small sphere material\n");
					if (mat)
					{
						mat->destroy(mat);
						free(mat);
					}
					hittable_list_clear(&world);
					cleanup_materials(materials, mat_count);
					return 1;
				}
				materials[mat_count++] = mat;

				t_sphere s = create_sphere_moving(&center, &center2, 0.2, albedo, mat);
				if (!hittable_list_add_sphere(&world, &s))
				{
					fprintf(stderr, "Error: failed to add small sphere\n");
					hittable_list_clear(&world);
					cleanup_materials(materials, mat_count);
					return 1;
				}
			}
			else if (choose_mat < 0.95)
			{
				/* metal */
				albedo = vec3_random_interval((real_t)0.5, (real_t)1.0);
				fuzz = random_real_interval((real_t)0.0, (real_t)0.5);
				mat = metal_create_fuzz(albedo, fuzz);

				if (!mat || mat_count >= 1024)
				{
					fprintf(stderr, "Error: failed to allocate small sphere material\n");
					if (mat)
					{
						mat->destroy(mat);
						free(mat);
					}
					hittable_list_clear(&world);
					cleanup_materials(materials, mat_count);
					return 1;
				}
				materials[mat_count++] = mat;

				t_sphere s = create_sphere(&center, 0.2, albedo, mat);
				if (!hittable_list_add_sphere(&world, &s))
				{
					fprintf(stderr, "Error: failed to add small sphere\n");
					hittable_list_clear(&world);
					cleanup_materials(materials, mat_count);
					return 1;
				}
			}
			else
			{
				/* dielectric */
				mat = dielectric_create((real_t)1.5);

				if (!mat || mat_count >= 1024)
				{
					fprintf(stderr, "Error: failed to allocate small sphere material\n");
					if (mat)
					{
						mat->destroy(mat);
						free(mat);
					}
					hittable_list_clear(&world);
					cleanup_materials(materials, mat_count);
					return 1;
				}
				materials[mat_count++] = mat;

				t_sphere s = create_sphere(&center, 0.2, vec3_create(1.0, 1.0, 1.0), mat);
				if (!hittable_list_add_sphere(&world, &s))
				{
					fprintf(stderr, "Error: failed to add small sphere\n");
					hittable_list_clear(&world);
					cleanup_materials(materials, mat_count);
					return 1;
				}
			}
		}
	}

	/* three large spheres */
	t_material *material1 = dielectric_create((real_t)1.5);
	t_material *material2 = lambertian_create(vec3_create(0.4, 0.2, 0.1));
	t_material *material3 = metal_create_fuzz(vec3_create(0.7, 0.6, 0.5), (real_t)0.0);
	if (!material1 || !material2 || !material3 || mat_count + 3 > 1024)
	{
		fprintf(stderr, "Error: failed to allocate large sphere materials\n");
		if (material1)
		{
			material1->destroy(material1);
			free(material1);
		}
		if (material2)
		{
			material2->destroy(material2);
			free(material2);
		}
		if (material3)
		{
			material3->destroy(material3);
			free(material3);
		}
		hittable_list_clear(&world);
		cleanup_materials(materials, mat_count);
		return 1;
	}
	materials[mat_count++] = material1;
	materials[mat_count++] = material2;
	materials[mat_count++] = material3;

	t_point3 c1 = point3_create(0.0, 1.0, 0.0);
	t_point3 c2 = point3_create(-4.0, 1.0, 0.0);
	t_point3 c3 = point3_create(4.0, 1.0, 0.0);
	t_sphere s1 = create_sphere(&c1, 1.0, vec3_create(1.0, 1.0, 1.0), material1);
	t_sphere s2 = create_sphere(&c2, 1.0, vec3_create(0.4, 0.2, 0.1), material2);
	t_sphere s3 = create_sphere(&c3, 1.0, vec3_create(0.7, 0.6, 0.5), material3);

	/* Optional: create a moving sphere for motion blur demonstration */
	t_point3 c_start = point3_create(0.0, 0.5, -1.0);
	t_point3 c_end = point3_create(0.0, 1.5, -1.0);
	t_material *moving_mat = lambertian_create(vec3_create(0.9, 0.3, 0.3));
	if (!moving_mat || mat_count >= 1024)
	{
		fprintf(stderr, "Error: failed to allocate moving sphere material\n");
		hittable_list_clear(&world);
		cleanup_materials(materials, mat_count);
		return 1;
	}
	materials[mat_count++] = moving_mat;
	t_sphere s_moving = create_sphere_moving(&c_start, &c_end, 0.5, vec3_create(0.9, 0.3, 0.3), moving_mat);

	if (!hittable_list_add_sphere(&world, &s1) ||
		!hittable_list_add_sphere(&world, &s2) ||
		!hittable_list_add_sphere(&world, &s3) ||
		!hittable_list_add_sphere(&world, &s_moving))
	{
		fprintf(stderr, "Error: failed to add large spheres\n");
		hittable_list_clear(&world);
		cleanup_materials(materials, mat_count);
		return 1;
	}

	/* Build BVH tree from world */
	t_bvh_node *bvh_root = bvh_node_create(&world);
	if (!bvh_root)
	{
		fprintf(stderr, "Error: failed to create BVH tree\n");
		hittable_list_clear(&world);
		cleanup_materials(materials, mat_count);
		return 1;
	}

	/* Wrap BVH root in a hittable list for rendering */
	t_hittable_list world_bvh;
	hittable_list_init(&world_bvh);
	t_aabb bvh_bbox = bvh_root->bbox;
	if (!hittable_list_add_nonowned(&world_bvh, bvh_root, set_current_bvh, bvh_node_hit, &bvh_bbox))
	{
		fprintf(stderr, "Error: failed to wrap BVH root\n");
		bvh_node_destroy(bvh_root);
		hittable_list_clear(&world);
		cleanup_materials(materials, mat_count);
		return 1;
	}

	/* camera setup */
	t_camera cam;
	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20.0;
	cam.lookfrom = point3_create(13.0, 2.0, 3.0);
	cam.lookat = point3_create(0.0, 0.0, 0.0);
	cam.vup = vec3_create(0.0, 1.0, 0.0);

	cam.defocus_angle = 0.6;
	cam.focus_dist = 10.0;

	cam.u = vec3_zero();
	cam.v = vec3_zero();
	cam.w = vec3_zero();
	cam.defocus_disk_u = vec3_zero();
	cam.defocus_disk_v = vec3_zero();

	/* Render using BVH-accelerated world */
	camera_init(&cam, cam.aspect_ratio, cam.image_width);
	FILE *out = fopen("output.ppm", "w");
	if (!out)
	{
		fprintf(stderr, "Error: failed to open output file\n");
		bvh_node_destroy(bvh_root);
		hittable_list_clear(&world_bvh);
		hittable_list_clear(&world);
		cleanup_materials(materials, mat_count);
		return 1;
	}

	camera_render(&cam, out, &world_bvh);
	fclose(out);

	bvh_node_destroy(bvh_root);
	hittable_list_clear(&world_bvh);
	hittable_list_clear(&world);
	cleanup_materials(materials, mat_count);
	return 0;
}