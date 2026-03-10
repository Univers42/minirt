/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_parser.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:55:47 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_PARSER_H
# define RT_PARSER_H

# include "types.h"
# include "vector.h"
# include "point.h"
# include "hittable_list.h"
# include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  Maximum limits                                                    */
/* ------------------------------------------------------------------ */

# include "studio_config.h"

# define RT_MAX_LIGHTS		16
# define RT_MAX_OBJECTS		256

/* ------------------------------------------------------------------ */
/*  Parsed element: ambient light                                     */
/* ------------------------------------------------------------------ */

typedef struct s_rt_ambient
{
	double		ratio;
	t_color		color;
}	t_rt_ambient;

/* ------------------------------------------------------------------ */
/*  Parsed element: camera                                            */
/* ------------------------------------------------------------------ */

typedef struct s_rt_camera
{
	t_point3	pos;
	t_vec3		dir;
	double		fov;
	double		defocus_angle;
	double		focus_dist;
	t_vec3		vup;
	bool		has_defocus;
	bool		has_focus_dist;
	bool		has_vup;
}	t_rt_camera;

/* ------------------------------------------------------------------ */
/*  Parsed element: light                                             */
/* ------------------------------------------------------------------ */

typedef struct s_rt_light
{
	t_point3	pos;
	double		brightness;
	t_color		color;
}	t_rt_light;

/* ------------------------------------------------------------------ */
/*  Raw parsed objects (validated, before engine conversion)           */
/* ------------------------------------------------------------------ */

typedef enum e_obj_type
{
	OBJ_SPHERE,
	OBJ_PLANE,
	OBJ_CYLINDER,
	OBJ_CONE,
	OBJ_QUAD,
	OBJ_MESH,
	OBJ_TRIANGLE,
	OBJ_NONE
}	t_obj_type;

typedef enum e_mat_type
{
	MAT_LAMBERTIAN,
	MAT_METAL,
	MAT_DIELECTRIC,
	MAT_EMISSIVE,
	MAT_CHECKER,
	MAT_GLOSSY,
	MAT_TINTED_GLASS,
	MAT_ISOTROPIC
}	t_mat_type;

typedef struct s_rt_sphere
{
	t_point3	center;
	double		diameter;
	t_color		color;
}	t_rt_sphere;

typedef struct s_rt_plane
{
	t_point3	point;
	t_vec3		normal;
	t_color		color;
}	t_rt_plane;

typedef struct s_rt_cylinder
{
	t_point3	center;
	t_vec3		axis;
	double		diameter;
	double		height;
	t_color		color;
}	t_rt_cylinder;

typedef struct s_rt_cone
{
	t_point3	apex;
	t_vec3		axis;
	double		diameter;
	double		height;
	t_color		color;
}	t_rt_cone;

typedef struct s_rt_quad
{
	t_point3	position;
	t_vec3		u;
	t_vec3		v;
	t_color		color;
}	t_rt_quad;

typedef struct s_rt_mesh
{
	char		path[256];
	t_point3	position;
	double		scale;
	t_color		color;
}	t_rt_mesh;

typedef struct s_rt_triangle
{
	t_point3	v0;
	t_point3	v1;
	t_point3	v2;
	t_color		color;
}	t_rt_triangle;

typedef struct s_mat_spec
{
	t_mat_type	type;
	double		fuzz;
	double		ior;
	double		scale;
	double		roughness;
	t_color		color2;
}	t_mat_spec;

typedef union u_obj_data
{
	t_rt_sphere		sphere;
	t_rt_plane		plane;
	t_rt_cylinder	cylinder;
	t_rt_cone		cone;
	t_rt_quad		quad;
	t_rt_mesh		mesh;
	t_rt_triangle	triangle;
}	t_obj_data;

typedef struct s_rt_object
{
	t_obj_type	type;
	t_obj_data	data;
	t_mat_spec	mat;
}	t_rt_object;

/* ------------------------------------------------------------------ */
/*  Render quality overrides (populated from JSON "render" block)     */
/* ------------------------------------------------------------------ */

typedef struct s_render_opts
{
	int		width;
	int		height;
	int		spp;
	int		max_depth;
	int		color_space;
	double	aspect_ratio;
	bool	has_width;
	bool	has_spp;
	bool	has_depth;
	bool	has_aspect;
	bool	has_color_space;
}	t_render_opts;

/* ------------------------------------------------------------------ */
/*  Scene: everything the renderer needs                              */
/* ------------------------------------------------------------------ */

typedef struct s_scene
{
	t_rt_ambient	ambient;
	t_rt_camera		camera;
	t_rt_light		lights[RT_MAX_LIGHTS];
	int				light_count;
	t_rt_object		objects[RT_MAX_OBJECTS];
	int				object_count;
	bool			has_ambient;
	bool			has_camera;
	t_hittable_list	world;
	t_render_opts	render;
}	t_scene;

/* ------------------------------------------------------------------ */
/*  Public API                                                        */
/* ------------------------------------------------------------------ */

bool	parse_scene(const char *filepath, t_scene *scene);
bool	build_scene_objects(t_scene *scene);
void	scene_init(t_scene *scene);
void	scene_cleanup(t_scene *scene);

/* Full RT pipeline: parse → build → BVH → render → MLX display */
int		rt_run(const char *filepath);

#endif
