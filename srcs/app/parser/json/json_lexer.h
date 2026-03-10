/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_lexer.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JSON_LEXER_H
# define JSON_LEXER_H

# include <stdbool.h>
# include <stddef.h>

/* ------------------------------------------------------------------ */
/*  Token types                                                       */
/* ------------------------------------------------------------------ */

typedef enum e_jtok_type
{
	JTOK_LBRACE,
	JTOK_RBRACE,
	JTOK_LBRACKET,
	JTOK_RBRACKET,
	JTOK_COLON,
	JTOK_COMMA,
	JTOK_STRING,
	JTOK_NUMBER,
	JTOK_TRUE,
	JTOK_FALSE,
	JTOK_NULL,
	JTOK_EOF,
	JTOK_ERROR
}	t_jtok_type;

/* ------------------------------------------------------------------ */
/*  Token                                                             */
/* ------------------------------------------------------------------ */

# define JSON_STR_MAX	512

typedef struct s_jtok
{
	t_jtok_type	type;
	char		str[JSON_STR_MAX];
	double		num;
	int			line;
	int			col;
}	t_jtok;

/* ------------------------------------------------------------------ */
/*  Lexer state                                                       */
/* ------------------------------------------------------------------ */

typedef struct s_jlexer
{
	const char	*src;
	size_t		len;
	size_t		pos;
	int			line;
	int			col;
}	t_jlexer;

/* ------------------------------------------------------------------ */
/*  API                                                               */
/* ------------------------------------------------------------------ */

void	jlexer_init(t_jlexer *lex, const char *src, size_t len);
bool	jlexer_next(t_jlexer *lex, t_jtok *tok);
bool	jlexer_peek(t_jlexer *lex, t_jtok *tok);

#endif
