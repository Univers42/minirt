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

#include "error_internal.h"
#include <stdio.h>
#include <string.h>

const char	*severity_color(t_severity sev)
{
	if (sev == SEV_ERROR)
		return (ANSI_RED);
	if (sev == SEV_WARNING)
		return (ANSI_YELLOW);
	return (ANSI_BLUE);
}

const char	*severity_label(t_severity sev)
{
	if (sev == SEV_ERROR)
		return ("ERROR");
	if (sev == SEV_WARNING)
		return ("WARNING");
	return ("INFO");
}

static void	print_src_line(const char *src, int line)
{
	int	i;
	int	len;

	len = (int)strlen(src);
	while (len > 0 && (src[len - 1] == '\n' || src[len - 1] == '\r'))
		len--;
	fprintf(stderr, " %4d " ANSI_DIM "|" ANSI_RESET " ", line);
	i = -1;
	while (++i < len)
		fputc(src[i], stderr);
	fputc('\n', stderr);
}

static void	print_squiggle(int col_start, int col_end, const char *color)
{
	int	i;

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

/*
** Print the line from the file buffer with a squiggle underline
** highlighting the error span [col_start, col_end).
*/
void	print_line_highlight(const t_file_buf *fb, const t_parse_error *err,
			const char *color)
{
	if (!fb || err->line < 1 || err->line > fb->line_count)
		return ;
	print_src_line(fb->lines[err->line - 1], err->line);
	print_squiggle(err->col_start, err->col_end, color);
}
