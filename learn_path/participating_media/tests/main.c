#include "../common.h"
#include "../constant_medium.h"

void cornell_smoke(void)
{
	t_hittable_list world;
	hittable_list_init(&world);

	/* Materials */
	t_material *red = lambertian_create(vec3_create(0.65, 0.05, 0.05));
	t_material *white = lambertian_create(vec3_create(0.73, 0.73, 0.73));
	t_material *green = lambertian_create(vec3_create(0.12, 0.45, 0.15));
	t_material *light = diffuse_light_create(vec3_create(7.0, 7.0, 7.0));

	if (!red || !white || !green || !light)
	{
		fprintf(stderr, "Failed to create materials\n");
		return;
	}

	/* Left wall (green) */
	{
		t_point3 q = point3_create(555.0, 0.0, 0.0);
		t_vec3 u = vec3_create(0.0, 555.0, 0.0);
		t_vec3 v = vec3_create(0.0, 0.0, 555.0);
		t_quad quad = quad_create(&q, &u, &v, green);
		t_quad *quad_copy = (t_quad *)malloc(sizeof(t_quad));
		if (quad_copy)
		{
			*quad_copy = quad;
			hittable_list_add_nonowned(&world, quad_copy, set_current_quad, quad_hit_noobj, &quad.bbox);
		}
	}

	/* Right wall (red) */
	{
		t_point3 q = point3_create(0.0, 0.0, 0.0);
		t_vec3 u = vec3_create(0.0, 555.0, 0.0);
		t_vec3 v = vec3_create(0.0, 0.0, 555.0);
		t_quad quad = quad_create(&q, &u, &v, red);
		t_quad *quad_copy = (t_quad *)malloc(sizeof(t_quad));
		if (quad_copy)
		{
			*quad_copy = quad;
			hittable_list_add_nonowned(&world, quad_copy, set_current_quad, quad_hit_noobj, &quad.bbox);
		}
	}

	/* Light (top) */
	{
		t_point3 q = point3_create(113.0, 554.0, 127.0);
		t_vec3 u = vec3_create(330.0, 0.0, 0.0);
		t_vec3 v = vec3_create(0.0, 0.0, 305.0);
		t_quad quad = quad_create(&q, &u, &v, light);
		t_quad *quad_copy = (t_quad *)malloc(sizeof(t_quad));
		if (quad_copy)
		{
			*quad_copy = quad;
			hittable_list_add_nonowned(&world, quad_copy, set_current_quad, quad_hit_noobj, &quad.bbox);
		}
	}

	/* Back wall (white) */
	{
		t_point3 q = point3_create(0.0, 555.0, 0.0);
		t_vec3 u = vec3_create(555.0, 0.0, 0.0);
		t_vec3 v = vec3_create(0.0, 0.0, 555.0);
		t_quad quad = quad_create(&q, &u, &v, white);
		t_quad *quad_copy = (t_quad *)malloc(sizeof(t_quad));
		if (quad_copy)
		{
			*quad_copy = quad;
			hittable_list_add_nonowned(&world, quad_copy, set_current_quad, quad_hit_noobj, &quad.bbox);
		}
	}

	/* Front wall (white) */
	{
		t_point3 q = point3_create(0.0, 0.0, 0.0);
		t_vec3 u = vec3_create(555.0, 0.0, 0.0);
		t_vec3 v = vec3_create(0.0, 0.0, 555.0);
		t_quad quad = quad_create(&q, &u, &v, white);
		t_quad *quad_copy = (t_quad *)malloc(sizeof(t_quad));
		if (quad_copy)
		{
			*quad_copy = quad;
			hittable_list_add_nonowned(&world, quad_copy, set_current_quad, quad_hit_noobj, &quad.bbox);
		}
	}

	/* Floor (white) */
	{
		t_point3 q = point3_create(0.0, 0.0, 555.0);
		t_vec3 u = vec3_create(555.0, 0.0, 0.0);
		t_vec3 v = vec3_create(0.0, 555.0, 0.0);
		t_quad quad = quad_create(&q, &u, &v, white);
		t_quad *quad_copy = (t_quad *)malloc(sizeof(t_quad));
		if (quad_copy)
		{
			*quad_copy = quad;
			hittable_list_add_nonowned(&world, quad_copy, set_current_quad, quad_hit_noobj, &quad.bbox);
		}
	}

	/* Keep handles for cleanup */
	t_constant_medium *medium1 = NULL;
	t_constant_medium *medium2 = NULL;
	t_translate_wrap *box1_final = NULL;
	t_rotate_y_wrap *box1_rotated = NULL;
	t_hittable_list *box1_list = NULL;
	t_translate_wrap *box2_final = NULL;
	t_rotate_y_wrap *box2_rotated = NULL;
	t_hittable_list *box2_list = NULL;

	/* Box 1: tall box with black smoke */
	{
		t_point3 a = point3_create(0.0, 0.0, 0.0);
		t_point3 b = point3_create(165.0, 400.0, 165.0); /* taller height */
		box1_list = (t_hittable_list *)malloc(sizeof(t_hittable_list));
		if (box1_list)
		{
			hittable_list_init(box1_list);
			box_create_list(&a, &b, white, box1_list);

			t_hittable_wrapper box1_wrapper = hittable_list_wrapper(box1_list);
			box1_rotated = rotate_y_create(&box1_wrapper, 15.0);
			if (box1_rotated)
			{
				t_vec3 offset = vec3_create(265.0, 0.0, 295.0);
				box1_final = translate_create(
					&(t_hittable_wrapper){
						.object = box1_rotated,
						.owned = false,
						.set_current = set_current_rotate,
						.hit_noobj = rotate_y_hit_noobj,
						.bbox = box1_rotated->bbox},
					&offset);

				if (box1_final)
				{
					t_hittable_wrapper translate_wrapper = {
						.object = box1_final,
						.owned = true,
						.set_current = set_current_translate,
						.hit_noobj = translate_hit_noobj,
						.bbox = box1_final->bbox};

					medium1 = constant_medium_create_color(&translate_wrapper, 0.01, vec3_create(0.0, 0.0, 0.0));
					if (medium1)
						hittable_list_add_nonowned(&world, medium1, set_current_medium, constant_medium_hit_noobj, &medium1->bbox);
				}
			}
		}
	}

	/* Box 2: short box with white smoke */
	{
		t_point3 a = point3_create(0.0, 0.0, 0.0);
		t_point3 b = point3_create(165.0, 165.0, 165.0);
		box2_list = (t_hittable_list *)malloc(sizeof(t_hittable_list));
		if (box2_list)
		{
			hittable_list_init(box2_list);
			box_create_list(&a, &b, white, box2_list);

			t_hittable_wrapper box2_wrapper = hittable_list_wrapper(box2_list);
			box2_rotated = rotate_y_create(&box2_wrapper, -18.0);
			if (box2_rotated)
			{
				t_vec3 offset = vec3_create(130.0, 0.0, 65.0);
				box2_final = translate_create(
					&(t_hittable_wrapper){
						.object = box2_rotated,
						.owned = false,
						.set_current = set_current_rotate,
						.hit_noobj = rotate_y_hit_noobj,
						.bbox = box2_rotated->bbox},
					&offset);

				if (box2_final)
				{
					t_hittable_wrapper translate_wrapper = {
						.object = box2_final,
						.owned = true,
						.set_current = set_current_translate,
						.hit_noobj = translate_hit_noobj,
						.bbox = box2_final->bbox};

					medium2 = constant_medium_create_color(&translate_wrapper, 0.01, vec3_create(1.0, 1.0, 1.0));
					if (medium2)
						hittable_list_add_nonowned(&world, medium2, set_current_medium, constant_medium_hit_noobj, &medium2->bbox);
				}
			}
		}
	}

	/* Setup camera */
	t_camera camera;
	camera.aspect_ratio = 1.0;
	camera.image_width = 600;
	camera.samples_per_pixel = 300.0;
	camera.max_depth = 50;
	camera.background = vec3_create(0.0, 0.0, 0.0);
	camera.vfov = 40.0;
	camera.lookfrom = point3_create(278.0, 278.0, -800.0);
	camera.lookat = point3_create(278.0, 278.0, 0.0);
	camera.vup = vec3_create(0.0, 1.0, 0.0);
	camera.defocus_angle = 0.0;
	camera.focus_dist = 10.0;

	camera_init(&camera, camera.aspect_ratio, camera.image_width);

	/* Render */
	camera_render(&camera, stdout, &world);

	/* Cleanup */
	hittable_list_clear(&world);
	if (medium1)
	{
		constant_medium_destroy(medium1);
	}
	if (medium2)
	{
		constant_medium_destroy(medium2);
	}
	if (box1_final)
		free(box1_final);
	if (box1_rotated)
		free(box1_rotated);
	if (box1_list)
	{
		hittable_list_clear(box1_list);
		free(box1_list);
	}
	if (box2_final)
		free(box2_final);
	if (box2_rotated)
		free(box2_rotated);
	if (box2_list)
	{
		hittable_list_clear(box2_list);
		free(box2_list);
	}
	red->destroy(red);
	free(red);
	white->destroy(white);
	free(white);
	green->destroy(green);
	free(green);
	light->destroy(light);
	free(light);
}

int main(void)
{
	cornell_smoke();
	return 0;
}
