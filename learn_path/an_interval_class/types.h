/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:47:22 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 20:48:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
#define TYPES_H

/* only keep real_t here; point3 is defined in Point.h to avoid duplicate typedefs */
#ifndef FLOAT_TYPE
typedef double real_t;
#else
typedef float real_t;
#endif

#endif