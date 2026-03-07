/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_viewer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 22:08:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include <stdio.h>
#include <math.h>

/*
** GL interleaved vertex: normal (3f) + position (3f)
*/
typedef struct s_gl_vert
{
	float	nx;
	float	ny;
	float	nz;
	float	vx;
	float	vy;
	float	vz;
}	t_gl_vert;

typedef struct s_gl_data
{
	t_gl_vert	*verts;
	size_t		nverts;
	float		mid[3];
	float		sca;
}	t_gl_data;

static bool	wf_mesh_to_gl(const t_wf_mesh *mesh, t_gl_data *gl)
{
	size_t	i;
	int		j;

	gl->nverts = mesh->ntris * 3;
	gl->verts = (t_gl_vert *)malloc(gl->nverts * sizeof(t_gl_vert));
	if (!gl->verts)
		return (false);
	i = 0;
	while (i < mesh->ntris)
	{
		j = 0;
		while (j < 3)
		{
			gl->verts[i * 3 + j].nx = mesh->tris[i].n[j].x;
			gl->verts[i * 3 + j].ny = mesh->tris[i].n[j].y;
			gl->verts[i * 3 + j].nz = mesh->tris[i].n[j].z;
			gl->verts[i * 3 + j].vx = mesh->tris[i].v[j].x;
			gl->verts[i * 3 + j].vy = mesh->tris[i].v[j].y;
			gl->verts[i * 3 + j].vz = mesh->tris[i].v[j].z;
			j++;
		}
		i++;
	}
	gl->mid[0] = mesh->center.x;
	gl->mid[1] = mesh->center.y;
	gl->mid[2] = mesh->center.z;
	gl->sca = mesh->bounds_max.x - mesh->bounds_min.x;
	if (mesh->bounds_max.y - mesh->bounds_min.y > gl->sca)
		gl->sca = mesh->bounds_max.y - mesh->bounds_min.y;
	if (mesh->bounds_max.z - mesh->bounds_min.z > gl->sca)
		gl->sca = mesh->bounds_max.z - mesh->bounds_min.z;
	if (gl->sca < 1e-6f)
		gl->sca = 1.0f;
	return (true);
}

static void	gl_perspective(double fovy, double aspect, double zn, double zf)
{
	double	h;
	double	w;

	h = zn * tan(fovy / 360.0 * 3.141592653589793);
	w = h * aspect;
	glFrustum(-w, +w, -h, +h, zn, zf);
}

static void	gl_setup_lights(float sca, const float mid[3])
{
	float	tsec;
	float	orbit;
	float	lpos[4];

	tsec = (float)SDL_GetTicks64() / 1000.0f;
	orbit = sca * 2.2f;
	lpos[0] = mid[0] + cosf(tsec * 0.5f) * orbit;
	lpos[1] = mid[1] + sca * 0.8f + sinf(tsec * 0.3f) * sca * 0.4f;
	lpos[2] = mid[2] + sinf(tsec * 0.5f) * orbit;
	lpos[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,
		(GLfloat[]){1.0f, 0.95f, 0.8f, 1.0f});
	glLightfv(GL_LIGHT0, GL_SPECULAR,
		(GLfloat[]){1.0f, 1.0f, 0.95f, 1.0f});
	lpos[0] = mid[0] + cosf(tsec * 0.7f + 2.0f) * orbit * 0.8f;
	lpos[1] = mid[1] + sca * 0.5f;
	lpos[2] = mid[2] + sinf(tsec * 0.7f + 2.0f) * orbit * 0.8f;
	glLightfv(GL_LIGHT1, GL_POSITION, lpos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,
		(GLfloat[]){0.6f, 0.7f, 1.0f, 1.0f});
}

static void	gl_render_frame(const t_gl_data *gl, float zoom)
{
	float	rotate;

	glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -4.5f * zoom);
	glScalef(1.0f / gl->sca, 1.0f / gl->sca, 1.0f / gl->sca);
	glTranslatef(-gl->mid[0], -gl->mid[1], -gl->mid[2]);
	rotate = (float)(SDL_GetTicks64() % (40ULL * 360)) / 40.0f;
	glRotatef(rotate, 0.0f, 1.0f, 0.0f);
	gl_setup_lights(gl->sca, gl->mid);
	glColor3f(0.85f, 0.85f, 0.85f);
	glInterleavedArrays(GL_N3F_V3F, 0, gl->verts);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)gl->nverts);
}

static int	gl_handle_event(SDL_Event *e, float *zoom, int *width,
		int *height)
{
	const float	step = 1.1f;

	if (e->type == SDL_QUIT)
		return (0);
	if (e->type == SDL_KEYDOWN && e->key.keysym.sym == 'q')
		return (0);
	if (e->type == SDL_MOUSEWHEEL)
	{
		if (e->wheel.y > 0)
			*zoom *= step;
		else if (e->wheel.y < 0)
			*zoom /= step;
		if (*zoom < 0.1f)
			*zoom = 0.1f;
		if (*zoom > 10.0f)
			*zoom = 10.0f;
	}
	if (e->type == SDL_WINDOWEVENT
		&& e->window.event == SDL_WINDOWEVENT_RESIZED)
	{
		*width = e->window.data1;
		*height = e->window.data2;
		if (*height <= 0)
			*height = 1;
		glViewport(0, 0, *width, *height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gl_perspective(35, (double)*width / *height, 0.1, 20);
		glMatrixMode(GL_MODELVIEW);
	}
	return (1);
}

static int	gl_main_loop(SDL_Window *win, const t_gl_data *gl)
{
	float		zoom;
	int			w;
	int			h;
	SDL_Event	ev;

	zoom = 1.0f;
	w = 2000;
	h = 1300;
	while (1)
	{
		while (SDL_PollEvent(&ev))
		{
			if (!gl_handle_event(&ev, &zoom, &w, &h))
				return (0);
		}
		gl_render_frame(gl, zoom);
		SDL_GL_SwapWindow(win);
	}
	return (0);
}

static int	gl_init_window(SDL_Window **win, int w, int h)
{
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("SDL_Init(): %s", SDL_GetError());
		return (1);
	}
	*win = SDL_CreateWindow("WF Viewer", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, w, h,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!*win)
	{
		SDL_Log("SDL_CreateWindow(): %s", SDL_GetError());
		return (1);
	}
	if (!SDL_GL_CreateContext(*win))
	{
		SDL_Log("SDL_GL_CreateContext(): %s", SDL_GetError());
		return (1);
	}
	SDL_GL_SetSwapInterval(1);
	return (0);
}

static void	gl_setup(int width, int height)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,
		(GLfloat[]){0.30f, 0.28f, 0.25f, 1.0f});
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,
		(GLfloat[]){0.60f, 0.55f, 0.50f, 1.0f});
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
		(GLfloat[]){0.20f, 0.20f, 0.20f, 1.0f});
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	gl_perspective(35, (double)width / height, 0.1, 20);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_MODELVIEW);
}

int	main(int argc, char **argv)
{
	t_wf_model		model;
	t_wf_mesh		mesh;
	t_gl_data		gl;
	SDL_Window		*win;
	const char		*path;

	path = (argc > 1) ? argv[1] : "input/suzanne.obj";
	wf_model_init(&model);
	wf_mesh_init(&mesh);
	if (!wf_parse_file(&model, path))
	{
		fprintf(stderr, "Error: cannot load '%s'\n", path);
		return (1);
	}
	wf_model_print_info(&model);
	if (!wf_triangulate(&mesh, &model))
		return (1);
	wf_mesh_print_info(&mesh);
	if (!wf_mesh_to_gl(&mesh, &gl))
		return (1);
	wf_mesh_free(&mesh);
	wf_model_free(&model);
	if (gl_init_window(&win, 2000, 1300))
		return (1);
	gl_setup(2000, 1300);
	fprintf(stderr, "Controls: Mouse Wheel = Zoom, Q = Quit\n");
	return (gl_main_loop(win, &gl));
}
