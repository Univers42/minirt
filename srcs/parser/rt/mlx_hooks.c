/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_hooks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/08 12:40:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt_mlx.h"
#include "mlx.h"
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/*  ESC key handler — close window and exit                           */
/* ------------------------------------------------------------------ */

int	mlx_key_handler(int keycode, t_mlx_ctx *ctx)
{
	if (keycode == KEY_ESC)
	{
		mlx_ctx_destroy(ctx);
		exit(0);
	}
	return (0);
}

/* ------------------------------------------------------------------ */
/*  Window close (red cross / DestroyNotify) handler                  */
/* ------------------------------------------------------------------ */

int	mlx_close_handler(t_mlx_ctx *ctx)
{
	mlx_ctx_destroy(ctx);
	exit(0);
	return (0);
}
