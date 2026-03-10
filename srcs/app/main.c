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
#include "mlx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Declarations from scene_build.c / render.c */
void			setup_camera(t_camera *cam, const t_scene *sc, int width);
bool			add_scene_lights(t_hittable_list *world, const t_scene *sc);
unsigned char	*render_to_buffer(const t_camera *cam,
					const t_hittable_list *world);

/* Global flag: if true, dump PPM instead of opening MLX window */
static int	g_ppm_mode;

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
	w.owned = true;
	w.set_current = set_current_bvh;
	w.hit_noobj = bvh_node_hit;
	w.bbox = bvh->bbox;
	hittable_list_add_wrapper(accel, &w);
}

static int	display_scene(t_scene *scene)
{
	t_camera		cam;
	t_bvh_node		*bvh;
	t_hittable_list	accel;
	unsigned char	*buf;
	t_mlx_ctx		ctx;
	int				ret;

	setup_camera(&cam, scene, RT_IMAGE_WIDTH);
	bvh = bvh_node_create(&scene->world);
	hittable_list_init(&accel);
	if (bvh)
		wrap_bvh(&accel, bvh);
	buf = render_to_buffer(&cam, bvh ? &accel : &scene->world);
	if (!buf)
	{
		hittable_list_clear(&accel);
		return (fprintf(stderr, "Error\nRender failed\n"), 1);
	}
	if (g_ppm_mode)
	{
		ret = save_ppm(buf, cam.image_width, cam.image_height,
				"render.ppm");
		free(buf);
		hittable_list_clear(&accel);
		return (ret);
	}
	if (mlx_ctx_init(&ctx, cam.image_width, cam.image_height,
			"miniRT") < 0)
	{
		free(buf);
		hittable_list_clear(&accel);
		return (fprintf(stderr, "Error\nDisplay init failed\n"), 1);
	}
	mlx_display_rgb(&ctx, buf);
	free(buf);
	mlx_expose_hook(ctx.win, (int (*)())mlx_expose_handler, &ctx);
	mlx_hook(ctx.win, X_KEY_PRESS, KEY_MASK_PRESS,
		(t_mlx_hcb){mlx_key_handler, &ctx});
	mlx_hook(ctx.win, X_DESTROY_NOTIFY, 0,
		(t_mlx_hcb){mlx_close_handler, &ctx});
	mlx_loop(ctx.mlx);
	mlx_ctx_destroy(&ctx);
	hittable_list_clear(&accel);
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

static int	run_obj(const char *filepath)
{
	t_scene		scene;
	t_material	*mat;
	int			ret;

	scene_init(&scene);
	obj_default_camera(&scene);
	mat = obj_make_material();
	if (!mat)
		return (fprintf(stderr, "Error\nMaterial alloc failed\n"), 1);
	if (!obj_parse_to_list(filepath, &scene.world, mat,
			RT_OBJ_TARGET_SIZE, 0.0f, 0.0f, 0.0f))
	{
		fprintf(stderr, "Error\nFailed to load OBJ: %s\n", filepath);
		scene_cleanup(&scene);
		return (1);
	}
	fprintf(stderr, "OBJ loaded: %zu triangles\n",
		scene.world.count);
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
/*  Dispatch                                                          */
/* ------------------------------------------------------------------ */

static void	usage(const char *prog)
{
	fprintf(stderr,
		"Usage: %s [--ppm] <scene.rt | scene.json | scene.obj>\n"
		"  --ppm  Save render to render.ppm instead of MLX window\n",
		prog);
}

static int	dispatch(const char *path)
{
	const char	*ext;

	ext = get_ext(path);
	if (strcmp(ext, ".rt") == 0)
	{
		if (g_ppm_mode)
			return (run_rt_display(path));
		return (rt_run(path));
	}
	if (strcmp(ext, ".json") == 0)
		return (run_json(path));
	if (strcmp(ext, ".obj") == 0)
		return (run_obj(path));
	fprintf(stderr, "Error\nUnsupported file extension: %s\n", ext);
	return (1);
}

int	main(int argc, char **argv)
{
	const char	*scene_path;

	g_ppm_mode = 0;
	if (argc == 3 && strcmp(argv[1], "--ppm") == 0)
	{
		g_ppm_mode = 1;
		scene_path = argv[2];
	}
	else if (argc == 2)
		scene_path = argv[1];
	else
	{
		usage(argv[0]);
		return (1);
	}
	return (dispatch(scene_path));
}
