/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   json_parser.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JSON_PARSER_H
# define JSON_PARSER_H

# include "json_lexer.h"
# include <stdbool.h>
# include <stddef.h>

/* ------------------------------------------------------------------ */
/*  JSON node types                                                   */
/* ------------------------------------------------------------------ */

typedef enum e_json_type
{
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_STRING,
	JSON_NUMBER,
	JSON_BOOL,
	JSON_NULL
}	t_json_type;

/* ------------------------------------------------------------------ */
/*  Forward declarations                                              */
/* ------------------------------------------------------------------ */

typedef struct s_json_node		t_json_node;
typedef struct s_json_pair		t_json_pair;

/* ------------------------------------------------------------------ */
/*  JSON object = array of key-value pairs                            */
/* ------------------------------------------------------------------ */

struct s_json_pair
{
	char		key[JSON_STR_MAX];
	t_json_node	*value;
};

typedef struct s_json_object
{
	t_json_pair	*pairs;
	size_t		count;
	size_t		capacity;
}	t_json_object;

/* ------------------------------------------------------------------ */
/*  JSON array                                                        */
/* ------------------------------------------------------------------ */

typedef struct s_json_array
{
	t_json_node	**items;
	size_t		count;
	size_t		capacity;
}	t_json_array;

/* ------------------------------------------------------------------ */
/*  JSON node (tagged union)                                          */
/* ------------------------------------------------------------------ */

typedef union u_json_val
{
	char			str[JSON_STR_MAX];
	double			num;
	bool			boolean;
	t_json_object	obj;
	t_json_array	arr;
}	t_json_val;

struct s_json_node
{
	t_json_type	type;
	t_json_val	val;
};

/* ------------------------------------------------------------------ */
/*  API                                                               */
/* ------------------------------------------------------------------ */

/* Parse a JSON buffer into a node tree. Returns NULL on failure. */
t_json_node	*json_parse_buf(const char *buf, size_t len);

/* Parse a JSON file. Returns NULL on failure. */
t_json_node	*json_parse_file(const char *path);

/* Free a JSON node tree recursively. */
void		json_free(t_json_node *node);

#endif
