/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error3.c                                           :+:      :+:    :+:   */
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
#include <stdlib.h>
#include <string.h>

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
