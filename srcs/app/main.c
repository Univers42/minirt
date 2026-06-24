/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 15:22:36 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/10 13:47:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "json_scene.h"
#include "obj_parser.h"
#include "studio_config.h"
#include "rt_mlx.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "bvh.h"
#include "bvh_flat.h"
#include "mesh_accel.h"
#include "shading.h"
#include "lode_image.h"
#include "mlx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* Declarations from scene_build.c / scene_build3.c / render.c */
void			setup_camera(t_camera *cam, const t_scene *sc, int width);
int				rt_render_width(void);
bool			add_scene_lights(t_hittable_list *world, const t_scene *sc);
unsigned char	*render_to_buffer(const t_camera *cam,
					const t_hittable_list *world);

/* Global flags: dump PPM / PNG instead of opening the MLX window. */
static int	g_ppm_mode;
static int	g_png_mode;

/* Output path for --png: $RT_PNG_OUT if set, else render.png. */
static const char	*png_out_path(void)
{
	const char	*p;

	p = getenv("RT_PNG_OUT");
	if (p && *p)
		return (p);
	return ("render.png");
}

static const char	*get_ext(const char *path)
{
	const char	*dot;

	dot = strrchr(path, '.');
	if (!dot || dot == path)
		return ("");
	return (dot);
}

/* ------------------------------------------------------------------ */
/*  PPM output: dump the render buffer to a file                      */
/* ------------------------------------------------------------------ */

/* Output path for --ppm: $RT_PPM_OUT if set, else render.ppm.        */
/* Lets parallel headless renders write to distinct files.            */
static const char	*ppm_out_path(void)
{
	const char	*p;

	p = getenv("RT_PPM_OUT");
	if (p && *p)
		return (p);
	return ("render.ppm");
}

static int	save_ppm(const unsigned char *buf, int w, int h, const char *path)
{
	FILE	*fp;

	fp = fopen(path, "wb");
	if (!fp)
	{
		fprintf(stderr, "Error\nCannot open %s for writing\n", path);
		return (1);
	}
	fprintf(fp, "P6\n%d %d\n255\n", w, h);
	fwrite(buf, 1, (size_t)w * (size_t)h * 3, fp);
	fclose(fp);
	fprintf(stderr, "PPM saved to %s (%dx%d)\n", path, w, h);
	return (0);
}

/* ------------------------------------------------------------------ */
/*  Shared: BVH → render → MLX display                               */
/* ------------------------------------------------------------------ */

static void	wrap_bvh(t_hittable_list *accel, t_bvh_node *bvh)
{
	t_hittable_wrapper	w;

	w.object = bvh;
	w.owned = false;
	w.set_current = set_current_bvh;
	w.hit_noobj = bvh_node_hit;
	w.bbox = bvh->bbox;
	hittable_list_add_wrapper(accel, &w);
}

/* Build wrapper + flat BVH over scene->world into accel; returns flat bvh. */
static t_flat_bvh	*display_accel(t_hittable_list *accel, t_scene *scene,
						t_bvh_node **bvh)
{
	*bvh = bvh_node_create(&scene->world);
	hittable_list_init(accel);
	if (*bvh)
		wrap_bvh(accel, *bvh);
	return (accel_attach_fast(accel, &scene->world));
}

static void	display_free(t_hittable_list *accel, t_bvh_node *bvh,
				t_flat_bvh *fbvh)
{
	hittable_list_clear(accel);
	flat_bvh_free(fbvh);
	bvh_node_destroy(bvh);
}

/* Open the window and pump the event loop; frees buf, returns 0 on success. */
static int	display_window(t_camera *cam, unsigned char *buf)
{
	t_mlx_ctx	ctx;

	if (mlx_ctx_init(&ctx, cam->image_width, cam->image_height, "rt") < 0)
		return (free(buf), fprintf(stderr, "Error\nDisplay init failed\n"), 1);
	mlx_display_rgb(&ctx, buf);
	free(buf);
	mlx_expose_hook(ctx.win, (int (*)())mlx_expose_handler, &ctx);
	mlx_hook(ctx.win, X_KEY_PRESS, KEY_MASK_PRESS,
		(t_mlx_hcb){mlx_key_handler, &ctx});
	mlx_hook(ctx.win, X_DESTROY_NOTIFY, 0,
		(t_mlx_hcb){mlx_close_handler, &ctx});
	mlx_loop(ctx.mlx);
	mlx_ctx_destroy(&ctx);
	return (0);
}

static int	display_ppm(t_camera *cam, unsigned char *buf)
{
	int	ret;

	ret = save_ppm(buf, cam->image_width, cam->image_height, ppm_out_path());
	free(buf);
	return (ret);
}

static int	display_png(t_camera *cam, unsigned char *buf)
{
	unsigned int	err;
	const char		*path;

	path = png_out_path();
	err = lode_image_save_png(path, buf, cam->image_width, cam->image_height);
	free(buf);
	if (err)
		return (fprintf(stderr, "Error\nPNG write failed (%u)\n", err), 1);
	fprintf(stderr, "PNG saved to %s (%dx%d)\n", path,
		cam->image_width, cam->image_height);
	return (0);
}

static int	display_scene(t_scene *scene)
{
	t_camera		cam;
	t_bvh_node		*bvh;
	t_flat_bvh		*fbvh;
	t_hittable_list	accel;
	unsigned char	*buf;

	setup_camera(&cam, scene, rt_render_width());
	fbvh = display_accel(&accel, scene, &bvh);
	buf = render_to_buffer(&cam, (bvh || fbvh) ? &accel : &scene->world);
	if (!buf)
		return (display_free(&accel, bvh, fbvh),
			fprintf(stderr, "Error\nRender failed\n"), 1);
	if (g_png_mode)
		return (display_free(&accel, bvh, fbvh), display_png(&cam, buf));
	if (g_ppm_mode)
		return (display_free(&accel, bvh, fbvh), display_ppm(&cam, buf));
	display_window(&cam, buf);
	display_free(&accel, bvh, fbvh);
	return (0);
}

/* ------------------------------------------------------------------ */
/*  RT pipeline via display_scene (used for --ppm mode)               */
/* ------------------------------------------------------------------ */

static int	run_rt_display(const char *filepath)
{
	t_scene	scene;
	int		ret;

	if (!parse_scene(filepath, &scene))
		return (1);
	if (!build_scene_objects(&scene))
	{
		fprintf(stderr, "Error\nFailed to build RT scene objects\n");
		scene_cleanup(&scene);
		return (1);
	}
	if (!add_scene_lights(&scene.world, &scene))
	{
		scene_cleanup(&scene);
		return (1);
	}
	ret = display_scene(&scene);
	scene_cleanup(&scene);
	return (ret);
}

/* ------------------------------------------------------------------ */
/*  JSON pipeline                                                     */
/* ------------------------------------------------------------------ */

static int	run_json(const char *filepath)
{
	t_scene	scene;
	int		ret;

	if (!json_parse_scene(filepath, &scene))
		return (1);
	if (!build_scene_objects(&scene))
	{
		fprintf(stderr, "Error\nFailed to build JSON scene objects\n");
		scene_cleanup(&scene);
		return (1);
	}
	if (!add_scene_lights(&scene.world, &scene))
	{
		scene_cleanup(&scene);
		return (1);
	}
	ret = display_scene(&scene);
	scene_cleanup(&scene);
	return (ret);
}

/* ------------------------------------------------------------------ */
/*  OBJ pipeline: loads a mesh, wraps it in a default scene           */
/* ------------------------------------------------------------------ */

static void	obj_default_camera(t_scene *sc)
{
	sc->ambient.color = vec3_create(RT_DEFAULT_AMBIENT,
			RT_DEFAULT_AMBIENT, RT_DEFAULT_AMBIENT);
	sc->ambient.ratio = 1.0;
	sc->camera.pos = vec3_create(0, RT_OBJ_CAM_HEIGHT, RT_OBJ_CAM_DIST);
	sc->camera.dir = vec3_create(0, -0.2, -1.0);
	sc->camera.fov = RT_OBJ_CAM_FOV;
	sc->light_count = 1;
	sc->lights[0].pos = vec3_create(5.0, 8.0, 5.0);
	sc->lights[0].color = vec3_create(1.0, 1.0, 1.0);
	sc->lights[0].brightness = 1.0;
}

static t_material	*obj_make_material(void)
{
	t_color	clr;
	t_color	tint;

	clr = vec3_create(RT_OBJ_COLOR_R, RT_OBJ_COLOR_G, RT_OBJ_COLOR_B);
	if (RT_OBJ_MAT_TYPE == 1)
		return (metal_create_fuzz(clr, RT_OBJ_MAT_FUZZ));
	if (RT_OBJ_MAT_TYPE == 2)
		return (glossy_create(clr, RT_OBJ_MAT_ROUGHNESS, RT_OBJ_MAT_FUZZ));
	if (RT_OBJ_MAT_TYPE == 3)
		return (dielectric_create(RT_OBJ_MAT_REFRACTION));
	if (RT_OBJ_MAT_TYPE == 4)
	{
		tint = vec3_create(RT_OBJ_MAT_TINT_R,
				RT_OBJ_MAT_TINT_G, RT_OBJ_MAT_TINT_B);
		return (tinted_glass_create(RT_OBJ_MAT_REFRACTION, tint));
	}
	if (RT_OBJ_MAT_TYPE == 5)
		return (diffuse_light_create(vec3_create(RT_OBJ_MAT_LIGHT_INTENSITY,
					RT_OBJ_MAT_LIGHT_INTENSITY,
					RT_OBJ_MAT_LIGHT_INTENSITY)));
	if (RT_OBJ_TEXTURE_TYPE == 1)
	{
		return (lambertian_create_texture(checker_texture_create(
					RT_OBJ_CHECKER_SCALE,
					solid_color_create(clr),
					solid_color_create(vec3_create(0.1, 0.1, 0.1)))));
	}
	return (lambertian_create(clr));
}

/* Parse the OBJ into one accelerated mesh primitive (contiguous triangle
   soup + mesh-local flat BVH) and register it in the scene as a SINGLE
   wrapper. Returns the accel (caller frees with mesh_accel_free) or NULL. */
static t_mesh_accel	*obj_load_accel(const char *path, t_scene *scene,
						t_material *mat)
{
	t_mesh				mesh;
	t_mesh_accel		*ma;
	t_hittable_wrapper	w;

	if (!obj_parse_to_mesh_at(path, &mesh, mat, RT_OBJ_TARGET_SIZE,
			0.0f, 0.0f, 0.0f))
		return (fprintf(stderr, "Error\nFailed to load OBJ: %s\n", path), NULL);
	fprintf(stderr, "OBJ loaded: %zu triangles\n", mesh.count);
	ma = mesh_accel_build(&mesh);
	mesh_clear(&mesh);
	if (!ma)
		return (NULL);
	w = mesh_accel_wrapper(ma);
	if (!hittable_list_add_wrapper(&scene->world, &w))
		return (mesh_accel_free(ma), NULL);
	return (ma);
}

static int	run_obj(const char *filepath)
{
	t_scene			scene;
	t_material		*mat;
	t_mesh_accel	*ma;
	int				ret;

	scene_init(&scene);
	obj_default_camera(&scene);
	mat = obj_make_material();
	if (!mat)
		return (fprintf(stderr, "Error\nMaterial alloc failed\n"), 1);
	mat_registry_add(mat);
	ma = obj_load_accel(filepath, &scene, mat);
	if (!ma || !add_scene_lights(&scene.world, &scene))
		return (mesh_accel_free(ma), scene_cleanup(&scene), 1);
	ret = display_scene(&scene);
	mesh_accel_free(ma);
	scene_cleanup(&scene);
	return (ret);
}

/* ------------------------------------------------------------------ */
/*  Dispatch                                                          */
/* ------------------------------------------------------------------ */

static void	usage(const char *prog)
{
	fprintf(stderr,
		"Usage: %s [--ppm] [--cinematic] "
		"<scene.rt | scene.json | scene.obj>\n"
		"  --ppm        Save render to render.ppm instead of MLX window\n"
		"  --cinematic  Use the Monte-Carlo path tracer (slow, photoreal)\n"
		"               instead of the default fast deterministic engine\n",
		prog);
}

/* Scan argv for flags (--ppm, --cinematic) and the single scene path.
   Returns the scene path, or NULL on a malformed command line. */
static const char	*parse_args(int argc, char **argv)
{
	const char	*path;
	int			i;

	path = NULL;
	i = 1;
	while (i < argc)
	{
		if (strcmp(argv[i], "--ppm") == 0)
			g_ppm_mode = 1;
		else if (strcmp(argv[i], "--png") == 0)
			g_png_mode = 1;
		else if (strcmp(argv[i], "--cinematic") == 0)
			render_set_engine_mode(ENGINE_CINEMATIC);
		else if (argv[i][0] == '-' || path)
			return (NULL);
		else
			path = argv[i];
		i++;
	}
	return (path);
}

static int	dispatch(const char *path)
{
	const char	*ext;

	ext = get_ext(path);
	if (strcasecmp(ext, ".rt") == 0)
	{
		if (g_ppm_mode || g_png_mode)
			return (run_rt_display(path));
		return (rt_run(path));
	}
	if (strcasecmp(ext, ".json") == 0)
		return (run_json(path));
	if (strcasecmp(ext, ".obj") == 0)
		return (run_obj(path));
	fprintf(stderr, "Error\nUnsupported file extension: %s\n", ext);
	return (1);
}

int	main(int argc, char **argv)
{
	const char	*scene_path;

	g_ppm_mode = 0;
	render_set_engine_mode(ENGINE_DIRECT);
	scene_path = parse_args(argc, argv);
	if (!scene_path)
	{
		usage(argv[0]);
		return (1);
	}
	return (dispatch(scene_path));
}
