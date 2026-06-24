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
#include "libft.h"

void	rt_parse_texture_kw(const char *line, t_mat_spec *mat);

/*
** Optional trailing material keyword on an object line, e.g.
**   sp 0,3,0 5 255,255,255 glass
** idx is the token index just after the colour.  Absent or unknown keyword
** leaves the zero-initialised default (lambertian).  Per-type numeric
** defaults (ior, roughness) are filled in by create_material().
** Keywords are matched against the lexer's 7-char identifier buffer.
** The raw line is then scanned for tex:/bump:/bumpstr: image tokens, which
** the limited alpha-only lexer cannot represent (paths carry '/', '.', ':').
*/
void	rt_parse_material_kw(t_lexer *lex, int idx, t_mat_spec *mat)
{
	const char	*kw;

	rt_parse_texture_kw(lex->line, mat);
	if (idx >= lex->count || lex->tokens[idx].type != TOK_IDENTIFIER)
		return ;
	kw = lex->tokens[idx].val.id;
	if (!ft_strncmp(kw, "glass", 8))
		mat->type = MAT_DIELECTRIC;
	else if (!ft_strncmp(kw, "metal", 8) || !ft_strncmp(kw, "mirror", 8))
		mat->type = MAT_METAL;
	else if (!ft_strncmp(kw, "glossy", 8))
		mat->type = MAT_GLOSSY;
	else if (!ft_strncmp(kw, "tglass", 8) || !ft_strncmp(kw, "tinted", 8))
		mat->type = MAT_TINTED_GLASS;
	else if (!ft_strncmp(kw, "light", 8) || !ft_strncmp(kw, "lamp", 8))
		mat->type = MAT_EMISSIVE;
	else if (!ft_strncmp(kw, "checker", 8))
		mat->type = MAT_CHECKER;
	else if (!ft_strncmp(kw, "iso", 8) || !ft_strncmp(kw, "fog", 8))
		mat->type = MAT_ISOTROPIC;
	else if (!ft_strncmp(kw, "marble", 8) || !ft_strncmp(kw, "noise", 8))
		mat->type = MAT_MARBLE;
	else if (!ft_strncmp(kw, "wood", 8))
		mat->type = MAT_WOOD;
}
