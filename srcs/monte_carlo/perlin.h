/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 23:07:54 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:48:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PERLIN_H
# define PERLIN_H

# include "random.h"
# include "types.h"
# include "vector.h"
# include <stdlib.h>
# include <math.h>

# define POINT_COUNT 256

typedef struct s_perlin
{
	t_vec3	rand_vec[POINT_COUNT];
	int		perm_x[POINT_COUNT];
	int		perm_y[POINT_COUNT];
	int		perm_z[POINT_COUNT];
}	t_perlin;

void	perlin_permute(int *p, int n);
void	perlin_generate_perm(int *p);
real_t	perlin_fade(real_t t);
real_t	perlin_interp(const t_vec3 c[2][2][2], real_t u, real_t v, real_t w);
void	perlin_init(t_perlin *perlin);
real_t	perlin_noise(const t_perlin *perlin, const t_vec3 *p);
real_t	perlin_turb(const t_perlin *perlin, const t_vec3 *p, int depth);

#endif

