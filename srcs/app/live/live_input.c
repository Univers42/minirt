/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live_input.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "live.h"
#include "mlx.h"

/* Map a keycode to the held-key flag it controls; set it to `down`.
   Returns 1 if the key was a recognised movement key, else 0. */
static int	live_set_move(t_live_keys *k, int code, int down)
{
	if (code == LK_W)
		return (k->fwd = down, 1);
	if (code == LK_S)
		return (k->back = down, 1);
	if (code == LK_A)
		return (k->left = down, 1);
	if (code == LK_D)
		return (k->right = down, 1);
	if (code == LK_E)
		return (k->up = down, 1);
	if (code == LK_Q)
		return (k->down = down, 1);
	return (0);
}

/* Map rotate keys (arrows or I/J/K/L) to yaw/pitch held flags. */
static int	live_set_rot(t_live_keys *k, int code, int down)
{
	if (code == LK_LEFT || code == LK_J)
		return (k->yaw_l = down, 1);
	if (code == LK_RIGHT || code == LK_L)
		return (k->yaw_r = down, 1);
	if (code == LK_UP || code == LK_I)
		return (k->pitch_u = down, 1);
	if (code == LK_DOWN || code == LK_K)
		return (k->pitch_d = down, 1);
	return (0);
}

int	live_key_press(int keycode, t_live *lv)
{
	if (keycode == KEY_ESC)
		return (lv->running = 0, mlx_loop_end(lv->ctx.mlx), 0);
	if (live_set_move(&lv->keys, keycode, 1)
		|| live_set_rot(&lv->keys, keycode, 1))
		return (0);
	return (0);
}

int	live_key_release(int keycode, t_live *lv)
{
	live_set_move(&lv->keys, keycode, 0);
	live_set_rot(&lv->keys, keycode, 0);
	return (0);
}

int	live_close(t_live *lv)
{
	lv->running = 0;
	mlx_loop_end(lv->ctx.mlx);
	return (0);
}
