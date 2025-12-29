/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 22:46:28 by marvin            #+#    #+#             */
/*   Updated: 2025/12/29 22:46:28 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include <stdbool.h>

static inline int lodepng_max(int a, int b)
{
    return (a > b) ? a : b;
}

static inline int lodepng_min(int a, int b)
{
    return (a < b) ? a : b;
}
#endif