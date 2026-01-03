/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_pool.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/03 15:12:56 by dlesieur          #+#    #+#             */
/*   Updated: 2026/01/03 15:26:58 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdlib.h>
#include <stdbool.h>

/* Toggle pthread support with USE_PTHREAD macro */
#ifdef USE_PTHREAD
#include <pthread.h>
#endif

/* Forward declarations */
typedef struct s_task t_task;
typedef struct s_task_queue t_task_queue;
typedef struct s_thread_pool t_thread_pool;

/* Task function pointer type */
typedef void (*t_task_fn)(void *arg);

/* Task structure */
struct s_task
{
	t_task_fn function;
	void *arg;
	t_task *next;
};

/* Thread-safe task queue */
struct s_task_queue
{
	t_task *head;
	t_task *tail;
	int count;
#ifdef USE_PTHREAD
	pthread_mutex_t mutex;
	pthread_cond_t cond;
#endif
};

/* Thread pool structure */
struct s_thread_pool
{
#ifdef USE_PTHREAD
	pthread_t *threads;
	int active_workers; /* Number of threads currently processing tasks */
	pthread_mutex_t active_mutex;
	pthread_cond_t work_done;
#endif
	t_task_queue *queue;
	int num_threads;
	bool shutdown;
#ifdef USE_PTHREAD
	pthread_mutex_t shutdown_mutex;
#endif
};

/* Task queue functions */
static inline void task_queue_init(t_task_queue *queue)
{
	queue->head = NULL;
	queue->tail = NULL;
	queue->count = 0;
#ifdef USE_PTHREAD
	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->cond, NULL);
#endif
}

static inline void task_queue_destroy(t_task_queue *queue)
{
	t_task *current = queue->head;
	while (current)
	{
		t_task *next = current->next;
		free(current);
		current = next;
	}
#ifdef USE_PTHREAD
	pthread_mutex_destroy(&queue->mutex);
	pthread_cond_destroy(&queue->cond);
#endif
}

static inline bool task_queue_push(t_task_queue *queue, t_task_fn function, void *arg)
{
	t_task *task = (t_task *)malloc(sizeof(t_task));
	if (!task)
		return false;

	task->function = function;
	task->arg = arg;
	task->next = NULL;

#ifdef USE_PTHREAD
	pthread_mutex_lock(&queue->mutex);
#endif

	if (queue->tail)
		queue->tail->next = task;
	else
		queue->head = task;
	queue->tail = task;
	queue->count++;

#ifdef USE_PTHREAD
	pthread_cond_signal(&queue->cond);
	pthread_mutex_unlock(&queue->mutex);
#endif
	return true;
}

static inline bool task_queue_pop(t_task_queue *queue, t_task *out_task)
{
#ifdef USE_PTHREAD
	pthread_mutex_lock(&queue->mutex);
	/* non-blocking: if empty, return false */
	if (queue->count == 0)
	{
		pthread_mutex_unlock(&queue->mutex);
		return false;
	}
#endif

	if (queue->head == NULL)
	{
#ifdef USE_PTHREAD
		pthread_mutex_unlock(&queue->mutex);
#endif
		return false;
	}

	t_task *task = queue->head;
	*out_task = *task;
	queue->head = task->next;
	if (queue->head == NULL)
		queue->tail = NULL;
	queue->count--;

#ifdef USE_PTHREAD
	pthread_mutex_unlock(&queue->mutex);
#endif

	free(task);
	return true;
}

#ifdef USE_PTHREAD
/* Worker thread function */
static inline void *thread_pool_worker(void *arg)
{
	t_thread_pool *pool = (t_thread_pool *)arg;
	t_task task;

	while (true)
	{
		/* check shutdown with queue emptiness */
		pthread_mutex_lock(&pool->queue->mutex);
		int qcount = pool->queue->count;
		pthread_mutex_unlock(&pool->queue->mutex);

		pthread_mutex_lock(&pool->shutdown_mutex);
		bool should_shutdown = pool->shutdown;
		pthread_mutex_unlock(&pool->shutdown_mutex);

		if (should_shutdown && qcount == 0)
			break;

		if (task_queue_pop(pool->queue, &task))
		{
			pthread_mutex_lock(&pool->active_mutex);
			pool->active_workers++;
			pthread_mutex_unlock(&pool->active_mutex);

			task.function(task.arg);

			pthread_mutex_lock(&pool->active_mutex);
			pool->active_workers--;
			pthread_mutex_lock(&pool->queue->mutex);
			qcount = pool->queue->count;
			pthread_mutex_unlock(&pool->queue->mutex);
			if (pool->active_workers == 0 && qcount == 0)
				pthread_cond_broadcast(&pool->work_done);
			pthread_mutex_unlock(&pool->active_mutex);
		}
		else
			usleep(100); /* brief backoff */
	}
	return NULL;
}
#endif

/* Forward declare thread_pool_destroy */
static inline void thread_pool_destroy(t_thread_pool *pool);

/* Thread pool functions */
static inline t_thread_pool *thread_pool_create(int num_threads)
{
	if (num_threads <= 0)
		num_threads = 1;

	t_thread_pool *pool = (t_thread_pool *)malloc(sizeof(t_thread_pool));
	if (!pool)
		return NULL;

	pool->queue = (t_task_queue *)malloc(sizeof(t_task_queue));
	if (!pool->queue)
	{
		free(pool);
		return NULL;
	}

	task_queue_init(pool->queue);
	pool->num_threads = num_threads;
	pool->shutdown = false;

#ifdef USE_PTHREAD
	pool->active_workers = 0;
	pthread_mutex_init(&pool->shutdown_mutex, NULL);
	pthread_mutex_init(&pool->active_mutex, NULL);
	pthread_cond_init(&pool->work_done, NULL);

	pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
	if (!pool->threads)
	{
		pthread_mutex_destroy(&pool->active_mutex);
		pthread_cond_destroy(&pool->work_done);
		task_queue_destroy(pool->queue);
		free(pool->queue);
		free(pool);
		return NULL;
	}

	for (int i = 0; i < num_threads; i++)
	{
		if (pthread_create(&pool->threads[i], NULL, thread_pool_worker, pool) != 0)
		{
			pool->num_threads = i;
			thread_pool_destroy(pool);
			return NULL;
		}
	}
#else
	pool->threads = NULL;
#endif

	return pool;
}

static inline void thread_pool_destroy(t_thread_pool *pool)
{
	if (!pool)
		return;

#ifdef USE_PTHREAD
	pthread_mutex_lock(&pool->shutdown_mutex);
	pool->shutdown = true;
	pthread_mutex_unlock(&pool->shutdown_mutex);

	/* Wake up all threads */
	for (int i = 0; i < pool->num_threads; i++)
		pthread_cond_broadcast(&pool->queue->cond);

	/* Wait for threads to finish */
	for (int i = 0; i < pool->num_threads; i++)
		pthread_join(pool->threads[i], NULL);

	free(pool->threads);
	pthread_mutex_destroy(&pool->shutdown_mutex);
	pthread_mutex_destroy(&pool->active_mutex);
	pthread_cond_destroy(&pool->work_done);
#endif

	task_queue_destroy(pool->queue);
	free(pool->queue);
	free(pool);
}

static inline bool thread_pool_add_task(t_thread_pool *pool, t_task_fn function, void *arg)
{
	if (!pool || pool->shutdown)
		return false;

#ifndef USE_PTHREAD
	/* If pthread disabled, execute immediately */
	function(arg);
	return true;
#else
	return task_queue_push(pool->queue, function, arg);
#endif
}

static inline void thread_pool_wait(t_thread_pool *pool)
{
	if (!pool)
		return;
#ifdef USE_PTHREAD
	pthread_mutex_lock(&pool->active_mutex);
	while (true)
	{
		int active = pool->active_workers;
		pthread_mutex_lock(&pool->queue->mutex);
		int qcount = pool->queue->count;
		pthread_mutex_unlock(&pool->queue->mutex);
		if (active == 0 && qcount == 0)
			break;
		pthread_cond_wait(&pool->work_done, &pool->active_mutex);
	}
	pthread_mutex_unlock(&pool->active_mutex);
#endif
}

#endif