/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_json_parser.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 16:50:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "json_parser.h"
#include "json_helpers.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int	g_pass;
static int	g_fail;

static void	ok(int cond, const char *msg)
{
	if (cond)
	{
		printf("  \033[32m✓\033[0m %s\n", msg);
		g_pass++;
	}
	else
	{
		printf("  \033[31m✗\033[0m %s\n", msg);
		g_fail++;
	}
}

/* ------------------------------------------------------------------ */

static void	test_empty_object(void)
{
	t_json_node	*n;

	printf("test_empty_object\n");
	n = json_parse_buf("{}", 2);
	ok(n != NULL, "parses {}");
	ok(n && n->type == JSON_OBJECT, "type is OBJECT");
	ok(n && n->val.obj.count == 0, "count == 0");
	json_free(n);
}

static void	test_simple_object(void)
{
	const char	*src = "{\"x\": 1, \"y\": 2.5, \"name\": \"hello\"}";
	t_json_node	*n;

	printf("test_simple_object\n");
	n = json_parse_buf(src, strlen(src));
	ok(n != NULL, "parses object");
	ok(json_num(json_get(n, "x"), -1) == 1.0, "x == 1");
	ok(json_num(json_get(n, "y"), -1) == 2.5, "y == 2.5");
	ok(strcmp(json_str(json_get(n, "name"), ""), "hello") == 0,
		"name == hello");
	json_free(n);
}

static void	test_array(void)
{
	const char	*src = "[1, 2, 3]";
	t_json_node	*n;

	printf("test_array\n");
	n = json_parse_buf(src, strlen(src));
	ok(n != NULL, "parses array");
	ok(n && n->type == JSON_ARRAY, "type is ARRAY");
	ok(json_arr_len(n) == 3, "length == 3");
	ok(json_num(json_arr_at(n, 0), -1) == 1.0, "[0] == 1");
	ok(json_num(json_arr_at(n, 2), -1) == 3.0, "[2] == 3");
	json_free(n);
}

static void	test_nested(void)
{
	const char	*src = "{\"pos\": [1, 2, 3], \"flag\": true, \"nil\": null}";
	t_json_node	*n;
	t_vec3		v;

	printf("test_nested\n");
	n = json_parse_buf(src, strlen(src));
	ok(n != NULL, "parses nested");
	v = json_to_vec3(json_get(n, "pos"));
	ok(v.x == 1.0 && v.y == 2.0 && v.z == 3.0, "vec3 = (1,2,3)");
	ok(json_bool(json_get(n, "flag"), false) == true, "flag == true");
	ok(json_get(n, "nil") && json_get(n, "nil")->type == JSON_NULL,
		"nil == null");
	json_free(n);
}

static void	test_color255(void)
{
	const char	*src = "[255, 128, 0]";
	t_json_node	*n;
	t_color		c;

	printf("test_color255\n");
	n = json_parse_buf(src, strlen(src));
	c = json_to_color255(n);
	ok(c.x == 1.0, "r normalised to 1.0");
	ok(c.y > 0.49 && c.y < 0.51, "g ≈ 0.50");
	ok(c.z == 0.0, "b == 0");
	json_free(n);
}

int	main(void)
{
	printf("\n=== JSON Parser Tests ===\n\n");
	test_empty_object();
	test_simple_object();
	test_array();
	test_nested();
	test_color255();
	printf("\n--- %d passed, %d failed ---\n\n", g_pass, g_fail);
	return (g_fail != 0);
}
