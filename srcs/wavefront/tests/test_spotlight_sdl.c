/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_spotlight_sdl.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 00:41:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** Spotlight scene: open dark environment with colored spotlights
** aimed at a central OBJ model on a ground plane.  Shows emission /
** color-bleeding effects from multiple light sources.
**
** Usage:  ./test_spotlight_sdl [obj_file] [size] [material]
**   obj_file  : path to .obj (default: input/dragon.obj)
**   size      : target size in scene units (default: 3)
**   material  : gold | glass | chrome | white | marble (default: white)
**
** Example: ./test_spotlight_sdl input/bmw.obj 4 chrome
*/

#include "common.h"
#include "quad.h"
#include "bvh.h"
#include "hittable.h"
#include "triangle.h"
#include "wf_rt.h"
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <time.h>
#ifdef _OPENMP
# include <omp.h>
#endif

/* ------------------------------------------------------------------ */
/*  Helpers                                                           */
/* ------------------------------------------------------------------ */

static bool	add_quad(t_hittable_list *w, t_point3 *p,
				t_vec3 uv[2], t_material *mat)
{
	t_quad	q;
	t_quad	*qp;

	q = quad_create(p, &uv[0], &uv[1], mat);
	qp = (t_quad *)malloc(sizeof(t_quad));
	if (!qp)
		return (false);
	*qp = q;
	return (hittable_list_add_nonowned(w, qp, set_current_quad,
			quad_hit_noobj, &q.bbox));
}

static bool	add_sphere(t_hittable_list *w, t_point3 center,
				real_t radius, t_material *mat)
{
	t_sphere	s;
	t_sphere	*sp;
	t_vec3		alb;

	alb = vec3_create(1.0, 1.0, 1.0);
	s = create_sphere(&center, radius, alb, mat);
	sp = (t_sphere *)malloc(sizeof(t_sphere));
	if (!sp)
		return (false);
	*sp = s;
	return (hittable_list_add_nonowned(w, sp, set_current_sphere,
			sphere_hit_noobj, &s.bbox));
}

/* ------------------------------------------------------------------ */
/*  Ground plane: large matte grey quad                               */
/* ------------------------------------------------------------------ */

static void	add_ground(t_hittable_list *w)
{
	t_material	*grey;
	t_point3	p;
	t_vec3		uv[2];

	grey = lambertian_create(vec3_create(0.5, 0.5, 0.5));
	p = point3_create(-50.0, 0.0, -50.0);
	uv[0] = vec3_create(100.0, 0.0, 0.0);
	uv[1] = vec3_create(0.0, 0.0, 100.0);
	add_quad(w, &p, uv, grey);
}

/* ------------------------------------------------------------------ */
/*  Spotlights: small bright emissive quads angled toward the model   */
/*  Each one casts a different colour                                 */
/* ------------------------------------------------------------------ */

static void	add_spot_quad(t_hittable_list *w,
				t_point3 p, t_vec3 uv[2], t_color colour)
{
	t_material	*mat;

	mat = diffuse_light_create_scaled(colour, 25.0);
	add_quad(w, &p, uv, mat);
}

static void	add_spotlights(t_hittable_list *w)
{
	t_point3	p;
	t_vec3		uv[2];

	/* warm white overhead key-light (slightly forward) */
	p = point3_create(-1.0, 9.0, -1.0);
	uv[0] = vec3_create(2.0, 0.0, 0.0);
	uv[1] = vec3_create(0.0, 0.0, 2.0);
	add_spot_quad(w, p, uv, vec3_create(1.0, 0.95, 0.8));

	/* red spot – left */
	p = point3_create(-8.0, 5.0, -1.0);
	uv[0] = vec3_create(0.0, 0.0, 2.0);
	uv[1] = vec3_create(0.0, 2.0, 0.0);
	add_spot_quad(w, p, uv, vec3_create(1.0, 0.1, 0.1));

	/* blue spot – right */
	p = point3_create(8.0, 5.0, -1.0);
	uv[0] = vec3_create(0.0, 0.0, 2.0);
	uv[1] = vec3_create(0.0, 2.0, 0.0);
	add_spot_quad(w, p, uv, vec3_create(0.1, 0.2, 1.0));

	/* green spot – behind / high */
	p = point3_create(-1.0, 7.0, 6.0);
	uv[0] = vec3_create(2.0, 0.0, 0.0);
	uv[1] = vec3_create(0.0, 2.0, 0.0);
	add_spot_quad(w, p, uv, vec3_create(0.1, 1.0, 0.2));

	/* small bright fill sphere light in front */
	add_sphere(w, point3_create(0.0, 3.0, -7.0), 0.5,
		diffuse_light_create_scaled(vec3_create(1.0, 1.0, 1.0), 15.0));
}

/* ------------------------------------------------------------------ */
/*  CLI option parsing                                                */
/* ------------------------------------------------------------------ */

typedef struct s_obj_opts
{
	const char	*path;
	float		size;
	const char	*matname;
}	t_obj_opts;

static t_material	*pick_material(const char *name)
{
	if (strcmp(name, "glass") == 0)
		return (dielectric_create(1.5));
	if (strcmp(name, "chrome") == 0)
		return (metal_create_fuzz(vec3_create(0.95, 0.93, 0.88), 0.0));
	if (strcmp(name, "gold") == 0)
		return (metal_create_fuzz(vec3_create(0.85, 0.65, 0.13), 0.02));
	if (strcmp(name, "marble") == 0)
		return (lambertian_create(vec3_create(0.9, 0.85, 0.78)));
	/* default: white diffuse */
	return (lambertian_create(vec3_create(0.73, 0.73, 0.73)));
}

static void	parse_opts(t_obj_opts *o, int ac, char **av)
{
	o->path = "input/dragon.obj";
	o->size = 3.0f;
	o->matname = "white";
	if (ac > 1)
		o->path = av[1];
	if (ac > 2)
		o->size = (float)atof(av[2]);
	if (ac > 3)
		o->matname = av[3];
}

static void	add_obj_model(t_hittable_list *w, const t_obj_opts *o)
{
	t_material	*mat;

	mat = pick_material(o->matname);
	fprintf(stderr,
		"Loading %s  (size=%.1f  mat=%s)\n", o->path, o->size, o->matname);
	/* centre the model at (0, size/2, 0) so it sits on ground */
	if (!wf_obj_to_hittable_list(o->path, w, mat,
			(double)o->size, 0.0, (double)o->size * 0.5, 0.0))
		fprintf(stderr, "  ERROR: cannot load %s\n", o->path);
	else
		fprintf(stderr, "  OK\n");
}

/* ------------------------------------------------------------------ */
/*  Render helpers (same as cornell scene)                            */
/* ------------------------------------------------------------------ */

static void	render_pixel_row(const t_camera *cam,
				const t_hittable_list *w, t_vec3 *pixels, int j)
{
	int		i;
	int		si;
	int		sj;
	t_color	pc;
	t_ray	r;
	t_vec3	sc;

	i = 0;
	while (i < cam->image_width)
	{
		pc = vec3_zero();
		sj = 0;
		while (sj < cam->sqrt_spp)
		{
			si = 0;
			while (si < cam->sqrt_spp)
			{
				r = get_ray_stratified(cam, i, j, si, sj);
				sc = ray_color_with_background(&r, w,
						cam->max_depth, &cam->background);
				pc = vec3_add(&pc, &sc);
				si++;
			}
			sj++;
		}
		pixels[j * cam->image_width + i] = vec3_mul_scalar(&pc,
				cam->pixel_samples_scale);
		i++;
	}
}

static void	render_to_buffer(const t_camera *cam,
				const t_hittable_list *w,
				t_vec3 *pixels)
{
	int		rows_done;
	int		h;
	clock_t	start;
	char	buf[64];

	h = cam->image_height;
	rows_done = 0;
	start = clock();
	fprintf(stderr, "Starting render (%dx%d, spp=%d, depth=%d)...\n",
		cam->image_width, h, cam->sqrt_spp * cam->sqrt_spp,
		cam->max_depth);
#pragma omp parallel for schedule(dynamic, 1)
	for (int j = 0; j < h; ++j)
	{
		render_pixel_row(cam, w, pixels, j);
		{
			int done;
#pragma omp atomic capture
			done = ++rows_done;
			if ((done & 7) == 0 || done == h)
			{
				double elapsed = (double)(clock() - start)
					/ (double)CLOCKS_PER_SEC;
				double eta = (done > 0)
					? elapsed / (double)done * (double)(h - done) : 0.0;
#pragma omp critical
				{
					format_time(elapsed, buf, sizeof(buf));
					fprintf(stderr, "\rRendering: %5.1f%% | "
						"rows left: %4d | elapsed: %s | ETA: ",
						100.0 * (double)done / (double)h,
						h - done, buf);
					format_time(eta, buf, sizeof(buf));
					fprintf(stderr, "%s ", buf);
				}
			}
		}
	}
	fprintf(stderr, "\nRender complete.\n");
}

static unsigned char	*pixels_to_rgb(const t_camera *cam,
							const t_vec3 *pixels)
{
	int				total;
	unsigned char	*rgb;
	unsigned char	*ptr;
	int				i;

	total = cam->image_width * cam->image_height;
	rgb = (unsigned char *)malloc((size_t)total * 3);
	if (!rgb)
		return (NULL);
	ptr = rgb;
	i = 0;
	while (i < total)
	{
		ptr = write_color_to_buf_bin(ptr, &pixels[i]);
		i++;
	}
	return (rgb);
}

/* ------------------------------------------------------------------ */
/*  SDL2 / OpenGL viewer                                              */
/* ------------------------------------------------------------------ */

static void	sdl_display(const unsigned char *rgb, int w, int h)
{
	SDL_Window		*win;
	SDL_GLContext	gl_ctx;
	GLuint			tex;
	SDL_Event		ev;
	int				running;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return ;
	}
	win = SDL_CreateWindow("Spotlight Scene — Ray Tracer",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!win)
	{
		fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
		SDL_Quit();
		return ;
	}
	gl_ctx = SDL_GL_CreateContext(win);
	if (!gl_ctx)
	{
		fprintf(stderr, "GL context failed: %s\n", SDL_GetError());
		SDL_DestroyWindow(win);
		SDL_Quit();
		return ;
	}
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0,
		GL_RGB, GL_UNSIGNED_BYTE, rgb);
	running = 1;
	while (running)
	{
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
				running = 0;
			if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
				running = 0;
			if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_q)
				running = 0;
		}
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-1.0f, -1.0f);
		glEnd();
		SDL_GL_SwapWindow(win);
		SDL_Delay(16);
	}
	glDeleteTextures(1, &tex);
	SDL_GL_DeleteContext(gl_ctx);
	SDL_DestroyWindow(win);
	SDL_Quit();
}

/* ------------------------------------------------------------------ */
/*  Scene construction & main                                         */
/* ------------------------------------------------------------------ */

static void	build_scene(t_hittable_list *world, const t_obj_opts *o)
{
	add_ground(world);
	add_spotlights(world);
	add_obj_model(world, o);
}

static void	setup_camera(t_camera *cam)
{
	t_vec3	fv;

	cam->aspect_ratio = 16.0 / 9.0;
	cam->image_width = 800;
	cam->samples_per_pixel = 400;
	cam->max_depth = 50;
	cam->background = vec3_create(0.02, 0.02, 0.04);
	cam->vfov = 35.0;
	cam->lookfrom = point3_create(0.0, 5.0, -14.0);
	cam->lookat = point3_create(0.0, 2.0, 0.0);
	cam->vup = vec3_create(0.0, 1.0, 0.0);
	cam->defocus_angle = 0.0;
	fv = vec3_sub(&cam->lookfrom, &cam->lookat);
	cam->focus_dist = vec3_length(&fv);
	camera_init(cam, cam->aspect_ratio, cam->image_width);
}

int	main(int ac, char **av)
{
	t_hittable_list	world;
	t_bvh_node		*bvh;
	t_hittable_list	accel;
	t_camera		cam;
	t_vec3			*pixels;
	unsigned char	*rgb;
	t_obj_opts		opts;

	setvbuf(stderr, NULL, _IONBF, 0);
	parse_opts(&opts, ac, av);
	hittable_list_init(&world);
	build_scene(&world, &opts);
	fprintf(stderr, "Building BVH...\n");
	bvh = bvh_node_create(&world);
	hittable_list_init(&accel);
	if (bvh)
	{
		t_hittable_wrapper bw = {.object = bvh, .owned = true,
			.set_current = set_current_bvh,
			.hit_noobj = bvh_node_hit, .bbox = bvh->bbox};
		hittable_list_add_wrapper(&accel, &bw);
	}
	setup_camera(&cam);
	pixels = (t_vec3 *)malloc((size_t)cam.image_width
			* (size_t)cam.image_height * sizeof(t_vec3));
	if (!pixels)
		return (1);
	render_to_buffer(&cam, bvh ? &accel : &world, pixels);
	rgb = pixels_to_rgb(&cam, pixels);
	free(pixels);
	if (!rgb)
		return (1);
	fprintf(stderr, "Launching SDL2/OpenGL viewer...\n");
	sdl_display(rgb, cam.image_width, cam.image_height);
	free(rgb);
	hittable_list_clear(&accel);
	hittable_list_clear(&world);
	return (0);
}
