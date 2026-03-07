/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   noise_texture.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:15:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NOISE_TEXTURE_H
# define NOISE_TEXTURE_H

# include "texture.h"
# include "perlin.h"
# include "color.h"
# include <stdlib.h>
# include <math.h>

typedef struct s_noise_texture
{
	t_perlin	perlin;
	real_t		scale;
	bool		use_turb;
	int			turb_depth;
}	t_noise_texture;

real_t		rt_clamp(real_t x, real_t lo, real_t hi);
t_color		noise_texture_value(const t_texture *tex, real_t u, real_t v,
				const t_point3 *p);
void		noise_texture_destroy(t_texture *tex);
t_texture	*noise_texture_create(real_t scale);
t_texture	*noise_texture_create_turb(real_t scale, int turb_depth);

#endif
