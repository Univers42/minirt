/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   specular.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 14:52:58 by marvin            #+#    #+#             */
/*   Updated: 2025/12/28 14:52:58 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPECULAR_H
# define SPECULAR_H

# include "brdf.h"

typedef struct s_specular
{
	double		ks;
	double		e;
	t_vec3		cs;
}	t_specular;

t_specular	*specular_new(double ks, double e, t_vec3 cs);
t_specular  *specular_new_simple(double ks, t_vec3 color);
void		specular_free(t_specular *spec);
t_vec3		specular_f(const t_specular *spec, const t_hit_info *hit_info, const t_vec3 wi, const t_vec3 wo);
t_vec3		specular_sample_f(const t_specular *spec, const t_hit_info *hit_info, const t_vec3 wo, t_vec3 *wi, double *pdf);
t_vec3		specular_color(const t_specular *spec);
void		specular_copy(t_specular *dst, const t_specular *src);
double		specular_pdf(const t_specular *s, const t_material *mat, const t_hit_info *hit_info, const t_vec3 *wi,const t_vec3 *wo);
t_vec3		specular_rho(const t_specular *s, const t_material *mat, const t_hit_info *hit_info, const t_vec3 *wi,const t_vec3 *wo);
# endif