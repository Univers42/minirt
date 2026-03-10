/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_lexer.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:25:25 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_LEXER_H
# define RT_LEXER_H

# include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  Token types                                                       */
/* ------------------------------------------------------------------ */

typedef enum e_token_type
{
	TOK_IDENTIFIER,
	TOK_FLOAT,
	TOK_INT,
	TOK_VEC3,
	TOK_COLOR,
	TOK_EOL,
	TOK_ERROR
}	t_token_type;

/* ------------------------------------------------------------------ */
/*  Token value union                                                 */
/* ------------------------------------------------------------------ */

typedef struct s_vec3_val
{
	double	x;
	double	y;
	double	z;
}	t_vec3_val;

typedef struct s_color_val
{
	int	r;
	int	g;
	int	b;
}	t_color_val;

typedef union u_token_val
{
	double		f;
	int			i;
	t_vec3_val	vec;
	t_color_val	col;
	char		id[8];
}	t_token_val;

/* ------------------------------------------------------------------ */
/*  Token                                                             */
/* ------------------------------------------------------------------ */

# define MAX_TOKENS	16

typedef struct s_token
{
	t_token_type	type;
	t_token_val		val;
	int				col_start;
	int				col_end;
}	t_token;

/* ------------------------------------------------------------------ */
/*  Lexer state (per-line, stack-allocated)                           */
/* ------------------------------------------------------------------ */

typedef struct s_lexer
{
	const char	*line;
	int			pos;
	int			line_num;
	t_token		tokens[MAX_TOKENS];
	int			count;
}	t_lexer;

/* ------------------------------------------------------------------ */
/*  Lexer functions                                                   */
/* ------------------------------------------------------------------ */

int		tokenize_line(t_lexer *lex, const char *line, int line_num);
void	lexer_skip_spaces(t_lexer *lex);
bool	lexer_parse_float(t_lexer *lex, double *out);
bool	lexer_parse_int(t_lexer *lex, int *out);
bool	lexer_parse_vec3(t_lexer *lex, t_vec3_val *out);
bool	lexer_parse_color(t_lexer *lex, t_color_val *out);
bool	lexer_parse_identifier(t_lexer *lex, char *out, int maxlen);

/* ------------------------------------------------------------------ */
/*  ft_atof (custom float parser)                                     */
/* ------------------------------------------------------------------ */

double	rt_atof(const char *s, const char **endp);

#endif
