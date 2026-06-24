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
#include <stdio.h>

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

/* Map rotate keys to yaw/pitch held flags ([ ] R also drive object yaw). */
static int	live_set_rot(t_live_keys *k, int code, int down)
{
	if (code == LK_LEFT || code == LK_J || code == LK_LBRACK || code == LK_R)
		return (k->yaw_l = down, 1);
	if (code == LK_RIGHT || code == LK_L || code == LK_RBRACK)
		return (k->yaw_r = down, 1);
	if (code == LK_UP || code == LK_I)
		return (k->pitch_u = down, 1);
	if (code == LK_DOWN || code == LK_K)
		return (k->pitch_d = down, 1);
	return (0);
}

/* Tab toggles camera/object mode; F2 or P save a PNG of the view. */
static int	live_set_action(int code, t_live *lv)
{
	if (code == LK_TAB)
	{
		lv->obj_mode = !lv->obj_mode;
		if (lv->obj_mode)
			fprintf(stderr, "Live: OBJECT mode (WASD/QE move, [ ]/R rotate)\n");
		else
			fprintf(stderr, "Live: CAMERA mode\n");
		return (1);
	}
	if (code == LK_F2 || code == LK_P)
		return (live_save_png(lv), 1);
	return (live_select_object(lv, code));
}

int	live_key_press(int keycode, t_live *lv)
{
	if (keycode == KEY_ESC)
		return (lv->running = 0, mlx_loop_end(lv->ctx.mlx), 0);
	if (live_set_move(&lv->keys, keycode, 1)
		|| live_set_rot(&lv->keys, keycode, 1)
		|| live_set_action(keycode, lv))
		return (0);
	return (0);
}

int	live_key_release(int keycode, t_live *lv)
{
	live_set_move(&lv->keys, keycode, 0);
	live_set_rot(&lv->keys, keycode, 0);
	return (0);
}
