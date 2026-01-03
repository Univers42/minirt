/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 23:53:07 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 15:29:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include "types.h"
#include "point.h"
#include "vector.h"
#include "ray.h"
#include "hittable_list.h"
#include "random.h"
#include "color.h"
#include "thread_pool.h"

/* Thread-safe progress tracker */
typedef struct s_progress
{
	int completed_rows;
	int total_rows;
	int last_reported;
#ifdef USE_PTHREAD
	pthread_mutex_t mutex;
#endif
} t_progress;

static inline t_progress *progress_create(int total_rows)
{
	t_progress *prog = (t_progress *)malloc(sizeof(t_progress));
	if (!prog)
		return NULL;
	prog->completed_rows = 0;
	prog->total_rows = total_rows;
	prog->last_reported = 0;
#ifdef USE_PTHREAD
	pthread_mutex_init(&prog->mutex, NULL);
#endif
	return prog;
}

static inline void progress_destroy(t_progress *prog)
{
	if (!prog)
		return;
#ifdef USE_PTHREAD
	pthread_mutex_destroy(&prog->mutex);
#endif
	free(prog);
}

static inline void progress_increment(t_progress *prog, int rows)
{
	if (!prog)
		return;
#ifdef USE_PTHREAD
	pthread_mutex_lock(&prog->mutex);
#endif
	prog->completed_rows += rows;
#ifdef USE_PTHREAD
	pthread_mutex_unlock(&prog->mutex);
#endif
}

static inline int progress_get(t_progress *prog)
{
	if (!prog)
		return 0;
#ifdef USE_PTHREAD
	pthread_mutex_lock(&prog->mutex);
#endif
	int completed = prog->completed_rows;
#ifdef USE_PTHREAD
	pthread_mutex_unlock(&prog->mutex);
#endif
	return completed;
}

/* Report progress if threshold reached (every 5% or 10 rows, whichever is less) */
static inline bool progress_should_report(t_progress *prog, int completed)
{
	if (!prog)
		return false;
	int threshold = (prog->total_rows / 20); /* 5% */
	if (threshold < 1)
		threshold = 1;
	return (completed - prog->last_reported) >= threshold;
}

static inline void progress_mark_reported(t_progress *prog, int completed)
{
	prog->last_reported = completed;
}

/* Pixel buffer for thread-safe rendering */
typedef struct s_pixel_buffer
{
	t_color *pixels;
	int width;
	int height;
	/* per-pixel mutexes removed: tasks own disjoint rows */
} t_pixel_buffer;

static inline t_pixel_buffer *pixel_buffer_create(int width, int height)
{
	t_pixel_buffer *buf = (t_pixel_buffer *)malloc(sizeof(t_pixel_buffer));
	if (!buf)
		return NULL;

	buf->width = width;
	buf->height = height;
	buf->pixels = (t_color *)calloc(width * height, sizeof(t_color));
	if (!buf->pixels)
	{
		free(buf);
		return NULL;
	}
	return buf;
}

static inline void pixel_buffer_destroy(t_pixel_buffer *buf)
{
	if (!buf)
		return;
	free(buf->pixels);
	free(buf);
}

static inline void pixel_buffer_set(t_pixel_buffer *buf, int x, int y, t_color color)
{
	int idx = y * buf->width + x;
	buf->pixels[idx] = color;
}

/* Camera type - declare early so render_task can use it */
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
	t_point3 center;
	t_point3 pixel00_loc;
	t_vec3 pixel_delta_u;
	t_vec3 pixel_delta_v;
	t_vec3 u;
	t_vec3 v;
	t_vec3 w;
	t_vec3 defocus_disk_u;
	t_vec3 defocus_disk_v;
	int num_threads; /* Number of threads for rendering */
} t_camera;

/* Return a random point in the [-0.5,-0.5] .. [+0.5,+0.5] square (z = 0) */
static inline t_vec3 sample_square(void)
{
	return vec3_create(random_real() - (real_t)0.5, random_real() - (real_t)0.5, (real_t)0.0);
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

/* construct a camera ray originating from the defocus disk (or center) and directed at a
   randomly sampled point around pixel (i,j) */
static inline t_ray get_ray(const t_camera *cam, int i, int j)
{
	t_vec3 offset = sample_square();

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
	return ray_create(ray_origin, ray_direction);
}

/* Render task argument - NOW after t_camera and get_ray are defined */
typedef struct s_render_task
{
	const t_camera *camera;
	const t_hittable_list *world;
	t_pixel_buffer *buffer;
	t_progress *progress;
	int start_row;
	int end_row;
} t_render_task;

/* Task function for rendering rows */
static inline void render_rows_task(void *arg)
{
	t_render_task *task = (t_render_task *)arg;

	for (int j = task->start_row; j < task->end_row; ++j)
	{
		for (int i = 0; i < task->camera->image_width; ++i)
		{
			t_color pixel_color = vec3_zero();
			for (int sample = 0; sample < (int)task->camera->samples_per_pixel; ++sample)
			{
				t_ray r = get_ray(task->camera, i, j);
				t_vec3 sample_color = ray_color_depth(&r, task->world, task->camera->max_depth);
				pixel_color = vec3_add(&pixel_color, &sample_color);
			}
			t_vec3 scaled = vec3_mul_scalar(&pixel_color, task->camera->pixel_samples_scale);
			pixel_buffer_set(task->buffer, i, j, scaled);
		}
		/* Update progress after each row */
		progress_increment(task->progress, 1);

		/* Report progress periodically */
		int completed = progress_get(task->progress);
		if (progress_should_report(task->progress, completed))
		{
			int remaining = task->camera->image_height - completed;
			int percent = (completed * 100) / task->camera->image_height;
			fprintf(stderr, "\rRendering: %d%% (%d/%d scanlines) %d remaining  ",
					percent, completed, task->camera->image_height, remaining);
			fflush(stderr);
			progress_mark_reported(task->progress, completed);
		}
	}
	free(task);
}

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
	camera->pixel_samples_scale = (real_t)1.0 / camera->samples_per_pixel;

	t_vec3 sum = vec3_add(&camera->pixel_delta_u, &camera->pixel_delta_v);
	t_vec3 half_sum = vec3_mul_scalar(&sum, (real_t)0.5);
	camera->pixel00_loc = vec3_add(&viewport_upper_left, &half_sum);

	/* Calculate defocus disk basis vectors */
	defocus_radius = camera->focus_dist * (real_t)tan((double)degrees_to_radians(camera->defocus_angle / (real_t)2.0));
	camera->defocus_disk_u = vec3_mul_scalar(&camera->u, defocus_radius);
	camera->defocus_disk_v = vec3_mul_scalar(&camera->v, defocus_radius);
}

/* Render function: stream PPM output to `out` using given world */
static inline void camera_render(const t_camera *camera, FILE *out, const t_hittable_list *world)
{
	if (!camera || !out)
		return;

	fprintf(out, "P3\n%d %d\n255\n", camera->image_width, camera->image_height);

	/* Create pixel buffer */
	t_pixel_buffer *buffer = pixel_buffer_create(camera->image_width, camera->image_height);
	if (!buffer)
	{
		fprintf(stderr, "Error: failed to allocate pixel buffer\n");
		return;
	}

	/* Create progress tracker */
	t_progress *progress = progress_create(camera->image_height);
	if (!progress)
	{
		fprintf(stderr, "Error: failed to allocate progress tracker\n");
		pixel_buffer_destroy(buffer);
		return;
	}

	/* Determine number of threads */
	int num_threads = (camera->num_threads > 0) ? camera->num_threads : 1;

#ifdef USE_PTHREAD
	fprintf(stderr, "Rendering with %d threads...\n", num_threads);
	fprintf(stderr, "Image: %dx%d, Samples: %d per pixel, Max depth: %d\n",
			camera->image_width, camera->image_height,
			(int)camera->samples_per_pixel, camera->max_depth);
#else
	fprintf(stderr, "Rendering (single-threaded, pthread disabled)...\n");
	num_threads = 1;
#endif

	/* Create thread pool */
	t_thread_pool *pool = thread_pool_create(num_threads);
	if (!pool)
	{
		fprintf(stderr, "Error: failed to create thread pool\n");
		pixel_buffer_destroy(buffer);
		progress_destroy(progress);
		return;
	}

	/* Divide work into chunks (rows per task) */
	int rows_per_task = (camera->image_height + num_threads - 1) / num_threads;

	for (int t = 0; t < num_threads; ++t)
	{
		int start_row = t * rows_per_task;
		int end_row = start_row + rows_per_task;
		if (end_row > camera->image_height)
			end_row = camera->image_height;

		if (start_row >= camera->image_height)
			break;

		t_render_task *task = (t_render_task *)malloc(sizeof(t_render_task));
		if (!task)
			continue;

		task->camera = camera;
		task->world = world;
		task->buffer = buffer;
		task->progress = progress;
		task->start_row = start_row;
		task->end_row = end_row;

		thread_pool_add_task(pool, render_rows_task, task);
	}

	fprintf(stderr, "Starting render...\n");

	/* Wait for all tasks to complete */
	thread_pool_wait(pool);
	thread_pool_destroy(pool);

	/* Write buffer to output */
	fprintf(stderr, "\nWriting to file...\n");
	for (int j = 0; j < camera->image_height; ++j)
	{
		if (j % 100 == 0 || j == camera->image_height - 1)
		{
			fprintf(stderr, "\rWriting scanline: %d / %d ", j + 1, camera->image_height);
			fflush(stderr);
		}
		for (int i = 0; i < camera->image_width; ++i)
		{
			int idx = j * camera->image_width + i;
			write_color(out, &buffer->pixels[idx]);
		}
	}

	pixel_buffer_destroy(buffer);
	progress_destroy(progress);
	fprintf(stderr, "\rDone.                                                                       \n");
}

#endif