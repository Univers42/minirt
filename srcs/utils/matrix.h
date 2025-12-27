/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:03:52 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:03:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MATRIX_H
# define MATRIX_H
# include "vec3.h"
# include "point3.h"
# include "normal.h"

typedef struct s_matrix4
{
	double	elements[4][4];
}			t_matrix4;

t_matrix4	matrix4_new(void);
t_matrix4	matrix4_copy(const t_matrix4 *mat);
t_matrix4	matrix4_assign(t_matrix4 *dst, const t_matrix4 *src);
void		matrix4_set_identity(t_matrix4 *mat);
t_matrix4	matrix4_mul(const t_matrix4 *a, const t_matrix4 *b);
t_vec3		matrix4_mul_vec3(const t_matrix4 *mat, const t_vec3 *vect);
t_point3	matrix4_mul_point3(const t_matrix4 *mat, const t_point3 *point);
t_normal	matrix4_mul_normal(const t_matrix4 *mat, const t_normal *normal);
t_matrix4	matrix4_div_scale(const t_matrix4 *mat, double d);

#endif