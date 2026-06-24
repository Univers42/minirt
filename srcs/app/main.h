/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 10:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/24 10:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include "rt_parser.h"
# include "camera.h"

/* Where the final render is sent. */
typedef enum e_out_mode
{
	OUT_WINDOW = 0,
	OUT_PPM = 1,
	OUT_PNG = 2
}	t_out_mode;

/* Command-line options resolved by parse_args. */
typedef struct s_opts
{
	t_out_mode	out;
	int			edit;
}	t_opts;

/* main_output.c */
const char	*ppm_out_path(void);
const char	*png_out_path(void);
int			display_ppm(t_camera *cam, unsigned char *buf);
int			display_png(t_camera *cam, unsigned char *buf);

/* main_display.c */
int			display_scene(t_scene *scene, t_out_mode out);

/* main_run.c */
int			run_rt_display(const char *filepath, t_out_mode out);
int			run_json(const char *filepath, t_out_mode out);
int			run_edit(const char *filepath);

/* main_obj.c */
int			run_obj(const char *filepath, t_out_mode out);

/* main.c */
const char	*get_ext(const char *path);

#endif
