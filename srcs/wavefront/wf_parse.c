/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_parse.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 21:52:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include "wf_internal.h"
#include <stdlib.h>
#include <string.h>

static bool	wf_parse_vertex_line(t_wf_model *m, const char *line)
{
	t_wf_vec3	v;
	const char	*p;

	p = line;
	v.x = wf_parse_float(&p);
	v.y = wf_parse_float(&p);
	v.z = wf_parse_float(&p);
	return (wf_model_add_vert(m, v));
}

static bool	wf_parse_normal_line(t_wf_model *m, const char *line)
{
	t_wf_vec3	n;
	const char	*p;

	p = line;
	n.x = wf_parse_float(&p);
	n.y = wf_parse_float(&p);
	n.z = wf_parse_float(&p);
	return (wf_model_add_norm(m, n));
}

static bool	wf_parse_texcoord_line(t_wf_model *m, const char *line)
{
	t_wf_vec2	tc;
	const char	*p;

	p = line;
	tc.u = wf_parse_float(&p);
	tc.v = wf_parse_float(&p);
	return (wf_model_add_texcoord(m, tc));
}

static bool	wf_parse_meta_line(t_wf_model *m, const char *kind,
		const char *rest)
{
	char	buf[256];

	if (strcmp(kind, "g") == 0 || strcmp(kind, "o") == 0)
	{
		rest = wf_skip_spaces(rest);
		wf_next_token(rest, buf, sizeof(buf));
		return (wf_model_add_group(m, buf));
	}
	if (strcmp(kind, "usemtl") == 0)
	{
		rest = wf_skip_spaces(rest);
		wf_next_token(rest, buf, sizeof(buf));
		wf_model_set_material(m, buf);
		return (true);
	}
	if (strcmp(kind, "mtllib") == 0)
	{
		rest = wf_skip_spaces(rest);
		wf_next_token(rest, m->mtllib, sizeof(m->mtllib));
		return (true);
	}
	return (true);
}

bool	wf_dispatch_line(t_wf_model *m, const char *line)
{
	char		kind[16];
	const char	*rest;

	rest = wf_next_token(line, kind, sizeof(kind));
	if (kind[0] == '#' || kind[0] == '\0')
		return (true);
	if (strcmp(kind, "v") == 0)
		return (wf_parse_vertex_line(m, rest));
	if (strcmp(kind, "vn") == 0)
		return (wf_parse_normal_line(m, rest));
	if (strcmp(kind, "vt") == 0)
		return (wf_parse_texcoord_line(m, rest));
	if (strcmp(kind, "f") == 0)
		return (wf_parse_face_line(m, rest));
	return (wf_parse_meta_line(m, kind, rest));
}
