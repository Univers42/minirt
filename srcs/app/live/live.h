/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   live.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIVE_H
# define LIVE_H

# include "rt_parser.h"
# include "rt_mlx.h"
# include "camera.h"
# include "bvh.h"
# include "bvh_flat.h"

/* ------------------------------------------------------------------ */
/*  Extra X11 / key codes for the interactive editor                  */
/* ------------------------------------------------------------------ */

# define X_KEY_RELEASE		3
# define KEY_MASK_RELEASE	(1L << 1)

# define LK_W		119
# define LK_A		97
# define LK_S		115
# define LK_D		100
# define LK_Q		113
# define LK_E		101
# define LK_I		105
# define LK_J		106
# define LK_K		107
# define LK_L		108
# define LK_LEFT	65361
# define LK_UP		65362
# define LK_RIGHT	65363
# define LK_DOWN	65364

/* Movement / rotation step per frame an action key is held. */
# define LIVE_MOVE_STEP		0.35
# define LIVE_ROT_STEP		0.04
/* Preview render width: small = instant feedback while flying. */
# define LIVE_PREVIEW_W		480

/* ------------------------------------------------------------------ */
/*  Held-key state: one flag per recognised action key.               */
/* ------------------------------------------------------------------ */

typedef struct s_live_keys
{
	int	fwd;
	int	back;
	int	left;
	int	right;
	int	up;
	int	down;
	int	yaw_l;
	int	yaw_r;
	int	pitch_u;
	int	pitch_d;
}	t_live_keys;

/* ------------------------------------------------------------------ */
/*  Editor state: scene, render camera, MLX ctx, accel + preview buf. */
/* ------------------------------------------------------------------ */

typedef struct s_live
{
	t_scene			*scene;
	t_mlx_ctx		ctx;
	t_camera		cam;
	t_bvh_node		*bvh;
	t_flat_bvh		*fbvh;
	t_hittable_list	accel;
	unsigned char	*buf;
	t_live_keys		keys;
	int				dirty;
	int				running;
}	t_live;

/* live_state.c */
int		live_state_init(t_live *lv, t_scene *scene);
void	live_build_accel(t_live *lv);
void	live_free_accel(t_live *lv);
void	live_teardown(t_live *lv);

/* live_camera.c */
void	live_rebuild_camera(t_live *lv);
void	live_apply_keys(t_live *lv);

/* live_loop.c */
int		live_frame(t_live *lv);
int		rt_live_run(t_scene *scene);

/* live_input.c */
int		live_key_press(int keycode, t_live *lv);
int		live_key_release(int keycode, t_live *lv);
int		live_close(t_live *lv);
int		live_expose(t_live *lv);

#endif
