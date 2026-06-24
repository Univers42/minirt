/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 08:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 08:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GNL_H
# define GNL_H

# include <stddef.h>

# ifndef GNL_BUFFER_SIZE
#  define GNL_BUFFER_SIZE 1024
# endif

size_t	gnl_strlen(const char *s);
char	*gnl_has_nl(const char *s);
char	*extract_line(char **stash);
char	*rt_get_next_line(int fd);

#endif
