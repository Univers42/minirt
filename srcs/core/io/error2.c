/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 08:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 08:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error_internal.h"
#include <stdio.h>
#include <stdarg.h>

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
		print_line_highlight(fb, err, col);
}

static void	fill_error(t_parse_error *err, const t_err_loc *loc,
				t_severity sev)
{
	err->severity = sev;
	err->filename = NULL;
	if (loc->fb)
		err->filename = loc->fb->filename;
	err->line = loc->line;
	err->col_start = loc->col_start;
	err->col_end = loc->col_end;
}

void	rt_error(const t_err_loc *loc, const char *fmt, ...)
{
	t_parse_error	err;
	va_list			ap;

	fprintf(stderr, "Error\n");
	fill_error(&err, loc, SEV_ERROR);
	va_start(ap, fmt);
	vsnprintf(err.message, sizeof(err.message), fmt, ap);
	va_end(ap);
	rt_print_error(loc->fb, &err);
}

void	rt_warning(const t_err_loc *loc, const char *fmt, ...)
{
	t_parse_error	err;
	va_list			ap;

	fill_error(&err, loc, SEV_WARNING);
	va_start(ap, fmt);
	vsnprintf(err.message, sizeof(err.message), fmt, ap);
	va_end(ap);
	rt_print_error(loc->fb, &err);
}
