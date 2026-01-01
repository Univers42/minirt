/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:47:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/01 17:12:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

# ifndef FLOAT_TYPE
typedef struct s_point3
{
	double	e[3];
}	t_point3;
# else

typedef struct s_point3
{
	float	e[3];
}	t_point3;

#endif

#ifndef FLOAT_TYPE
typedef double real_t;
#else
typedef float real_t;
#endif

#endif