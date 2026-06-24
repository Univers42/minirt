/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:11:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_lexer.h"
#include <string.h>

/*
** Single-line tokenizer for the .rt format.
** Each line has:  IDENTIFIER  [values...]
**
** The tokenizer identifies the element type first, then depending
** on the element, interprets the remaining values as:
**   - floats (single numbers)
**   - vec3   (comma-separated triples: x,y,z)
**   - color  (comma-separated int triples: R,G,B)
**
** The token layout per element type is fixed, so we just scan
** left-to-right placing typed tokens into lex->tokens[].
** This gives O(n) per line with O(1) working memory.
*/

typedef struct s_elem_schema
{
	const char	*id;
	int			token_types[8];
}	t_elem_schema;

/*
** Token type codes for schema: 'V'=vec3, 'F'=float, 'C'=color, 'I'=int, 0=end
*/
static int	g_schema_a[] = {'F', 'C', 0};
static int	g_schema_c[] = {'V', 'V', 'F', 0};
static int	g_schema_l[] = {'V', 'F', 'C', 0};
/* 'M' = optional trailing material keyword (glass, metal, ...) */
static int	g_schema_sp[] = {'V', 'F', 'C', 'M', 0};
static int	g_schema_pl[] = {'V', 'V', 'C', 'M', 0};
static int	g_schema_cy[] = {'V', 'V', 'F', 'F', 'C', 'M', 0};
static int	g_schema_co[] = {'V', 'V', 'F', 'F', 'C', 'M', 0};
/* tr – Triangle: v0:V  v1:V  v2:V  color:C  [material] */
static int	g_schema_tr[] = {'V', 'V', 'V', 'C', 'M', 0};
/* di – Disk: center:V  normal:V  diameter:F  color:C  [material] */
static int	g_schema_di[] = {'V', 'V', 'F', 'C', 'M', 0};
/* pb – Paraboloid: vertex:V  axis:V  diameter:F  height:F  color:C  [mat] */
static int	g_schema_pb[] = {'V', 'V', 'F', 'F', 'C', 'M', 0};
/* hy – Hyperboloid: center:V  axis:V  diameter:F  height:F  color:C  [mat] */
static int	g_schema_hy[] = {'V', 'V', 'F', 'F', 'C', 'M', 0};

static int	*get_schema(const char *id)
{
	if (strcmp(id, "A") == 0)
		return (g_schema_a);
	if (strcmp(id, "C") == 0)
		return (g_schema_c);
	if (strcmp(id, "L") == 0)
		return (g_schema_l);
	if (strcmp(id, "sp") == 0)
		return (g_schema_sp);
	if (strcmp(id, "pl") == 0)
		return (g_schema_pl);
	if (strcmp(id, "cy") == 0)
		return (g_schema_cy);
	if (strcmp(id, "co") == 0)
		return (g_schema_co);
	if (strcmp(id, "tr") == 0)
		return (g_schema_tr);
	if (strcmp(id, "di") == 0)
		return (g_schema_di);
	if (strcmp(id, "pb") == 0)
		return (g_schema_pb);
	if (strcmp(id, "hy") == 0)
		return (g_schema_hy);
	return (NULL);
}

static bool	add_token(t_lexer *lex, t_token_type type,
				t_token_val val, int col_start)
{
	if (lex->count >= MAX_TOKENS)
		return (false);
	lex->tokens[lex->count].type = type;
	lex->tokens[lex->count].val = val;
	lex->tokens[lex->count].col_start = col_start;
	lex->tokens[lex->count].col_end = lex->pos;
	lex->count++;
	return (true);
}

static bool	parse_by_schema(t_lexer *lex, int *schema)
{
	int			col;
	t_token_val	val;

	while (*schema)
	{
		lexer_skip_spaces(lex);
		col = lex->pos;
		memset(&val, 0, sizeof(val));
		if (*schema == 'F')
		{
			if (!lexer_parse_float(lex, &val.f))
				return (false);
			add_token(lex, TOK_FLOAT, val, col);
		}
		else if (*schema == 'I')
		{
			if (!lexer_parse_int(lex, &val.i))
				return (false);
			add_token(lex, TOK_INT, val, col);
		}
		else if (*schema == 'V')
		{
			if (!lexer_parse_vec3(lex, &val.vec))
				return (false);
			add_token(lex, TOK_VEC3, val, col);
		}
		else if (*schema == 'C')
		{
			if (!lexer_parse_color(lex, &val.col))
				return (false);
			add_token(lex, TOK_COLOR, val, col);
		}
		else if (*schema == 'M')
		{
			if (lexer_parse_identifier(lex, val.id, sizeof(val.id)))
				add_token(lex, TOK_IDENTIFIER, val, col);
		}
		schema++;
	}
	return (true);
}

static bool	is_empty_or_comment(const char *line)
{
	while (*line == ' ' || *line == '\t')
		line++;
	return (*line == '\0' || *line == '\n' || *line == '#');
}

/*
** Tokenize a single line.
** Returns: number of tokens (>= 1 identifier + values), or 0 for
** empty/comment lines, or -1 on error.
*/
int	tokenize_line(t_lexer *lex, const char *line, int line_num)
{
	t_token_val	val;
	int			col;
	int			*schema;

	lex->line = line;
	lex->pos = 0;
	lex->line_num = line_num;
	lex->count = 0;
	if (is_empty_or_comment(line))
		return (0);
	lexer_skip_spaces(lex);
	col = lex->pos;
	memset(&val, 0, sizeof(val));
	if (!lexer_parse_identifier(lex, val.id, sizeof(val.id)))
		return (-1);
	add_token(lex, TOK_IDENTIFIER, val, col);
	schema = get_schema(val.id);
	if (!schema)
		return (-1);
	if (!parse_by_schema(lex, schema))
		return (-1);
	return (lex->count);
}
