/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interval3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 21:00:45 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 19:14:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "interval.h"

t_interval	interval_add(const t_interval *ival, real_t displacement)
{
	return (interval(ival->min + displacement, ival->max + displacement));
}
