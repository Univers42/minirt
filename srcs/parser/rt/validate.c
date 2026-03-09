/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:40:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_lexer.h"
#include "rt_error.h"
#include <math.h>

/*
** Range checkers return false and print an error on failure.
** All validation is done BEFORE scene construction so no heap
** is wasted on invalid data.
*/

bool	validate_range_f(const t_file_buf *fb, const t_token *tok,
				double lo, double hi, const char *name)
{
	if (tok->val.f < lo || tok->val.f > hi)
	{
		rt_error(fb, 0, tok->col_start, tok->col_end,
			"%s must be in range [%.1f, %.1f], got %.4f",
			name, lo, hi, tok->val.f);
		return (false);
	}
	return (true);
}

bool	validate_color(const t_file_buf *fb, const t_token *tok)
{
	t_color_val	c;

	c = tok->val.col;
	if (c.r < 0 || c.r > 255 || c.g < 0 || c.g > 255
		|| c.b < 0 || c.b > 255)
	{
		rt_error(fb, 0, tok->col_start, tok->col_end,
			"color channels must be in [0, 255], got %d,%d,%d",
			c.r, c.g, c.b);
		return (false);
	}
	return (true);
}

bool	validate_normalized(const t_file_buf *fb, const t_token *tok,
				const char *name)
{
	t_vec3_val	v;
	double		len;

	v = tok->val.vec;
	if (v.x < -1.0 || v.x > 1.0 || v.y < -1.0 || v.y > 1.0
		|| v.z < -1.0 || v.z > 1.0)
	{
		rt_error(fb, 0, tok->col_start, tok->col_end,
			"%s components must be in [-1, 1]", name);
		return (false);
	}
	len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (len < 0.0001)
	{
		rt_error(fb, 0, tok->col_start, tok->col_end,
			"%s cannot be a zero vector", name);
		return (false);
	}
	return (true);
}

bool	validate_positive(const t_file_buf *fb, const t_token *tok,
				const char *name)
{
	if (tok->val.f <= 0.0)
	{
		rt_error(fb, 0, tok->col_start, tok->col_end,
			"%s must be positive, got %.4f", name, tok->val.f);
		return (false);
	}
	return (true);
}
