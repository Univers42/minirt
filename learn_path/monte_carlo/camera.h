/* ============================================================================ */
/*                                                                              */
/*                                 FILE HEADER                                  */
/* ---------------------------------------------------------------------------- */
/*  File:       camera.h                                                        */
/*  Author:     dlesieur                                                        */
/*  Email:      dlesieur@student.42.fr                                          */
/*  Created:    2026/01/04 17:17:17                                             */
/*  Updated:    2026/01/04 17:17:17                                             */
/*                                                                              */
/* ============================================================================ */

#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "types.h"
#include "point.h"
#include "vector.h"
#include "ray.h"
#include "hittable_list.h"
#include "random.h"
#include "color.h"

/* Camera type */
typedef struct s_camera
{
	real_t aspect_ratio;
	real_t samples_per_pixel;
	real_t pixel_samples_scale;
	int image_width;
	int image_height;
	int max_depth;
	real_t vfov;
	t_point3 lookfrom;
	t_point3 lookat;
	t_vec3 vup;
	real_t defocus_angle;
	real_t focus_dist;
	t_color background;
	t_point3 center;
	t_point3 pixel00_loc;
	t_vec3 pixel_delta_u;
	t_vec3 pixel_delta_v;
	t_vec3 u;
	t_vec3 v;
	t_vec3 w;
	t_vec3 defocus_disk_u;
	t_vec3 defocus_disk_v;
	int sqrt_spp;
	real_t recip_sqrt_spp;
} t_camera;

/* Initialize camera in-place */
static inline void camera_init(t_camera *camera, real_t aspect_ratio, int image_width)
{
	real_t focal_length;
	real_t viewport_height;
	real_t viewport_width;
	real_t theta;
	real_t h;
	t_vec3 viewport_u;
	t_vec3 viewport_v;
	t_vec3 viewport_upper_left;
	real_t defocus_radius;

	if (!camera)
		return;

	camera->max_depth = 50;
	camera->aspect_ratio = (aspect_ratio > 0) ? aspect_ratio : (real_t)1.0;
	camera->image_width = (image_width > 0) ? image_width : 100;
	camera->image_height = (int)((real_t)camera->image_width / camera->aspect_ratio);
	if (camera->image_height < 1)
		camera->image_height = 1;

	/* Initialize background to black if not set */
	if (camera->background.x == 0.0 && camera->background.y == 0.0 && camera->background.z == 0.0)
		camera->background = vec3_create(0.0, 0.0, 0.0);

	camera->center = camera->lookfrom;

	/* Calculate w: direction from lookfrom to lookat, then negate */
	t_vec3 lookat_minus_lookfrom = vec3_sub(&camera->lookat, &camera->lookfrom);
	camera->w = unit_vector(&lookat_minus_lookfrom);
	camera->w = vec3_neg(&camera->w);

	/* Calculate focal_length from lookfrom - lookat distance */
	focal_length = vec3_length(&lookat_minus_lookfrom);
	if (focal_length < (real_t)1e-6)
		focal_length = (real_t)1.0;

	/* Determine viewport dimensions using vfov and focus_dist */
	theta = degrees_to_radians(camera->vfov);
	h = (real_t)tan((double)theta / 2.0);
	viewport_height = (real_t)(2.0 * h * camera->focus_dist);
	viewport_width = viewport_height * ((real_t)camera->image_width / (real_t)camera->image_height);

	/* Calculate camera basis vectors: u, v from w */
	t_vec3 cross_vup_w = cross(&camera->vup, &camera->w);
	camera->u = unit_vector(&cross_vup_w);
	camera->v = cross(&camera->w, &camera->u);

	/* Calculate viewport edges using camera basis vectors */
	viewport_u = vec3_mul_scalar(&camera->u, viewport_width);
	/* Negate v to go downward in image space */
	t_vec3 neg_v = vec3_neg(&camera->v);
	viewport_v = vec3_mul_scalar(&neg_v, viewport_height);

	camera->pixel_delta_u = vec3_div_scalar(&viewport_u, (real_t)camera->image_width);
	camera->pixel_delta_v = vec3_div_scalar(&viewport_v, (real_t)camera->image_height);

	/* Calculate viewport upper left */
	t_vec3 focal_w = vec3_mul_scalar(&camera->w, camera->focus_dist);
	t_vec3 center_minus_focal = vec3_sub(&camera->center, &focal_w);
	t_vec3 half_u = vec3_div_scalar(&viewport_u, (real_t)2.0);
	t_vec3 half_v = vec3_div_scalar(&viewport_v, (real_t)2.0);
	viewport_upper_left = vec3_sub(&center_minus_focal, &half_u);
	viewport_upper_left = vec3_sub(&viewport_upper_left, &half_v);

	if (camera->samples_per_pixel <= (real_t)0.0)
		camera->samples_per_pixel = (real_t)1.0;

	/* Compute sqrt_spp and pixel_samples_scale for stratified sampling */
	camera->sqrt_spp = (int)sqrt((double)camera->samples_per_pixel);
	camera->recip_sqrt_spp = (real_t)1.0 / (real_t)camera->sqrt_spp;
	camera->pixel_samples_scale = (real_t)1.0 / (camera->sqrt_spp * camera->sqrt_spp);

	t_vec3 sum = vec3_add(&camera->pixel_delta_u, &camera->pixel_delta_v);
	t_vec3 half_sum = vec3_mul_scalar(&sum, (real_t)0.5);
	camera->pixel00_loc = vec3_add(&viewport_upper_left, &half_sum);

	/* Calculate defocus disk basis vectors */
	defocus_radius = camera->focus_dist * (real_t)tan((double)degrees_to_radians(camera->defocus_angle / (real_t)2.0));
	camera->defocus_disk_u = vec3_mul_scalar(&camera->u, defocus_radius);
	camera->defocus_disk_v = vec3_mul_scalar(&camera->v, defocus_radius);
}

/* Return a random point in the [-0.5,-0.5] .. [+0.5,+0.5] square (z = 0) */
static inline t_vec3 sample_square(void)
{
	return vec3_create(random_real() - (real_t)0.5, random_real() - (real_t)0.5, (real_t)0.0);
}

/* Stratified sampling: divide pixel into sqrt_spp x sqrt_spp grid and sample within [s_i, s_j] cell */
static inline t_vec3 sample_square_stratified(int s_i, int s_j, real_t recip_sqrt_spp)
{
	real_t px = ((real_t)s_i + random_real()) * recip_sqrt_spp - (real_t)0.5;
	real_t py = ((real_t)s_j + random_real()) * recip_sqrt_spp - (real_t)0.5;
	return vec3_create(px, py, (real_t)0.0);
}

/* Return a random point on the defocus disk */
static inline t_vec3 defocus_disk_sample(const t_camera *cam)
{
	t_vec3 p = random_in_unit_disk();
	t_vec3 p0_term = vec3_mul_scalar(&cam->defocus_disk_u, p.x);
	t_vec3 p1_term = vec3_mul_scalar(&cam->defocus_disk_v, p.y);
	t_vec3 offset = vec3_add(&p0_term, &p1_term);
	return vec3_add(&cam->center, &offset);
}

/* Construct a camera ray with stratified sampling: originates from defocus disk (or center)
   and directed at a randomly sampled point around pixel (i,j) in stratified sub-cell (s_i, s_j) */
static inline t_ray get_ray_stratified(const t_camera *cam, int i, int j, int s_i, int s_j)
{
	t_vec3 offset = sample_square_stratified(s_i, s_j, cam->recip_sqrt_spp);

	/* tmp_u = (i + offset.x) * pixel_delta_u */
	t_vec3 tmp_u = vec3_mul_scalar(&cam->pixel_delta_u, (real_t)(i + offset.x));
	/* tmp_v = (j + offset.y) * pixel_delta_v */
	t_vec3 tmp_v = vec3_mul_scalar(&cam->pixel_delta_v, (real_t)(j + offset.y));

	t_vec3 tmp = vec3_add(&cam->pixel00_loc, &tmp_u);
	t_vec3 pixel_sample = vec3_add(&tmp, &tmp_v);

	/* Ray origin: defocus disk if enabled, otherwise camera center */
	t_vec3 ray_origin = (cam->defocus_angle <= (real_t)0.0) ? cam->center : defocus_disk_sample(cam);

	/* Ray direction: from origin to pixel sample */
	t_vec3 ray_direction = vec3_sub(&pixel_sample, &ray_origin);

	/* Random time in [0, 1] for motion blur */
	real_t ray_time = random_real();

	return ray_create(ray_origin, ray_direction, ray_time);
}

/* Legacy get_ray: unstratified sampling (kept for compatibility) */
static inline t_ray get_ray(const t_camera *cam, int i, int j)
{
	return get_ray_stratified(cam, i, j, 0, 0);
}

/* Ensure output directory exists */
static inline void ensure_output_dir(void)
{
	const char *output_dir = "../output";
	struct stat st = {0};
	if (stat(output_dir, &st) == -1)
	{
		mkdir(output_dir, 0755);
	}
}

/* Generate unique filename in output directory */
static inline void get_output_filename(char *filename, size_t size, const char *base_name)
{
	ensure_output_dir();
	snprintf(filename, size, "../output/%s.ppm", base_name);
}

/* ray_color_depth with material emission, scattering PDF, and background support */
static inline t_vec3 ray_color_with_background(const t_ray *r, const t_hittable_list *world, int depth, const t_color *background)
{
	t_hit_record rec;

	if (depth <= 0)
		return vec3_zero();

	if (!hittable_list_hit(world, r, interval((real_t)1e-4, INFINITY), &rec))
		return *background;

	t_ray scattered;
	t_color attenuation;
	t_color emission = vec3_zero();

	/* Get emitted color from material */
	if (rec.mat && rec.mat->emitted)
		emission = rec.mat->emitted(rec.mat, rec.u, rec.v, &rec.p);

	/* If material exists and scatters, combine emission with scattered light */
	if (rec.mat && rec.mat->scatter(rec.mat, r, &rec, &attenuation, &scattered))
	{
		/* Get scattering PDF from material */
		real_t scattering_pdf = (rec.mat->scattering_pdf) ? rec.mat->scattering_pdf(rec.mat, r, &rec, &scattered) : (real_t)0.0;

		/* Use uniform PDF value: 1 / (2*pi) */
		real_t pdf_value = (real_t)1.0 / ((real_t)2.0 * (real_t)PI);

		/* Recursively compute scattered color */
		t_vec3 scattered_col = ray_color_with_background(&scattered, world, depth - 1, background);

		/* Combine: attenuation * scattering_pdf * scattered_color / pdf_value */
		t_vec3 weighted = vec3_mul_scalar(&scattered_col, scattering_pdf / pdf_value);
		t_vec3 attenuated = vec3_mul_elem(&attenuation, &weighted);

		return vec3_add(&emission, &attenuated);
	}

	/* Material does not scatter: return only emission */
	return emission;
}

/* Render function with stratified sampling */
static inline void camera_render(const t_camera *camera, FILE *out, const t_hittable_list *world)
{
	(void)out; /* unused: we write to file instead */

	if (!camera)
		return;

	/* Generate output filename */
	char filename[256];
	snprintf(filename, sizeof(filename), "../output/render.ppm");

	/* Open output file */
	FILE *ppm_file = fopen(filename, "w");
	if (!ppm_file)
	{
		fprintf(stderr, "Error: cannot open file %s for writing\n", filename);
		return;
	}

	fprintf(ppm_file, "P3\n%d %d\n255\n", camera->image_width, camera->image_height);

	for (int j = 0; j < camera->image_height; ++j)
	{
		fprintf(stderr, "\rScanlines remaining: %d ", (camera->image_height - j));
		fflush(stderr);
		for (int i = 0; i < camera->image_width; ++i)
		{
			t_color pixel_color = vec3_zero();
			/* Stratified sampling: loop over sqrt_spp x sqrt_spp grid */
			for (int s_j = 0; s_j < camera->sqrt_spp; ++s_j)
			{
				for (int s_i = 0; s_i < camera->sqrt_spp; ++s_i)
				{
					t_ray r = get_ray_stratified(camera, i, j, s_i, s_j);
					t_vec3 sample_color = ray_color_with_background(&r, world, camera->max_depth, &camera->background);
					pixel_color = vec3_add(&pixel_color, &sample_color);
				}
			}
			t_vec3 scaled = vec3_mul_scalar(&pixel_color, camera->pixel_samples_scale);
			write_color(ppm_file, &scaled);
		}
	}
	fprintf(stderr, "\rDone.                 \n");
	fprintf(stderr, "Rendered image saved to: %s\n", filename);

	fclose(ppm_file);
}

#endif