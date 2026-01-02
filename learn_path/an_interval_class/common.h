/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 20:00:51 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/02 21:35:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_H
#define COMMON_H

#include "settings.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

/* Include interval early so objects/hittable_list can use t_interval */
#include "interval.h"

/* low-level math types and helpers */
/* Ensure primitive types and vec/point/ray/object/hittable_list are declared
   before color.h which depends on hittable_list. */
#include "vector.h"
#include "point.h"
#include "ray.h"
#include "object.h"
#include "hittable.h"
#include "hittable_list.h"
#include "color.h"

/* UTILITIES IN COMMON */
static inline real_t degrees_to_radians(real_t degrees) { return (degrees * PI / 180.0); }

#endif