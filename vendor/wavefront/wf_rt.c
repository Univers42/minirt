/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_rt.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 22:08:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_rt.h"

bool	wf_mesh_to_rt_mesh(const t_wf_mesh *wf, t_mesh *rt,
		t_material *mat)
{
	size_t		i;
	t_point3	v0;
	t_point3	v1;
	t_point3	v2;
	t_triangle	tri;

	mesh_init(rt);
	i = 0;
	while (i < wf->ntris)
	{
		v0 = (t_point3){wf->tris[i].v[0].x,
			wf->tris[i].v[0].y,
			wf->tris[i].v[0].z};
		v1 = (t_point3){wf->tris[i].v[1].x,
			wf->tris[i].v[1].y,
			wf->tris[i].v[1].z};
		v2 = (t_point3){wf->tris[i].v[2].x,
			wf->tris[i].v[2].y,
			wf->tris[i].v[2].z};
		tri = triangle_create(&v0, &v1, &v2, mat);
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
		ok = wf_triangulate(&wfmesh, &model);
		if (ok)
		{
			wf_mesh_center(&wfmesh);
			wf_mesh_normalize(&wfmesh, target_size);
			wf_mesh_translate(&wfmesh, px, py, pz);
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
