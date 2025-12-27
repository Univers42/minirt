/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_math.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:00:32 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:00:32 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MATH_H
# define FT_MATH_H

static inline	double ft_clamp(double value, double min, double max)
{
	if (value < min)
		return (min);
	else if (value > max)
		return (max);
	else
		return (value);
}

static inline double ft_lerp_double(double t, double a, double b)
{
	return ((1 - t) * a + t * b);
}

static inline Vect3 ft_lerp_vec3(Vect3 t, Vect3 a, Vect3 b)
{
	return ((Vect3(1) - t) * a + t * b);
}

#endif
