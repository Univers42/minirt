/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_pool.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:33:26 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:33:26 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef THREAD_POOL_H
# define THREAD_POOL_H

#include "../dsa/enque/safe_queue.h"
#include <pthrad.h>
#include <stdbool.h>

typedef void (*t_task_fn)(void *);

typedef struct s_task
{
	t_task_fn		function;
	void			*arg;
}	t_task;

typedef struct s_thread_pool
{
	pthread_t		*threads;
	int				num_threads;
	t_safe_queue	task_queue;
	pthread_cond_t 	cond;
	bool			shutdown;
}	t_thread_pool;

t_thread_pool	*thread_pool_create(int num_threads);
void			thread_pool_destroy(t_thread_pool *pool);
void			thread_pool_submit(t_thread_pool *pool, t_task_fn function, void *arg);
void			thread_shudown(void *arg);
void			*thread_worker(void *arg);
void			task_execute(void *arg);
void 			thread_pool_init(t_thread_pool *pool);
#endif