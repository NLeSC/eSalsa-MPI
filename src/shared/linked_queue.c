#include <stdlib.h>
#include "linked_queue.h"

static inline linked_queue_node *linked_queue_get_node(linked_queue *queue)
{
	// NOTE: assumption is that queue is locked!
	linked_queue_node *tmp;

	if (queue->node_cache_size == 0) {
		return malloc(sizeof(linked_queue_node));
	}

	tmp = queue->node_cache;
	queue->node_cache = tmp->next;
	queue->node_cache_size--;
	tmp->next = NULL;

	return tmp;
}

static inline void linked_queue_put_node(linked_queue *queue, linked_queue_node *node)
{
	// NOTE: assumption is that queue is locked!
	if (queue->node_cache_size >= queue->max_node_cache_size) {
		free(node);
		return;
	}

	node->next = queue->node_cache;
	node->data = NULL;
	node->size = 0;

	queue->node_cache = node;
	queue->node_cache_size++;
}

linked_queue *linked_queue_create(int max_node_cache_size)
{
	linked_queue *tmp = malloc(sizeof(linked_queue));

	if (tmp == NULL) {
		return NULL;
	}

	tmp->head = NULL;
	tmp->tail = NULL;
	tmp->length = 0;
	tmp->data_size = 0;

	tmp->node_cache = NULL;
	tmp->node_cache_size = 0;

	if (max_node_cache_size < 0) {
		tmp->max_node_cache_size = DEFAULT_NODE_CACHE_SIZE;
	} else {
		tmp->max_node_cache_size = max_node_cache_size;
	}

	return tmp;
}

bool linked_queue_destroy(linked_queue *queue)
{
	linked_queue_node *tmp;

	if (queue == NULL) {
		// We need a queue to destroy!
		return false;
	}

	if (queue->length != 0) {
		// We can only destroy an empty queue!
		return false;
	}

	while (queue->node_cache != NULL) {
		tmp = queue->node_cache;
		queue->node_cache = tmp->next;
		free(tmp);
	}

	free(queue);

	return true;
}

bool linked_queue_enqueue(linked_queue *queue, void *elt, size_t size)
{
	linked_queue_node *n = linked_queue_get_node(queue);

	if (n == NULL) {
		return false;
	}

	n->data = elt;
	n->size = size;

	if (queue->length == 0) {
		queue->head = queue->tail = n;
	} else {
		queue->tail->next = n;
		queue->tail = n;
	}

	// Update the queue stats.
	queue->length++;
	queue->data_size += size;

	return true;
}

void *linked_queue_dequeue(linked_queue *queue)
{
	linked_queue_node *current;
	void *elt;

	if (queue->head == NULL) {
		return NULL;
	}

	current = queue->head;
	elt = current->data;

	if (queue->head == queue->tail) {
		queue->head = queue->tail = NULL;
	} else {
		queue->head = queue->head->next;
	}

	queue->length--;
	queue->data_size -= current->size;

	// Store the list node for later use
	linked_queue_put_node(queue, current);

	return elt;
}

void *linked_queue_peek(linked_queue *queue)
{
	if (queue->head == NULL) {
		return NULL;
	} else {
		return queue->head->data;
	}
}

int linked_queue_bulk_dequeue(linked_queue *queue, void **buffer, int count)
{
	int stored = 0;

	while (queue->length > 0 && stored < count) {
		buffer[stored++] = linked_queue_dequeue(queue);
	}

	return stored;
}

bool linked_queue_isempty(linked_queue *queue)
{
	return (queue->length == 0);
}

int linked_queue_length(linked_queue *queue)
{
	return queue->length;
}

int linked_queue_data_size(linked_queue *queue)
{
	return queue->data_size;
}
