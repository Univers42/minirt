/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Point.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 01:00:09 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 01:19:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POINT_H
# define POINT_H

# include "types.h"

#ifndef FLOAT_TYPE
typedef struct s_point3
{
	double	x;
	double	y;
	double	z;
}	t_point3;
# else

typedef struct s_point3
{
	float	x;
	float	y;
	float	z;
}	t_point3;


#endif

t_point3	point3_create(real_t x, real_t y, real_t z)
{
	return ((t_point3){.x = x, .y = y, .z = z});
}

# endif