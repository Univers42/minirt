/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_str.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:14:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_str.h"
#include <string.h>
#include <math.h>

const char	*wf_skip_spaces(const char *s)
{
	while (*s == ' ' || *s == '\t')
		s++;
	return (s);
}

const char	*wf_next_token(const char *s, char *buf, size_t bufsz)
{
	size_t	i;

	s = wf_skip_spaces(s);
	i = 0;
	while (*s && *s != ' ' && *s != '\t' && *s != '\n' && *s != '\r')
	{
		if (i + 1 < bufsz)
			buf[i++] = *s;
		s++;
	}
	buf[i] = '\0';
	return (s);
}

static float	wf_parse_exp(const char **pp)
{
	int		exp_sign;
	int		exp_val;

	exp_sign = 1;
	if (**pp == '-')
	{
		exp_sign = -1;
		(*pp)++;
	}
	else if (**pp == '+')
		(*pp)++;
	exp_val = 0;
	while (**pp >= '0' && **pp <= '9')
		exp_val = exp_val * 10 + (*(*pp)++ - '0');
	return (powf(10.0f, (float)(exp_sign * exp_val)));
}

float	wf_parse_float(const char **pp)
{
	const char	*p;
	float		sign;
	float		val;
	float		frac;

	p = wf_skip_spaces(*pp);
	sign = 1.0f;
	if (*p == '-')
	{
		sign = -1.0f;
		p++;
	}
	else if (*p == '+')
		p++;
	val = 0.0f;
	while (*p >= '0' && *p <= '9')
		val = val * 10.0f + (float)(*p++ - '0');
	if (*p == '.')
	{
		p++;
		frac = 0.1f;
		while (*p >= '0' && *p <= '9')
		{
			val += (float)(*p++ - '0') * frac;
			frac *= 0.1f;
		}
	}
	if (*p == 'e' || *p == 'E')
	{
		p++;
		*pp = p;
		val *= wf_parse_exp(pp);
		return (sign * val);
	}
	*pp = p;
	return (sign * val);
}

int32_t	wf_parse_int(const char **pp)
{
	const char	*p;
	int32_t		sign;
	int32_t		val;

	p = wf_skip_spaces(*pp);
	sign = 1;
	if (*p == '-')
	{
		sign = -1;
		p++;
	}
	else if (*p == '+')
		p++;
	val = 0;
	while (*p >= '0' && *p <= '9')
		val = val * 10 + (*p++ - '0');
	*pp = p;
	return (sign * val);
}
