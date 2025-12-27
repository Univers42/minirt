/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_queue.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 17:36:06 by marvin            #+#    #+#             */
/*   Updated: 2025/12/27 17:36:06 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "safe_queue.h"

void safe_queue_init(t_safe_queue *queue) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->mutex, NULL);
}

void safe_queue_destroy(SafeQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    
    t_queue_node *current = queue->front;
    while (current != NULL) {
        t_queue_node *temp = current;
        current = current->next;
        free(temp);
    }
    
    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);
}

bool safe_queue_empty(t_safe_queue *queue) {
    pthread_mutex_lock(&queue->mutex);
    bool empty = (queue->size == 0);
    pthread_mutex_unlock(&queue->mutex);
    return empty;
}

int safe_queue_size(t_safe_queue *queue) {
    pthread_mutex_lock(&queue->mutex);
    int size = queue->size;
    pthread_mutex_unlock(&queue->mutex);
    return size;
}

void safe_queue_enqueue(t_safe_queue *queue, void *data) {
    t_queue_node *new_node = (t_queue_node *)malloc(sizeof(t_queue_node));
    new_node->data = data;
    new_node->next = NULL;
    
    pthread_mutex_lock(&queue->mutex);
    
    if (queue->rear == NULL) {
        queue->front = queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    queue->size++;
    
    pthread_mutex_unlock(&queue->mutex);
}

bool safe_queue_dequeue(t_safe_queue *queue, void **data) {
    pthread_mutex_lock(&queue->mutex);
    
    if (queue->front == NULL) {
        pthread_mutex_unlock(&queue->mutex);
        return false;
    }
    t_queue_node *temp = queue->front;
    *data = temp->data;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    queue->size--;
    free(temp); 
    pthread_mutex_unlock(&queue->mutex);
    return true;
}
