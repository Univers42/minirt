/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_lexer.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "json_lexer.h"
#include <string.h>
#include <stdlib.h>

void	jlexer_init(t_jlexer *lex, const char *src, size_t len)
{
	lex->src = src;
	lex->len = len;
	lex->pos = 0;
	lex->line = 1;
	lex->col = 1;
}

static char	jlex_cur(const t_jlexer *lex)
{
	if (lex->pos >= lex->len)
		return ('\0');
	return (lex->src[lex->pos]);
}

static void	jlex_advance(t_jlexer *lex)
{
	if (lex->pos < lex->len)
	{
		if (lex->src[lex->pos] == '\n')
		{
			lex->line++;
			lex->col = 1;
		}
		else
			lex->col++;
		lex->pos++;
	}
}

static void	jlex_skip_ws(t_jlexer *lex)
{
	char	c;

	c = jlex_cur(lex);
	while (c == ' ' || c == '\t' || c == '\n' || c == '\r')
	{
		jlex_advance(lex);
		c = jlex_cur(lex);
	}
}

static bool	jlex_string(t_jlexer *lex, t_jtok *tok)
{
	int	i;

	jlex_advance(lex);
	i = 0;
	while (jlex_cur(lex) != '"' && jlex_cur(lex) != '\0')
	{
		if (jlex_cur(lex) == '\\')
		{
			jlex_advance(lex);
			if (jlex_cur(lex) == '\0')
				break ;
		}
		if (i < JSON_STR_MAX - 1)
			tok->str[i++] = jlex_cur(lex);
		jlex_advance(lex);
	}
	tok->str[i] = '\0';
	if (jlex_cur(lex) != '"')
		return (false);
	jlex_advance(lex);
	tok->type = JTOK_STRING;
	return (true);
}

static bool	jlex_number(t_jlexer *lex, t_jtok *tok)
{
	char	buf[64];
	int		i;
	char	c;
	char	*end;

	i = 0;
	c = jlex_cur(lex);
	while ((c >= '0' && c <= '9') || c == '-' || c == '+'
		|| c == '.' || c == 'e' || c == 'E')
	{
		if (i < 63)
			buf[i++] = c;
		jlex_advance(lex);
		c = jlex_cur(lex);
	}
	buf[i] = '\0';
	tok->num = strtod(buf, &end);
	if (end == buf)
		return (false);
	tok->type = JTOK_NUMBER;
	return (true);
}

static bool	jlex_keyword(t_jlexer *lex, t_jtok *tok,
		const char *word, t_jtok_type type)
{
	size_t	wlen;

	wlen = strlen(word);
	if (lex->pos + wlen > lex->len)
		return (false);
	if (memcmp(lex->src + lex->pos, word, wlen) != 0)
		return (false);
	lex->pos += wlen;
	lex->col += (int)wlen;
	tok->type = type;
	return (true);
}

static void	jlex_set_simple(t_jlexer *lex, t_jtok *tok, t_jtok_type t)
{
	tok->type = t;
	jlex_advance(lex);
}

static bool	jlex_dispatch(t_jlexer *lex, t_jtok *tok, char c)
{
	if (c == '{')
		return (jlex_set_simple(lex, tok, JTOK_LBRACE), true);
	if (c == '}')
		return (jlex_set_simple(lex, tok, JTOK_RBRACE), true);
	if (c == '[')
		return (jlex_set_simple(lex, tok, JTOK_LBRACKET), true);
	if (c == ']')
		return (jlex_set_simple(lex, tok, JTOK_RBRACKET), true);
	if (c == ':')
		return (jlex_set_simple(lex, tok, JTOK_COLON), true);
	if (c == ',')
		return (jlex_set_simple(lex, tok, JTOK_COMMA), true);
	if (c == '"')
		return (jlex_string(lex, tok));
	if (c == '-' || (c >= '0' && c <= '9'))
		return (jlex_number(lex, tok));
	if (c == 't')
		return (jlex_keyword(lex, tok, "true", JTOK_TRUE));
	if (c == 'f')
		return (jlex_keyword(lex, tok, "false", JTOK_FALSE));
	if (c == 'n')
		return (jlex_keyword(lex, tok, "null", JTOK_NULL));
	return (false);
}

bool	jlexer_next(t_jlexer *lex, t_jtok *tok)
{
	char	c;

	memset(tok, 0, sizeof(*tok));
	jlex_skip_ws(lex);
	tok->line = lex->line;
	tok->col = lex->col;
	c = jlex_cur(lex);
	if (c == '\0')
	{
		tok->type = JTOK_EOF;
		return (true);
	}
	if (!jlex_dispatch(lex, tok, c))
	{
		tok->type = JTOK_ERROR;
		return (false);
	}
	return (true);
}

bool	jlexer_peek(t_jlexer *lex, t_jtok *tok)
{
	t_jlexer	saved;

	saved = *lex;
	if (!jlexer_next(lex, tok))
	{
		*lex = saved;
		return (false);
	}
	*lex = saved;
	return (true);
}
