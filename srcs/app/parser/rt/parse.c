/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/11 00:11:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "rt_lexer.h"
#include "rt_error.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  External parse handlers (parse_elements.c / parse_objects.c /     */
/*  parse_bonus.c)                                                    */
/* ------------------------------------------------------------------ */

bool	parse_ambient(t_scene *sc, t_lexer *lex, t_file_buf *fb);
bool	parse_camera(t_scene *sc, t_lexer *lex, t_file_buf *fb);
bool	parse_light(t_scene *sc, t_lexer *lex, t_file_buf *fb);
bool	parse_sphere(t_scene *sc, t_lexer *lex, t_file_buf *fb);
bool	parse_plane(t_scene *sc, t_lexer *lex, t_file_buf *fb);
bool	parse_cylinder(t_scene *sc, t_lexer *lex, t_file_buf *fb);
bool	parse_cone(t_scene *sc, t_lexer *lex, t_file_buf *fb);
bool	parse_triangle(t_scene *sc, t_lexer *lex, t_file_buf *fb);

/* ------------------------------------------------------------------ */
/*  GNL (gnl.c)                                                       */
/* ------------------------------------------------------------------ */

char	*rt_get_next_line(int fd);

/* ------------------------------------------------------------------ */
/*  Dispatch table lookup                                             */
/* ------------------------------------------------------------------ */

typedef bool	(*t_parse_fn)(t_scene *, t_lexer *, t_file_buf *);

typedef struct s_dispatch
{
	const char	*id;
	t_parse_fn	fn;
}	t_dispatch;

static const t_dispatch	g_dispatch[] = {
	{"A", parse_ambient},
	{"C", parse_camera},
	{"L", parse_light},
	{"sp", parse_sphere},
	{"pl", parse_plane},
	{"cy", parse_cylinder},
	{"co", parse_cone},
	{"tr", parse_triangle},
	{NULL, NULL}
};

static t_parse_fn	dispatch_lookup(const char *id)
{
	int	i;

	i = 0;
	while (g_dispatch[i].id)
	{
		if (strcmp(id, g_dispatch[i].id) == 0)
			return (g_dispatch[i].fn);
		i++;
	}
	return (NULL);
}

/* ------------------------------------------------------------------ */
/*  Validate .rt file extension                                       */
/* ------------------------------------------------------------------ */

static bool	valid_extension(const char *path)
{
	size_t	len;

	len = strlen(path);
	if (len < 4)
		return (false);
	return (strcmp(path + len - 3, ".rt") == 0);
}

/* ------------------------------------------------------------------ */
/*  Process one tokenized line                                        */
/* ------------------------------------------------------------------ */

static bool	process_line(t_scene *sc, t_lexer *lex, t_file_buf *fb)
{
	const char	*id;
	t_parse_fn	fn;

	id = lex->tokens[0].val.id;
	fn = dispatch_lookup(id);
	if (!fn)
	{
		rt_error(fb, lex->line_num, lex->tokens[0].col_start,
			lex->tokens[0].col_end,
			"unknown element type '%s'", id);
		return (false);
	}
	return (fn(sc, lex, fb));
}

/* ------------------------------------------------------------------ */
/*  Scene init / cleanup                                              */
/* ------------------------------------------------------------------ */

void	scene_init(t_scene *scene)
{
	memset(scene, 0, sizeof(*scene));
	hittable_list_init(&scene->world);
	mat_registry_reset();
}

void	scene_cleanup(t_scene *scene)
{
	hittable_list_clear(&scene->world);
	mat_registry_free_all();
}

/* ------------------------------------------------------------------ */
/*  Main parse loop: open → GNL → lex → dispatch → validate required */
/* ------------------------------------------------------------------ */

static bool	read_and_parse(int fd, t_scene *sc, t_file_buf *fb)
{
	char	*line;
	t_lexer	lex;
	int		line_num;
	int		tok_ret;
	bool	ok;

	line_num = 0;
	ok = true;
	line = rt_get_next_line(fd);
	while (line)
	{
		line_num++;
		file_buf_add_line(fb, line);
		tok_ret = tokenize_line(&lex, line, line_num);
		if (tok_ret < 0)
		{
			rt_error(fb, line_num, lex.tokens[0].col_start,
				lex.tokens[0].col_end,
				"syntax error: %s", lex.tokens[0].val.id);
			ok = false;
		}
		else if (tok_ret > 0)
		{
			if (!process_line(sc, &lex, fb))
				ok = false;
		}
		free(line);
		line = rt_get_next_line(fd);
	}
	return (ok);
}

static bool	validate_required(t_scene *sc, t_file_buf *fb)
{
	bool	ok;

	ok = true;
	if (!sc->has_ambient)
	{
		rt_error_simple(fb->filename,
			"missing required ambient light (A)");
		ok = false;
	}
	if (!sc->has_camera)
	{
		rt_error_simple(fb->filename,
			"missing required camera (C)");
		ok = false;
	}
	return (ok);
}

bool	parse_scene(const char *filepath, t_scene *scene)
{
	int			fd;
	t_file_buf	fb;
	bool		ok;

	if (!valid_extension(filepath))
	{
		rt_error_simple(filepath,
			"invalid file extension (expected .rt)");
		return (false);
	}
	fd = open(filepath, O_RDONLY);
	if (fd < 0)
	{
		rt_error_simple(filepath, "cannot open file");
		return (false);
	}
	scene_init(scene);
	file_buf_init(&fb, filepath);
	ok = read_and_parse(fd, scene, &fb);
	close(fd);
	if (ok)
		ok = validate_required(scene, &fb);
	file_buf_free(&fb);
	return (ok);
}
