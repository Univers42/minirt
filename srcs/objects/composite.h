/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   composite.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 11:36:10 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 11:36:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMPOSITE_H
# define COMPOSITE_H

# include "../includes/utils.h"
# include <stddef.h>
# include "box.h"
# include "../includes/objects.h"
# include "../includes/constants.h"

// Dynamic array of children
typedef struct s_composite
{
	t_material	*shader;
	t_object	**objects;
	size_t		count;
	size_t		capacity;
}	t_composite;

static inline void	compose_init(t_composite *c)
{
	if (!c)
		return ;
	c->shader = NULL;
	c->objects = NULL;
	c->count = 0;
	c->capacity = 0;
}

static inline void	capacity_init_with_shader(t_composite *c, t_material *shader)
{
	composite_init(c);
	c->shader = shader;
}

static inline void	composite_destroy(t_composite *c)
{
	if (!c)
		return ;
	free(c->objects);
	c->objects = NULL;
	c->count = 0;
	c->capacity = 0;
}

void	composite_add_object(t_composite *c, t_object *obj);
t_box	composite_calculate_bounding_box(t_composite *c);
int		composite_hit(t_composite *c, t_ray const *ray, t_posnorm *inter_norm, double *tmin)


#endif	// COMPOSITE_H