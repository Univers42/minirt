/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread_pool.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:42:10 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:42:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "thread_pool.h"

void *thread_worker(void *arg)
{
    t_thread_pool *pool = (t_thread_pool *)arg;
    
    while (true) {
        pthread_mutex_lock(&pool->mutex);
        
        while (safe_queue_empty(&pool->task_queue) && !pool->shutdown) {
            pthread_cond_wait(&pool->condition, &pool->mutex);
        }
        
        if (pool->shutdown && safe_queue_empty(&pool->task_queue)) {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }
        
        void *task_data;
        bool dequeued = safe_queue_dequeue(&pool->task_queue, &task_data);
        
        pthread_mutex_unlock(&pool->mutex);
        
        if (dequeued) {
            Task *task = (Task *)task_data;
            task->func(task->arg);
            free(task);
        }
    }
    
    return NULL;
}

t_thread_pool *thread_pool_create(int num_threads) {
    t_thread_pool *pool = (t_thread_pool *)malloc(sizeof(t_thread_pool));
    if (pool == NULL) {
        return NULL;
    }
    
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
    if (pool->threads == NULL) {
        free(pool);
        return NULL;
    }
    
    pool->num_threads = num_threads;
    pool->shutdown = false;
    
    safe_queue_init(&pool->task_queue);
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->condition, NULL);
    
    return pool;
}

void thread_pool_init(t_thread_pool *pool) {
    for (int i = 0; i < pool->num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, thread_worker, pool);
    }
}

void thread_pool_submit(t_thread_pool *pool, t_task_fn func, void *arg) {
    t_task *task = (t_task *)malloc(sizeof(t_task));
    task->function = func;
    task->arg = arg;
    
    safe_queue_enqueue(&pool->task_queue, task);
    pthread_cond_signal(&pool->cond);
}

void thread_pool_shutdown(t_thread_pool *pool) {
    pthread_mutex_lock(&pool->mutex);
    pool->shutdown = true;
    pthread_mutex_unlock(&pool->mutex);
    
    pthread_cond_broadcast(&pool->cond);
    
    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
}

void thread_pool_destroy(t_thread_pool *pool) {
    safe_queue_destroy(&pool->task_queue);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
    free(pool->threads);
    free(pool);
}
