/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_rt.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:44:19 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_rt.h"
#include "studio_config.h"
#include <stdio.h>

static t_triangle	wf_make_tri(const t_wf_triangle *wt, t_material *mat)
{
	t_point3	v[3];
	t_vec3		n[3];
	int			i;

	i = -1;
	while (++i < 3)
	{
		v[i] = (t_point3){wt->v[i].x, wt->v[i].y, wt->v[i].z};
		n[i] = (t_vec3){wt->n[i].x, wt->n[i].y, wt->n[i].z};
	}
	if (RT_OBJ_SMOOTH_NORMALS && wt->has_normals)
		return (triangle_create_smooth(&v[0], &v[1], &v[2],
				&n[0], &n[1], &n[2], mat));
	return (triangle_create(&v[0], &v[1], &v[2], mat));
}

bool	wf_mesh_to_rt_mesh(const t_wf_mesh *wf, t_mesh *rt,
		t_material *mat)
{
	size_t		i;
	t_triangle	tri;

	mesh_init(rt);
	i = 0;
	while (i < wf->ntris)
	{
		tri = wf_make_tri(&wf->tris[i], mat);
		if (!mesh_add_triangle(rt, &tri))
			return (false);
		i++;
	}
	return (true);
}

bool	wf_obj_to_rt_mesh(const char *path, t_mesh *mesh,
		t_material *mat, float target_size)
{
	t_wf_model	model;
	t_wf_mesh	wfmesh;
	bool		ok;

	wf_model_init(&model);
	wf_mesh_init(&wfmesh);
	ok = wf_parse_file(&model, path);
	if (ok)
	{
		ok = wf_triangulate(&wfmesh, &model);
		if (ok)
		{
			wf_mesh_center(&wfmesh);
			wf_mesh_normalize(&wfmesh, target_size);
			ok = wf_mesh_to_rt_mesh(&wfmesh, mesh, mat);
		}
	}
	wf_mesh_free(&wfmesh);
	wf_model_free(&model);
	return (ok);
}

bool	wf_obj_to_rt_mesh_at(const char *path, t_mesh *mesh,
		t_material *mat, float target_size,
		float px, float py, float pz)
{
	t_wf_model	model;
	t_wf_mesh	wfmesh;
	bool		ok;

	wf_model_init(&model);
	wf_mesh_init(&wfmesh);
	ok = wf_parse_file(&model, path);
	if (ok)
	{
		wf_model_print_info(&model);
		ok = wf_triangulate(&wfmesh, &model);
		if (ok)
		{
			wf_mesh_center(&wfmesh);
			wf_mesh_normalize(&wfmesh, target_size);
			wf_mesh_translate(&wfmesh, px, py, pz);
			wf_mesh_print_info(&wfmesh);
			ok = wf_mesh_to_rt_mesh(&wfmesh, mesh, mat);
		}
	}
	wf_mesh_free(&wfmesh);
	wf_model_free(&model);
	return (ok);
}

bool	wf_obj_to_hittable_list(const char *path, t_hittable_list *list,
		t_material *mat, float target_size,
		float px, float py, float pz)
{
	t_mesh	rt_mesh = {0};
	bool	ok;

	ok = wf_obj_to_rt_mesh_at(path, &rt_mesh, mat, target_size,
			px, py, pz);
	if (ok)
		ok = mesh_add_to_list(&rt_mesh, list);
	mesh_clear(&rt_mesh);
	return (ok);
}
