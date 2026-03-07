/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_parse2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 21:52:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_parse.h"
#include "wf_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static bool	wf_parse_line_buf(t_wf_model *m, const char *buf, size_t len)
{
	char		line[4096];
	size_t		li;
	size_t		i;

	li = 0;
	i = 0;
	while (i < len)
	{
		if (buf[i] == '\n' || buf[i] == '\r')
		{
			line[li] = '\0';
			if (li > 0 && !wf_dispatch_line(m, line))
				return (false);
			li = 0;
			if (buf[i] == '\r' && i + 1 < len && buf[i + 1] == '\n')
				i++;
		}
		else if (li + 1 < sizeof(line))
			line[li++] = buf[i];
		i++;
	}
	if (li > 0)
	{
		line[li] = '\0';
		if (!wf_dispatch_line(m, line))
			return (false);
	}
	return (true);
}

bool	wf_parse_buf(t_wf_model *m, const char *buf, size_t len)
{
	bool	ok;

	ok = wf_parse_line_buf(m, buf, len);
	wf_model_compute_center(m);
	return (ok);
}

bool	wf_parse_file(t_wf_model *m, const char *path)
{
	FILE	*fp;
	char	*buf;
	long	sz;
	size_t	rd;
	bool	ok;

	fp = fopen(path, "rb");
	if (!fp)
		return (false);
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (sz <= 0)
	{
		fclose(fp);
		return (false);
	}
	buf = (char *)malloc((size_t)sz);
	if (!buf)
	{
		fclose(fp);
		return (false);
	}
	rd = fread(buf, 1, (size_t)sz, fp);
	fclose(fp);
	ok = wf_parse_buf(m, buf, rd);
	free(buf);
	return (ok);
}
