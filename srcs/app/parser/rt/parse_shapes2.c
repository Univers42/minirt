/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_shapes2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 06:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 06:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "rt_lexer.h"
#include "rt_error.h"
#include <string.h>

/* libft prototype (rt_error.h collides with libft.h's ft_stddef.h) */
void	*ft_memset(void *b, int c, size_t len);

void	rt_parse_material_kw(t_lexer *lex, int idx, t_mat_spec *mat);
bool	validate_color(const t_file_buf *fb, const t_token *tok);
bool	validate_normalized(const t_file_buf *fb, const t_token *tok,
			const char *name);
bool	validate_positive(const t_file_buf *fb, const t_token *tok,
			const char *name);

/* ------------------------------------------------------------------ */
/*  hy – Hyperboloid (one-sheet, cooling-tower)  (BONUS)               */
/*  Schema: hy <center:V> <axis:V> <diameter:F> <height:F> <color:C>  */
/*  Tokens: [0]=ID [1]=center [2]=axis [3]=diam [4]=height [5]=color  */
/* ------------------------------------------------------------------ */

static bool	hyperboloid_validate(t_file_buf *fb, t_lexer *lex)
{
	if (!validate_normalized(fb, &lex->tokens[2], "hyperboloid axis"))
		return (false);
	if (!validate_positive(fb, &lex->tokens[3], "hyperboloid diameter"))
		return (false);
	if (!validate_positive(fb, &lex->tokens[4], "hyperboloid height"))
		return (false);
	if (!validate_color(fb, &lex->tokens[5]))
		return (false);
	return (true);
}

bool	parse_hyperboloid(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	t_rt_object	obj;

	if (!hyperboloid_validate(fb, lex))
		return (false);
	if (sc->object_count >= RT_MAX_OBJECTS)
	{
		rt_error(&(t_err_loc){fb, lex->line_num,
				lex->tokens[0].col_start, lex->tokens[0].col_end},
			"too many objects (max %d)", RT_MAX_OBJECTS);
		return (false);
	}
	ft_memset(&obj, 0, sizeof(obj));
	obj.type = OBJ_HYPERBOLOID;
	obj.data.hyperboloid.center = vec3_create(lex->tokens[1].val.vec.x,
			lex->tokens[1].val.vec.y, lex->tokens[1].val.vec.z);
	obj.data.hyperboloid.axis = vec3_create(lex->tokens[2].val.vec.x,
			lex->tokens[2].val.vec.y, lex->tokens[2].val.vec.z);
	obj.data.hyperboloid.diameter = lex->tokens[3].val.f;
	obj.data.hyperboloid.height = lex->tokens[4].val.f;
	obj.data.hyperboloid.color = vec3_create(
			(double)lex->tokens[5].val.col.r / 255.0,
			(double)lex->tokens[5].val.col.g / 255.0,
			(double)lex->tokens[5].val.col.b / 255.0);
	rt_parse_material_kw(lex, 6, &obj.mat);
	sc->objects[sc->object_count] = obj;
	sc->object_count++;
	return (true);
}

/* ------------------------------------------------------------------ */
/*  to – Torus (donut, quartic)  (BONUS)                               */
/*  Schema: to <center:V> <axis:V> <major:F> <minor:F> <color:C>       */
/*  Tokens: [0]=ID [1]=center [2]=axis [3]=major [4]=minor [5]=color   */
/* ------------------------------------------------------------------ */

static bool	torus_validate(t_file_buf *fb, t_lexer *lex)
{
	if (!validate_normalized(fb, &lex->tokens[2], "torus axis"))
		return (false);
	if (!validate_positive(fb, &lex->tokens[3], "torus major radius"))
		return (false);
	if (!validate_positive(fb, &lex->tokens[4], "torus minor radius"))
		return (false);
	if (!validate_color(fb, &lex->tokens[5]))
		return (false);
	return (true);
}

bool	parse_torus(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	t_rt_object	obj;

	if (!torus_validate(fb, lex))
		return (false);
	if (sc->object_count >= RT_MAX_OBJECTS)
	{
		rt_error(&(t_err_loc){fb, lex->line_num,
				lex->tokens[0].col_start, lex->tokens[0].col_end},
			"too many objects (max %d)", RT_MAX_OBJECTS);
		return (false);
	}
	ft_memset(&obj, 0, sizeof(obj));
	obj.type = OBJ_TORUS;
	obj.data.torus.center = vec3_create(lex->tokens[1].val.vec.x,
			lex->tokens[1].val.vec.y, lex->tokens[1].val.vec.z);
	obj.data.torus.axis = vec3_create(lex->tokens[2].val.vec.x,
			lex->tokens[2].val.vec.y, lex->tokens[2].val.vec.z);
	obj.data.torus.major = lex->tokens[3].val.f;
	obj.data.torus.minor = lex->tokens[4].val.f;
	obj.data.torus.color = vec3_create(
			(double)lex->tokens[5].val.col.r / 255.0,
			(double)lex->tokens[5].val.col.g / 255.0,
			(double)lex->tokens[5].val.col.b / 255.0);
	rt_parse_material_kw(lex, 6, &obj.mat);
	sc->objects[sc->object_count] = obj;
	sc->object_count++;
	return (true);
}
