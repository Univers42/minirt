/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt_error.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:51:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_ERROR_H
# define RT_ERROR_H

# include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  ANSI color codes                                                  */
/* ------------------------------------------------------------------ */

# define ANSI_RESET		"\033[0m"
# define ANSI_BOLD		"\033[1m"
# define ANSI_DIM		"\033[2m"
# define ANSI_ITALIC	"\033[3m"
# define ANSI_UNDERLINE	"\033[4m"
# define ANSI_RED		"\033[31m"
# define ANSI_GREEN		"\033[32m"
# define ANSI_YELLOW	"\033[33m"
# define ANSI_BLUE		"\033[34m"
# define ANSI_CYAN		"\033[36m"
# define ANSI_WHITE		"\033[37m"

/* ------------------------------------------------------------------ */
/*  Error severity                                                    */
/* ------------------------------------------------------------------ */

typedef enum e_severity
{
	SEV_ERROR,
	SEV_WARNING,
	SEV_INFO
}	t_severity;

/* ------------------------------------------------------------------ */
/*  Parse error record                                                */
/* ------------------------------------------------------------------ */

typedef struct s_parse_error
{
	const char	*filename;
	int			line;
	int			col_start;
	int			col_end;
	char		message[256];
	t_severity	severity;
}	t_parse_error;

/* ------------------------------------------------------------------ */
/*  File buffer (stored for error context display)                    */
/* ------------------------------------------------------------------ */

# define RT_MAX_LINE_LEN	1024

typedef struct s_file_buf
{
	char		**lines;
	int			line_count;
	const char	*filename;
}	t_file_buf;

/* ------------------------------------------------------------------ */
/*  Error API                                                         */
/* ------------------------------------------------------------------ */

void	rt_error(const t_file_buf *fb, int line, int col_start,
			int col_end, const char *fmt, ...);
void	rt_warning(const t_file_buf *fb, int line, int col_start,
			int col_end, const char *fmt, ...);
void	rt_error_simple(const char *filename, const char *fmt, ...);
void	rt_print_error(const t_file_buf *fb, const t_parse_error *err);
void	file_buf_init(t_file_buf *fb, const char *filename);
void	file_buf_add_line(t_file_buf *fb, const char *line);
void	file_buf_free(t_file_buf *fb);

#endif
