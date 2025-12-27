/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_queue.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:35:19 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:35:19 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SAFE_QUEUE_H
# define SAFE_QUEUE_H

#include <pthread.h>
#include <stdlib.h>

typedef struct s_queue_node
{
	void				*data;
	struct s_queue_node *next;
}	t_queue_node;

typedef struct s_safe_queue
{
	t_queue_node	*front;
	t_queue_node	*rear;
	int				size;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
}t_safe_queue;


#endif