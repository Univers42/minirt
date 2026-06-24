/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_shapes.c                                     :+:      :+:    :+:   */
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
/*  di – Disk  (BONUS)                                                */
/*  Schema: di <center:V> <normal:V> <diameter:F> <color:C>          */
/*  Tokens: [0]=ID [1]=center [2]=normal [3]=diameter [4]=color      */
/* ------------------------------------------------------------------ */

bool	parse_disk(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	t_rt_object	obj;

	if (!validate_normalized(fb, &lex->tokens[2], "disk normal"))
		return (false);
	if (!validate_positive(fb, &lex->tokens[3], "disk diameter"))
		return (false);
	if (!validate_color(fb, &lex->tokens[4]))
		return (false);
	if (sc->object_count >= RT_MAX_OBJECTS)
	{
		rt_error(&(t_err_loc){fb, lex->line_num,
				lex->tokens[0].col_start, lex->tokens[0].col_end},
			"too many objects (max %d)", RT_MAX_OBJECTS);
		return (false);
	}
	ft_memset(&obj, 0, sizeof(obj));
	obj.type = OBJ_DISK;
	obj.data.disk.center = vec3_from_tok(&lex->tokens[1]);
	obj.data.disk.normal = vec3_from_tok(&lex->tokens[2]);
	obj.data.disk.diameter = lex->tokens[3].val.f;
	obj.data.disk.color = color_from_tok(&lex->tokens[4]);
	rt_parse_material_kw(lex, 5, &obj.mat);
	sc->objects[sc->object_count] = obj;
	sc->object_count++;
	return (true);
}

/* ------------------------------------------------------------------ */
/*  pb – Paraboloid  (BONUS)                                          */
/*  Schema: pb <vertex:V> <axis:V> <diameter:F> <height:F> <color:C>  */
/*  Tokens: [0]=ID [1]=vertex [2]=axis [3]=diam [4]=height [5]=color  */
/* ------------------------------------------------------------------ */

static bool	paraboloid_validate(t_file_buf *fb, t_lexer *lex)
{
	if (!validate_normalized(fb, &lex->tokens[2], "paraboloid axis"))
		return (false);
	if (!validate_positive(fb, &lex->tokens[3], "paraboloid diameter"))
		return (false);
	if (!validate_positive(fb, &lex->tokens[4], "paraboloid height"))
		return (false);
	if (!validate_color(fb, &lex->tokens[5]))
		return (false);
	return (true);
}

bool	parse_paraboloid(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	t_rt_object	obj;

	if (!paraboloid_validate(fb, lex))
		return (false);
	if (sc->object_count >= RT_MAX_OBJECTS)
	{
		rt_error(&(t_err_loc){fb, lex->line_num,
				lex->tokens[0].col_start, lex->tokens[0].col_end},
			"too many objects (max %d)", RT_MAX_OBJECTS);
		return (false);
	}
	ft_memset(&obj, 0, sizeof(obj));
	obj.type = OBJ_PARABOLOID;
	obj.data.paraboloid.vertex = vec3_from_tok(&lex->tokens[1]);
	obj.data.paraboloid.axis = vec3_from_tok(&lex->tokens[2]);
	obj.data.paraboloid.diameter = lex->tokens[3].val.f;
	obj.data.paraboloid.height = lex->tokens[4].val.f;
	obj.data.paraboloid.color = color_from_tok(&lex->tokens[5]);
	rt_parse_material_kw(lex, 6, &obj.mat);
	sc->objects[sc->object_count] = obj;
	sc->object_count++;
	return (true);
}
