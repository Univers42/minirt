/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_hooks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 14:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/09 17:32:07 by dlesieur         ###   ########.fr       */
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

/* ------------------------------------------------------------------ */
/*  Expose handler — redraw image when window is revealed             */
/* ------------------------------------------------------------------ */

int	mlx_expose_handler(t_mlx_ctx *ctx)
{
	if (ctx->img && ctx->mlx && ctx->win)
	{
		mlx_put_image_to_window(ctx->mlx, ctx->win, ctx->img,
			(t_mlx_point){0, 0});
		mlx_do_sync(ctx->mlx);
	}
	return (0);
}
