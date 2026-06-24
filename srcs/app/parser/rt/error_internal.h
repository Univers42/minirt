/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_internal.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 08:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 08:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_INTERNAL_H
# define ERROR_INTERNAL_H

# include "rt_error.h"

const char	*severity_color(t_severity sev);
const char	*severity_label(t_severity sev);
void		print_line_highlight(const t_file_buf *fb,
				const t_parse_error *err, const char *color);

#endif
