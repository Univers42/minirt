/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_elements.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:40:43 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "rt_lexer.h"
#include "rt_error.h"

/* ------------------------------------------------------------------ */
/*  Validation helpers (declared in validate.c)                       */
/* ------------------------------------------------------------------ */

bool	validate_range_f(const t_file_buf *fb, const t_token *tok,
				double lo, double hi, const char *name);
bool	validate_color(const t_file_buf *fb, const t_token *tok);
bool	validate_normalized(const t_file_buf *fb, const t_token *tok,
				const char *name);
bool	validate_positive(const t_file_buf *fb, const t_token *tok,
				const char *name);

/* ------------------------------------------------------------------ */
/*  Convert a color token (0-255 ints) to engine color (0.0-1.0)     */
/* ------------------------------------------------------------------ */

static t_color	color_from_tok(const t_token *tok)
{
	return (vec3_create(
			(double)tok->val.col.r / 255.0,
			(double)tok->val.col.g / 255.0,
			(double)tok->val.col.b / 255.0));
}

static t_vec3	vec3_from_tok(const t_token *tok)
{
	return (vec3_create(tok->val.vec.x, tok->val.vec.y, tok->val.vec.z));
}

/* ------------------------------------------------------------------ */
/*  A – Ambient lighting                                              */
/*  Schema: A <ratio:F> <color:C>                                     */
/*  Tokens: [0]=ID  [1]=ratio  [2]=color                              */
/* ------------------------------------------------------------------ */

bool	parse_ambient(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	if (sc->has_ambient)
	{
		rt_error(fb, lex->line_num, lex->tokens[0].col_start,
			lex->tokens[0].col_end,
			"duplicate ambient light declaration (A)");
		return (false);
	}
	if (!validate_range_f(fb, &lex->tokens[1], 0.0, 1.0,
			"ambient lighting ratio"))
		return (false);
	if (!validate_color(fb, &lex->tokens[2]))
		return (false);
	sc->ambient.ratio = lex->tokens[1].val.f;
	sc->ambient.color = color_from_tok(&lex->tokens[2]);
	sc->has_ambient = true;
	return (true);
}

/* ------------------------------------------------------------------ */
/*  C – Camera                                                        */
/*  Schema: C <pos:V> <dir:V> <fov:F>                                 */
/*  Tokens: [0]=ID  [1]=pos  [2]=dir  [3]=fov                         */
/* ------------------------------------------------------------------ */

bool	parse_camera(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	if (sc->has_camera)
	{
		rt_error(fb, lex->line_num, lex->tokens[0].col_start,
			lex->tokens[0].col_end,
			"duplicate camera declaration (C)");
		return (false);
	}
	if (!validate_normalized(fb, &lex->tokens[2], "camera direction"))
		return (false);
	if (!validate_range_f(fb, &lex->tokens[3], 0.0, 180.0,
			"camera FOV"))
		return (false);
	sc->camera.pos = vec3_from_tok(&lex->tokens[1]);
	sc->camera.dir = vec3_from_tok(&lex->tokens[2]);
	sc->camera.fov = lex->tokens[3].val.f;
	sc->has_camera = true;
	return (true);
}

/* ------------------------------------------------------------------ */
/*  L – Light                                                         */
/*  Schema: L <pos:V> <brightness:F> <color:C>                        */
/*  Tokens: [0]=ID  [1]=pos  [2]=brightness  [3]=color                */
/* ------------------------------------------------------------------ */

bool	parse_light(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	t_rt_light	*light;

	if (sc->light_count >= RT_MAX_LIGHTS)
	{
		rt_error(fb, lex->line_num, lex->tokens[0].col_start,
			lex->tokens[0].col_end,
			"too many lights (max %d)", RT_MAX_LIGHTS);
		return (false);
	}
	if (!validate_range_f(fb, &lex->tokens[2], 0.0, 1.0,
			"light brightness"))
		return (false);
	if (!validate_color(fb, &lex->tokens[3]))
		return (false);
	light = &sc->lights[sc->light_count];
	light->pos = vec3_from_tok(&lex->tokens[1]);
	light->brightness = lex->tokens[2].val.f;
	light->color = color_from_tok(&lex->tokens[3]);
	sc->light_count++;
	return (true);
}
