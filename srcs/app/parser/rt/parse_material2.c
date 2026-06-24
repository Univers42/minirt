/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_material2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 18:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 18:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_parser.h"
#include "rt_lexer.h"
#include <string.h>
#include "libft.h"

/*
** Copy the path token that follows "key:" on the line into dst.  A path
** token runs from just after the colon up to the next whitespace, newline
** or comment.  Returns the char past the value, or NULL if key is absent.
** The alpha-only lexer cannot tokenize paths, so we scan the raw line here.
*/
static const char	*copy_kw_value(const char *line, const char *key,
						char *dst, size_t cap)
{
	const char	*p;
	size_t		i;

	p = ft_strnstr(line, key, ft_strlen(line));
	if (!p)
		return (NULL);
	p += ft_strlen(key);
	i = 0;
	while (p[i] && p[i] != ' ' && p[i] != '\t' && p[i] != '\n'
		&& p[i] != '#' && i + 1 < cap)
	{
		dst[i] = p[i];
		i++;
	}
	dst[i] = '\0';
	return (p + i);
}

/*
** Parse the optional bump strength: "bumpstr:0.6".  Defaults to 1.0 when a
** bump map is given without an explicit strength.
*/
static void	parse_bump_strength(const char *line, t_mat_spec *mat)
{
	char		buf[64];
	const char	*end;

	if (!mat->bump_path[0])
		return ;
	mat->bump_strength = 1.0;
	end = copy_kw_value(line, "bumpstr:", buf, sizeof(buf));
	if (end && buf[0])
		mat->bump_strength = rt_atof(buf, NULL);
}

/*
** Scan a raw object line for image-map tokens that the alpha-only lexer
** cannot represent:  tex:<path>  bump:<path>  bumpstr:<float>.
** Fills mat->texture_path / mat->bump_path / mat->bump_strength in place.
*/
void	rt_parse_texture_kw(const char *line, t_mat_spec *mat)
{
	copy_kw_value(line, "tex:", mat->texture_path,
		sizeof(mat->texture_path));
	copy_kw_value(line, "bump:", mat->bump_path,
		sizeof(mat->bump_path));
	parse_bump_strength(line, mat);
}
