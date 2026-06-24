/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_objects.c                                    :+:      :+:    :+:   */
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
#include <string.h>

/* libft prototype (rt_error.h collides with libft.h's ft_stddef.h) */
void	*ft_memset(void *b, int c, size_t len);

void	rt_parse_material_kw(t_lexer *lex, int idx, t_mat_spec *mat);
bool	validate_range_f(const t_file_buf *fb, const t_token *tok,
			double lo, double hi, const char *name);
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
/*  Add a generic object to t_scene.objects[]                         */
/* ------------------------------------------------------------------ */

static bool	add_object(t_scene *sc, t_lexer *lex, t_file_buf *fb,
			t_rt_object *obj)
{
	if (sc->object_count >= RT_MAX_OBJECTS)
	{
		rt_error(&(t_err_loc){fb, lex->line_num,
				lex->tokens[0].col_start, lex->tokens[0].col_end},
			"too many objects (max %d)", RT_MAX_OBJECTS);
		return (false);
	}
	sc->objects[sc->object_count] = *obj;
	sc->object_count++;
	return (true);
}

/* ------------------------------------------------------------------ */
/*  sp – Sphere                                                       */
/*  Schema: sp <center:V> <diameter:F> <color:C>                      */
/*  Tokens: [0]=ID  [1]=center  [2]=diameter  [3]=color               */
/* ------------------------------------------------------------------ */

bool	parse_sphere(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	t_rt_object	obj;

	if (!validate_positive(fb, &lex->tokens[2], "sphere diameter"))
		return (false);
	if (!validate_color(fb, &lex->tokens[3]))
		return (false);
	ft_memset(&obj, 0, sizeof(obj));
	obj.type = OBJ_SPHERE;
	obj.data.sphere.center = vec3_from_tok(&lex->tokens[1]);
	obj.data.sphere.diameter = lex->tokens[2].val.f;
	obj.data.sphere.color = color_from_tok(&lex->tokens[3]);
	rt_parse_material_kw(lex, 4, &obj.mat);
	return (add_object(sc, lex, fb, &obj));
}

/* ------------------------------------------------------------------ */
/*  pl – Plane                                                        */
/*  Schema: pl <point:V> <normal:V> <color:C>                         */
/*  Tokens: [0]=ID  [1]=point  [2]=normal  [3]=color                  */
/* ------------------------------------------------------------------ */

bool	parse_plane(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	t_rt_object	obj;

	if (!validate_normalized(fb, &lex->tokens[2], "plane normal"))
		return (false);
	if (!validate_color(fb, &lex->tokens[3]))
		return (false);
	ft_memset(&obj, 0, sizeof(obj));
	obj.type = OBJ_PLANE;
	obj.data.plane.point = vec3_from_tok(&lex->tokens[1]);
	obj.data.plane.normal = vec3_from_tok(&lex->tokens[2]);
	obj.data.plane.color = color_from_tok(&lex->tokens[3]);
	rt_parse_material_kw(lex, 4, &obj.mat);
	return (add_object(sc, lex, fb, &obj));
}

/* ------------------------------------------------------------------ */
/*  cy – Cylinder                                                     */
/*  Schema: cy <center:V> <axis:V> <diameter:F> <height:F> <color:C>  */
/*  Tokens: [0]=ID [1]=center [2]=axis [3]=diameter [4]=height [5]=col*/
/* ------------------------------------------------------------------ */

bool	parse_cylinder(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	t_rt_object	obj;

	if (!validate_normalized(fb, &lex->tokens[2], "cylinder axis"))
		return (false);
	if (!validate_positive(fb, &lex->tokens[3], "cylinder diameter"))
		return (false);
	if (!validate_positive(fb, &lex->tokens[4], "cylinder height"))
		return (false);
	if (!validate_color(fb, &lex->tokens[5]))
		return (false);
	ft_memset(&obj, 0, sizeof(obj));
	obj.type = OBJ_CYLINDER;
	obj.data.cylinder.center = vec3_from_tok(&lex->tokens[1]);
	obj.data.cylinder.axis = vec3_from_tok(&lex->tokens[2]);
	obj.data.cylinder.diameter = lex->tokens[3].val.f;
	obj.data.cylinder.height = lex->tokens[4].val.f;
	obj.data.cylinder.color = color_from_tok(&lex->tokens[5]);
	rt_parse_material_kw(lex, 6, &obj.mat);
	return (add_object(sc, lex, fb, &obj));
}
