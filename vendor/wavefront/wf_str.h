/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wf_str.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/07 21:51:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WF_STR_H
# define WF_STR_H

# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>

const char	*wf_skip_spaces(const char *s);
const char	*wf_next_token(const char *s, char *buf, size_t bufsz);
float		wf_parse_float(const char **pp);
int32_t		wf_parse_int(const char **pp);
bool		wf_starts_with(const char *s, const char *prefix);

#endif
