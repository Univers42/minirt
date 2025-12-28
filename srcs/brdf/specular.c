/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   specular.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 15:00:00 by marvin            #+#    #+#             */
/*   Updated: 2025/12/28 15:00:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "specular.h"

t_specular	*specular_new(double factor, double exp, t_vec3 color)
{
	t_specular	*spec;

	spec = (t_specular *)malloc(sizeof(t_specular));
	if (!spec)
		return (NULL);
	spec->ks = factor;
	spec->e = exp;
	spec->cs = color;
	return (spec);
}

t_specular	*specular_new_simple(double ks, t_vec3 color)
{
	return (specular_new(ks, 20.0, color));
}

void	specular_free(t_specular *spec)
{
	if (spec)
		free(spec);
}

t_vec3	specular_f(const t_specular *s, const t_hit_info *hit_info, const t_vec3 *wi, const t_vec3 *wo)
{
	(void)hit_info;
	(void)wi;
	(void)wo;
	return (vec3(0.0, 0.0, 0.0));
}

t_vec3	specular_color(const t_specular *s)
{
	return (vec_scale(&s->cs, s->ks));
}

Vect3 specular_sample_f(const Specular *s, const Material *mat, const Hitinfo *hitinfo, const Vect3 *wo)
{
    (void)s; (void)mat; (void)wo;
    /* reflection R = D - 2*(D·N)*N
       assuming hitinfo->direction is incident direction D and hitinfo->normal is N */
    double dot = v_dot(&hitinfo->direction, &hitinfo->normal);
    Vect3 scaledN = v_scale(&hitinfo->normal, 2.0 * dot);
    return v_sub(&hitinfo->direction, &scaledN);
}

double specular_pdf(const Specular *s, const Material *mat, const Hitinfo *hitinfo, const Vect3 *wi, const Vect3 *wo)
{
    (void)s; (void)mat; (void)wo;
    double d = v_dot(&hitinfo->normal, wi);
    return d > 0.0 ? d : 0.0;
}

Vect3 specular_eval(const Specular *s, const Material *mat, const Hitinfo *hitinfo, const Vect3 *wi, const Vect3 *wo)
{
    (void)mat; (void)hitinfo; (void)wi; (void)wo;
    return v_scale(&s->cs, s->ks);
}