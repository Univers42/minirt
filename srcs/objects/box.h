/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   box.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 11:36:08 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 11:36:08 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOX_H
# define BOX_H

# include "../includes/utils.h"

typedef struct s_box
{
	t_point3	p1;
	t_point3	p2;
}	t_box;

static inline void	box_init(t_box *b)
{
    if (!b)
        return;
    memset(b, 0, sizeof(*b));
}

static inline void	box_init_from(t_box *b, const t_point3 *p1, const t_point3 *p2)
{
	if (!b || !p1 || !p2)
		return ;
	b->p1 = *p1;
	b->p2 = *p2;
}

/**
 * for now empty but keep it in case we need to extend the logic to dynamic allocation
 */
static inline void	box_destroy(t_box *b)
{
	(void)b;
}

t_point3	*box_get_vertices(const t_box *b, t_point3 v[8]);
int			box_inside(const t_box *b, const t_point3 *p);
int			box_hit(const t_box *b, const t_ray *ray, double *t);

#endif