/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 08:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 08:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gnl.h"
#include <stdlib.h>

static char	*extract_rest(char *stash, size_t len)
{
	char	*rest;
	size_t	i;

	rest = malloc(gnl_strlen(stash + len) + 1);
	if (!rest)
		return (NULL);
	i = 0;
	while (stash[len + i])
	{
		rest[i] = stash[len + i];
		i++;
	}
	rest[i] = '\0';
	return (rest);
}

static char	*copy_line(const char *stash, size_t len)
{
	char	*line;
	size_t	i;

	line = malloc(len + 1);
	if (!line)
		return (NULL);
	i = -1;
	while (++i < len)
		line[i] = stash[i];
	line[len] = '\0';
	return (line);
}

char	*extract_line(char **stash)
{
	char	*nl;
	char	*line;
	char	*rest;
	size_t	len;

	if (!*stash || !**stash)
		return (free(*stash), *stash = NULL, NULL);
	nl = gnl_has_nl(*stash);
	if (nl)
		len = (size_t)(nl - *stash) + 1;
	else
		len = gnl_strlen(*stash);
	line = copy_line(*stash, len);
	if (!line)
		return (NULL);
	rest = NULL;
	if (nl && (*stash)[len])
		rest = extract_rest(*stash, len);
	if (nl && (*stash)[len] && !rest)
		return (free(line), NULL);
	free(*stash);
	*stash = rest;
	return (line);
}
