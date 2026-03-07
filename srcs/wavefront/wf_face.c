/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_face.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 21:51:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_internal.h"
#include <string.h>

static void	wf_resolve_index(int32_t *idx, size_t count)
{
	if (*idx < 0)
		*idx = (int32_t)count + *idx + 1;
}

/*
** Parse one face vertex token: "v", "v/t", "v//n", or "v/t/n"
** Sets vi, ti, ni (0 means not present, OBJ indices are 1-based)
*/
static void	wf_parse_face_vert(const char *tok, t_wf_face_vert *fv,
		const t_wf_model *m)
{
	const char	*p;

	p = tok;
	fv->vi = wf_parse_int(&p);
	fv->ti = 0;
	fv->ni = 0;
	if (*p == '/')
	{
		p++;
		if (*p != '/')
			fv->ti = wf_parse_int(&p);
		if (*p == '/')
		{
			p++;
			fv->ni = wf_parse_int(&p);
		}
	}
	wf_resolve_index(&fv->vi, m->nverts);
	wf_resolve_index(&fv->ti, m->ntexcoords);
	wf_resolve_index(&fv->ni, m->nnorms);
}

bool	wf_parse_face_line(t_wf_model *m, const char *line)
{
	t_wf_face	face;
	char		tok[64];
	const char	*p;

	memset(&face, 0, sizeof(face));
	p = wf_skip_spaces(line);
	while (*p && *p != '\n' && *p != '\r' && face.nverts < 4)
	{
		p = wf_next_token(p, tok, sizeof(tok));
		if (tok[0] == '\0')
			break ;
		wf_parse_face_vert(tok, &face.verts[face.nverts], m);
		face.nverts++;
		p = wf_skip_spaces(p);
	}
	if (face.nverts < 3)
		return (true);
	return (wf_group_add_face(m, &face));
}
