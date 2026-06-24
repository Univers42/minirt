/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_material.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 05:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 05:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "rt_lexer.h"
#include <string.h>

/*
** Optional trailing material keyword on an object line, e.g.
**   sp 0,3,0 5 255,255,255 glass
** idx is the token index just after the colour.  Absent or unknown keyword
** leaves the zero-initialised default (lambertian).  Per-type numeric
** defaults (ior, roughness) are filled in by create_material().
** Keywords are matched against the lexer's 7-char identifier buffer.
*/
void	rt_parse_material_kw(t_lexer *lex, int idx, t_mat_spec *mat)
{
	const char	*kw;

	if (idx >= lex->count || lex->tokens[idx].type != TOK_IDENTIFIER)
		return ;
	kw = lex->tokens[idx].val.id;
	if (!strncmp(kw, "glass", 8))
		mat->type = MAT_DIELECTRIC;
	else if (!strncmp(kw, "metal", 8) || !strncmp(kw, "mirror", 8))
		mat->type = MAT_METAL;
	else if (!strncmp(kw, "glossy", 8))
		mat->type = MAT_GLOSSY;
	else if (!strncmp(kw, "tglass", 8) || !strncmp(kw, "tinted", 8))
		mat->type = MAT_TINTED_GLASS;
	else if (!strncmp(kw, "light", 8) || !strncmp(kw, "lamp", 8))
		mat->type = MAT_EMISSIVE;
	else if (!strncmp(kw, "checker", 8))
		mat->type = MAT_CHECKER;
	else if (!strncmp(kw, "iso", 8) || !strncmp(kw, "fog", 8))
		mat->type = MAT_ISOTROPIC;
}
