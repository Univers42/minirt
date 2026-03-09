/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:40:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_lexer.h"
#include <stdlib.h>

void	lexer_skip_spaces(t_lexer *lex)
{
	while (lex->line[lex->pos] == ' ' || lex->line[lex->pos] == '\t')
		lex->pos++;
}

static int	is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

static int	is_sign(char c)
{
	return (c == '+' || c == '-');
}

/*
** Parse a single float value at current position.
** Advances lex->pos past the consumed characters.
*/
bool	lexer_parse_float(t_lexer *lex, double *out)
{
	const char	*start;
	const char	*end;

	start = lex->line + lex->pos;
	if (!is_digit(*start) && !is_sign(*start)
		&& *start != '.')
		return (false);
	*out = rt_atof(start, &end);
	if (end == start)
		return (false);
	lex->pos += (int)(end - start);
	return (true);
}

/*
** Parse a single integer value at current position.
** Only accepts digits (with optional leading sign), no decimal point.
*/
bool	lexer_parse_int(t_lexer *lex, int *out)
{
	const char	*s;
	int			sign;
	int			val;

	s = lex->line + lex->pos;
	sign = 1;
	if (*s == '-' || *s == '+')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
	if (!is_digit(*s))
		return (false);
	val = 0;
	while (is_digit(*s))
	{
		val = val * 10 + (*s - '0');
		s++;
	}
	*out = val * sign;
	lex->pos = (int)(s - lex->line);
	return (true);
}

/*
** Parse a comma-separated triple of floats: "x,y,z"
** No spaces allowed between components (per .rt format).
*/
bool	lexer_parse_vec3(t_lexer *lex, t_vec3_val *out)
{
	int	saved;

	saved = lex->pos;
	if (!lexer_parse_float(lex, &out->x))
		return (lex->pos = saved, false);
	if (lex->line[lex->pos] != ',')
		return (lex->pos = saved, false);
	lex->pos++;
	if (!lexer_parse_float(lex, &out->y))
		return (lex->pos = saved, false);
	if (lex->line[lex->pos] != ',')
		return (lex->pos = saved, false);
	lex->pos++;
	if (!lexer_parse_float(lex, &out->z))
		return (lex->pos = saved, false);
	return (true);
}

/*
** Parse a comma-separated triple of integers: "R,G,B"
** No spaces between components.
*/
bool	lexer_parse_color(t_lexer *lex, t_color_val *out)
{
	int	saved;

	saved = lex->pos;
	if (!lexer_parse_int(lex, &out->r))
		return (lex->pos = saved, false);
	if (lex->line[lex->pos] != ',')
		return (lex->pos = saved, false);
	lex->pos++;
	if (!lexer_parse_int(lex, &out->g))
		return (lex->pos = saved, false);
	if (lex->line[lex->pos] != ',')
		return (lex->pos = saved, false);
	lex->pos++;
	if (!lexer_parse_int(lex, &out->b))
		return (lex->pos = saved, false);
	return (true);
}

/*
** Parse an identifier (1-2 alpha characters).
*/
bool	lexer_parse_identifier(t_lexer *lex, char *out, int maxlen)
{
	int		i;
	char	c;

	i = 0;
	while (i < maxlen - 1)
	{
		c = lex->line[lex->pos + i];
		if (c < 'A' || (c > 'Z' && c < 'a') || c > 'z')
			break ;
		out[i] = c;
		i++;
	}
	if (i == 0)
		return (false);
	out[i] = '\0';
	lex->pos += i;
	return (true);
}
