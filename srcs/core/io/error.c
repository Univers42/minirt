/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:51:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_error.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static const char	*severity_color(t_severity sev)
{
	if (sev == SEV_ERROR)
		return (ANSI_RED);
	if (sev == SEV_WARNING)
		return (ANSI_YELLOW);
	return (ANSI_BLUE);
}

static const char	*severity_label(t_severity sev)
{
	if (sev == SEV_ERROR)
		return ("ERROR");
	if (sev == SEV_WARNING)
		return ("WARNING");
	return ("INFO");
}

/*
** Print the line from the file buffer with a squiggle underline
** highlighting the error span [col_start, col_end).
*/
static void	print_line_highlight(const t_file_buf *fb, int line,
				int col_start, int col_end, const char *color)
{
	const char	*src;
	int			i;
	int			len;

	if (!fb || line < 1 || line > fb->line_count)
		return ;
	src = fb->lines[line - 1];
	len = (int)strlen(src);
	while (len > 0 && (src[len - 1] == '\n' || src[len - 1] == '\r'))
		len--;
	fprintf(stderr, " %4d " ANSI_DIM "|" ANSI_RESET " ", line);
	i = -1;
	while (++i < len)
		fputc(src[i], stderr);
	fputc('\n', stderr);
	fprintf(stderr, "      " ANSI_DIM "|" ANSI_RESET " ");
	i = 0;
	while (i < col_start)
	{
		fputc(' ', stderr);
		i++;
	}
	fprintf(stderr, "%s" ANSI_BOLD, color);
	if (col_end <= col_start)
		col_end = col_start + 1;
	fputc('^', stderr);
	i = col_start + 1;
	while (i < col_end)
	{
		fputc('~', stderr);
		i++;
	}
	fprintf(stderr, ANSI_RESET "\n");
}

void	rt_print_error(const t_file_buf *fb, const t_parse_error *err)
{
	const char	*col;

	col = severity_color(err->severity);
	fprintf(stderr, "%s" ANSI_BOLD "[%s]" ANSI_RESET ": ",
		col, severity_label(err->severity));
	fprintf(stderr, "%s\n", err->message);
	if (fb && err->filename)
	{
		fprintf(stderr, ANSI_DIM ANSI_ITALIC ANSI_UNDERLINE
			"%s:%d:%d:" ANSI_RESET "\n",
			err->filename, err->line, err->col_start + 1);
	}
	if (fb)
		print_line_highlight(fb, err->line, err->col_start,
			err->col_end, col);
}

void	rt_error(const t_file_buf *fb, int line, int col_start,
			int col_end, const char *fmt, ...)
{
	t_parse_error	err;
	va_list			ap;

	fprintf(stderr, "Error\n");
	err.severity = SEV_ERROR;
	err.filename = NULL;
	if (fb)
		err.filename = fb->filename;
	err.line = line;
	err.col_start = col_start;
	err.col_end = col_end;
	va_start(ap, fmt);
	vsnprintf(err.message, sizeof(err.message), fmt, ap);
	va_end(ap);
	rt_print_error(fb, &err);
}

void	rt_warning(const t_file_buf *fb, int line, int col_start,
			int col_end, const char *fmt, ...)
{
	t_parse_error	err;
	va_list			ap;

	err.severity = SEV_WARNING;
	err.filename = NULL;
	if (fb)
		err.filename = fb->filename;
	err.line = line;
	err.col_start = col_start;
	err.col_end = col_end;
	va_start(ap, fmt);
	vsnprintf(err.message, sizeof(err.message), fmt, ap);
	va_end(ap);
	rt_print_error(fb, &err);
}

void	rt_error_simple(const char *filename, const char *fmt, ...)
{
	va_list	ap;
	char	buf[256];

	fprintf(stderr, "Error\n");
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	if (filename)
		fprintf(stderr, ANSI_RED ANSI_BOLD "[ERROR]" ANSI_RESET
			" %s: %s\n", filename, buf);
	else
		fprintf(stderr, ANSI_RED ANSI_BOLD "[ERROR]" ANSI_RESET
			": %s\n", buf);
}

void	file_buf_init(t_file_buf *fb, const char *filename)
{
	fb->lines = NULL;
	fb->line_count = 0;
	fb->filename = filename;
}

void	file_buf_add_line(t_file_buf *fb, const char *line)
{
	char	**new_lines;
	char	*copy;
	int		i;

	copy = strdup(line);
	if (!copy)
		return ;
	new_lines = (char **)malloc(sizeof(char *) * (fb->line_count + 1));
	if (!new_lines)
	{
		free(copy);
		return ;
	}
	i = -1;
	while (++i < fb->line_count)
		new_lines[i] = fb->lines[i];
	new_lines[fb->line_count] = copy;
	free(fb->lines);
	fb->lines = new_lines;
	fb->line_count++;
}

void	file_buf_free(t_file_buf *fb)
{
	int	i;

	i = -1;
	while (++i < fb->line_count)
		free(fb->lines[i]);
	free(fb->lines);
	fb->lines = NULL;
	fb->line_count = 0;
}
