/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   material8.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 08:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 08:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "material.h"

t_color	diffuse_light_emitted(t_emit *e)
{
	const t_diffuse_light	*light;
	t_color					c;

	light = (const t_diffuse_light *)e->mat->data;
	if (!light || !light->tex)
		return (vec3_create(0.0, 0.0, 0.0));
	if (!e->front_face)
		return (vec3_create(0.0, 0.0, 0.0));
	c = light->tex->value(light->tex, e->u, e->v, e->p);
	if (light->scale > (real_t)0.0)
		return (vec3_mul_scalar(&c, light->scale));
	return (vec3_mul_scalar(&c, (real_t)1.0));
}
