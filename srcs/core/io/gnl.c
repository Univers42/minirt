/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:40:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"
#include <stdlib.h>
#include <unistd.h>

size_t	gnl_strlen(const char *s)
{
	size_t	n;

	if (!s)
		return (0);
	n = 0;
	while (s[n])
		n++;
	return (n);
}

static char	*gnl_strjoin(char *s1, const char *s2)
{
	char	*out;
	size_t	l1;
	size_t	l2;
	size_t	i;

	l1 = gnl_strlen(s1);
	l2 = gnl_strlen(s2);
	out = malloc(l1 + l2 + 1);
	if (!out)
		return (free(s1), NULL);
	i = 0;
	while (i < l1)
	{
		out[i] = s1[i];
		i++;
	}
	while (i < l1 + l2)
	{
		out[i] = s2[i - l1];
		i++;
	}
	out[i] = '\0';
	free(s1);
	return (out);
}

char	*gnl_has_nl(const char *s)
{
	if (!s)
		return (NULL);
	while (*s)
	{
		if (*s == '\n')
			return ((char *)s);
		s++;
	}
	return (NULL);
}

static char	*read_until_nl(int fd, char *stash)
{
	char	buf[GNL_BUFFER_SIZE + 1];
	ssize_t	n;

	while (!gnl_has_nl(stash))
	{
		n = read(fd, buf, GNL_BUFFER_SIZE);
		if (n <= 0)
		{
			if (n < 0)
			{
				free(stash);
				return (NULL);
			}
			return (stash);
		}
		buf[n] = '\0';
		stash = gnl_strjoin(stash, buf);
		if (!stash)
			return (NULL);
	}
	return (stash);
}

/*
** Simplified GNL for the parser: single fd, no multi-fd support needed.
** Returns one line (including '\n' if present), or NULL at EOF/error.
** Caller must free() the returned line.
*/
char	*rt_get_next_line(int fd)
{
	static char	*stash;

	if (fd < 0)
		return (NULL);
	stash = read_until_nl(fd, stash);
	if (!stash)
		return (NULL);
	return (extract_line(&stash));
}
