/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "json_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "libft.h"

/* Forward declaration for recursive parsing */
static t_json_node	*parse_value(t_jlexer *lex);

/* ------------------------------------------------------------------ */
/*  Node allocation                                                   */
/* ------------------------------------------------------------------ */

static t_json_node	*node_new(t_json_type type)
{
	t_json_node	*n;

	n = (t_json_node *)malloc(sizeof(t_json_node));
	if (!n)
		return (NULL);
	ft_memset(n, 0, sizeof(*n));
	n->type = type;
	return (n);
}

/* ------------------------------------------------------------------ */
/*  Object helpers                                                    */
/* ------------------------------------------------------------------ */

static bool	obj_add_pair(t_json_object *obj, const char *key,
		t_json_node *val)
{
	t_json_pair	*tmp;
	size_t		newcap;

	if (obj->count >= obj->capacity)
	{
		newcap = obj->capacity == 0 ? 8 : obj->capacity * 2;
		tmp = realloc(obj->pairs, newcap * sizeof(t_json_pair));
		if (!tmp)
			return (false);
		obj->pairs = tmp;
		obj->capacity = newcap;
	}
	ft_memset(obj->pairs[obj->count].key, 0, JSON_STR_MAX);
	strncpy(obj->pairs[obj->count].key, key, JSON_STR_MAX - 1);
	obj->pairs[obj->count].value = val;
	obj->count++;
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Array helpers                                                     */
/* ------------------------------------------------------------------ */

static bool	arr_add_item(t_json_array *arr, t_json_node *item)
{
	t_json_node	**tmp;
	size_t		newcap;

	if (arr->count >= arr->capacity)
	{
		newcap = arr->capacity == 0 ? 8 : arr->capacity * 2;
		tmp = realloc(arr->items, newcap * sizeof(t_json_node *));
		if (!tmp)
			return (false);
		arr->items = tmp;
		arr->capacity = newcap;
	}
	arr->items[arr->count] = item;
	arr->count++;
	return (true);
}

/* ------------------------------------------------------------------ */
/*  Parse object: { "key": value, ... }                               */
/* ------------------------------------------------------------------ */

static bool	parse_one_pair(t_jlexer *lex, t_json_node *obj)
{
	t_jtok		key_tok;
	t_jtok		colon;
	t_json_node	*val;

	if (!jlexer_next(lex, &key_tok) || key_tok.type != JTOK_STRING)
		return (false);
	if (!jlexer_next(lex, &colon) || colon.type != JTOK_COLON)
		return (false);
	val = parse_value(lex);
	if (!val)
		return (false);
	return (obj_add_pair(&obj->val.obj, key_tok.str, val));
}

static t_json_node	*parse_object(t_jlexer *lex)
{
	t_json_node	*node;
	t_jtok		tok;

	node = node_new(JSON_OBJECT);
	if (!node)
		return (NULL);
	jlexer_peek(lex, &tok);
	if (tok.type == JTOK_RBRACE)
		return (jlexer_next(lex, &tok), node);
	while (1)
	{
		if (!parse_one_pair(lex, node))
			return (json_free(node), NULL);
		jlexer_peek(lex, &tok);
		if (tok.type == JTOK_COMMA)
			jlexer_next(lex, &tok);
		else
			break ;
	}
	if (!jlexer_next(lex, &tok) || tok.type != JTOK_RBRACE)
		return (json_free(node), NULL);
	return (node);
}

/* ------------------------------------------------------------------ */
/*  Parse array: [ value, ... ]                                       */
/* ------------------------------------------------------------------ */

static t_json_node	*parse_array(t_jlexer *lex)
{
	t_json_node	*node;
	t_jtok		tok;
	t_json_node	*item;

	node = node_new(JSON_ARRAY);
	if (!node)
		return (NULL);
	jlexer_peek(lex, &tok);
	if (tok.type == JTOK_RBRACKET)
		return (jlexer_next(lex, &tok), node);
	while (1)
	{
		item = parse_value(lex);
		if (!item || !arr_add_item(&node->val.arr, item))
			return (json_free(node), NULL);
		jlexer_peek(lex, &tok);
		if (tok.type == JTOK_COMMA)
			jlexer_next(lex, &tok);
		else
			break ;
	}
	if (!jlexer_next(lex, &tok) || tok.type != JTOK_RBRACKET)
		return (json_free(node), NULL);
	return (node);
}

/* ------------------------------------------------------------------ */
/*  Parse a single value                                              */
/* ------------------------------------------------------------------ */

static t_json_node	*parse_value(t_jlexer *lex)
{
	t_jtok		tok;
	t_json_node	*node;

	if (!jlexer_next(lex, &tok))
		return (NULL);
	if (tok.type == JTOK_LBRACE)
		return (parse_object(lex));
	if (tok.type == JTOK_LBRACKET)
		return (parse_array(lex));
	if (tok.type == JTOK_STRING)
	{
		node = node_new(JSON_STRING);
		if (node)
			strncpy(node->val.str, tok.str, JSON_STR_MAX - 1);
		return (node);
	}
	if (tok.type == JTOK_NUMBER)
	{
		node = node_new(JSON_NUMBER);
		if (node)
			node->val.num = tok.num;
		return (node);
	}
	if (tok.type == JTOK_TRUE || tok.type == JTOK_FALSE)
	{
		node = node_new(JSON_BOOL);
		if (node)
			node->val.boolean = (tok.type == JTOK_TRUE);
		return (node);
	}
	if (tok.type == JTOK_NULL)
		return (node_new(JSON_NULL));
	return (NULL);
}

/* ------------------------------------------------------------------ */
/*  Free                                                              */
/* ------------------------------------------------------------------ */

void	json_free(t_json_node *node)
{
	size_t	i;

	if (!node)
		return ;
	if (node->type == JSON_OBJECT)
	{
		i = 0;
		while (i < node->val.obj.count)
		{
			json_free(node->val.obj.pairs[i].value);
			i++;
		}
		free(node->val.obj.pairs);
	}
	else if (node->type == JSON_ARRAY)
	{
		i = 0;
		while (i < node->val.arr.count)
		{
			json_free(node->val.arr.items[i]);
			i++;
		}
		free(node->val.arr.items);
	}
	free(node);
}

/* ------------------------------------------------------------------ */
/*  Public API                                                        */
/* ------------------------------------------------------------------ */

t_json_node	*json_parse_buf(const char *buf, size_t len)
{
	t_jlexer	lex;
	t_json_node	*root;

	jlexer_init(&lex, buf, len);
	root = parse_value(&lex);
	return (root);
}

t_json_node	*json_parse_file(const char *path)
{
	FILE		*fp;
	char		*buf;
	long		sz;
	size_t		rd;
	t_json_node	*root;

	fp = fopen(path, "rb");
	if (!fp)
		return (NULL);
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (sz <= 0)
		return (fclose(fp), NULL);
	buf = (char *)malloc((size_t)sz + 1);
	if (!buf)
		return (fclose(fp), NULL);
	rd = fread(buf, 1, (size_t)sz, fp);
	fclose(fp);
	buf[rd] = '\0';
	root = json_parse_buf(buf, rd);
	free(buf);
	return (root);
}
