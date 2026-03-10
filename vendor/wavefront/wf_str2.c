/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_str2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 20:14:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "wf_str.h"

bool	wf_starts_with(const char *s, const char *prefix)
{
	while (*prefix)
	{
		if (*s != *prefix)
			return (false);
		s++;
		prefix++;
	}
	return (true);
}
