/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:40:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_lexer.h"

/*
** Hand-rolled float parser for .rt files.
** Handles: optional sign, integer part, optional fractional part.
** No scientific notation needed for the .rt format.
** Sets *endp to the character after the last consumed digit.
** Returns 0.0 if no valid number is found.
*/

static int	rt_is_space(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\r' || c == '\f' || c == '\v');
}

static const char	*skip_sign(const char *s, double *sign)
{
	*sign = 1.0;
	if (*s == '-')
	{
		*sign = -1.0;
		s++;
	}
	else if (*s == '+')
		s++;
	return (s);
}

static double	parse_frac(const char *s, const char **endp)
{
	double	frac;
	double	div;

	frac = 0.0;
	div = 10.0;
	while (*s >= '0' && *s <= '9')
	{
		frac += (*s - '0') / div;
		div *= 10.0;
		s++;
	}
	*endp = s;
	return (frac);
}

double	rt_atof(const char *s, const char **endp)
{
	double		sign;
	double		integer;
	const char	*start;

	while (rt_is_space(*s))
		s++;
	s = skip_sign(s, &sign);
	start = s;
	integer = 0.0;
	while (*s >= '0' && *s <= '9')
	{
		integer = integer * 10.0 + (*s - '0');
		s++;
	}
	if (*s == '.')
	{
		s++;
		integer += parse_frac(s, &s);
	}
	if (s == start)
	{
		if (endp)
			*endp = start;
		return (0.0);
	}
	if (endp)
		*endp = s;
	return (sign * integer);
}
