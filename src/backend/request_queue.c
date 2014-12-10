#include <stdlib.h>
#include "request_queue.h"
#include "empi.h"

static inline request_queue_node *request_queue_get_node(request_queue *queue)
{
	// NOTE: assumption is that queue is locked!
	request_queue_node *tmp;

	if (queue->node_cache_size == 0) {
		return malloc(sizeof(request_queue_node));
	}

	tmp = queue->node_cache;
	queue->node_cache = tmp->next;
	queue->node_cache_size--;
	tmp->next = NULL;

	return tmp;
}

static inline void request_queue_put_node(request_queue *queue, request_queue_node *node)
{
	// NOTE: assumption is that queue is locked!
	if (queue->node_cache_size >= queue->max_node_cache_size) {
		free(node);
		return;
	}

	node->next = queue->node_cache;
	node->data = NULL;

	queue->node_cache = node;
	queue->node_cache_size++;
}

request_queue *request_queue_create(int max_node_cache_size)
{
	request_queue *tmp = malloc(sizeof(request_queue));

	if (tmp == NULL) {
		return NULL;
	}

	tmp->head = NULL;
	tmp->tail = NULL;
//	tmp->length = 0;

	tmp->node_cache = NULL;
	tmp->node_cache_size = 0;

	if (max_node_cache_size < 0) {
		tmp->max_node_cache_size = DEFAULT_NODE_CACHE_SIZE;
	} else {
		tmp->max_node_cache_size = max_node_cache_size;
	}

	return tmp;
}

bool request_queue_destroy(request_queue *queue)
{
	request_queue_node *tmp;

	if (queue == NULL) {
		// We need a queue to destroy!
		return false;
	}

	if (queue->head != NULL) {
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

bool request_queue_enqueue(request_queue *queue, request *elt)
{
	request_queue_node *n = request_queue_get_node(queue);

	if (n == NULL) {
		return false;
	}

	n->data = elt;

	if (queue->head == NULL) {
		queue->head = queue->tail = n;
	} else {
		queue->tail->next = n;
		queue->tail = n;
	}

	return true;
}

request *request_queue_dequeue(request_queue *queue)
{
	request_queue_node *current;
	request *elt;

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

	// Store the list node for later use
	request_queue_put_node(queue, current);

	return elt;
}

request *request_queue_dequeue_matching(request_queue *queue, int comm, int source, int tag)
{
	request_queue_node *current;
	request_queue_node *prev;
	request *elt;

	if (queue->head == NULL) {
		return NULL;
	}

	current = queue->head;
	prev = NULL;

	while (current != NULL) {

		elt = current->data;

		if ((source == EMPI_ANY_SOURCE || source == elt->source_or_dest) &&
		   	(tag == EMPI_ANY_TAG || tag == elt->tag) &&
			(comm == elt->c->handle)) {

			// We've found a matching request!
			if (current == queue->head) {
				// delete head. check if list is empty afterwards
				if (queue->head == queue->tail) {
					queue->head = queue->tail = NULL;
				} else {
					queue->head = queue->head->next;
				}
			} else if (current == queue->tail) {
				// delete tail. set tail to prev
				queue->tail = prev;
				queue->tail->next = NULL;
			} else {
				// delete middle.
				prev->next = current->next;
			}

			request_queue_put_node(queue, current);
			return elt;
		}

		prev = current;
		current = current->next;
	}

	return NULL;
}

/*
request *request_queue_peek(request_queue *queue)
{
	if (queue->head == NULL) {
		return NULL;
	} else {
		return queue->head->data;
	}
}

int request_queue_length(request_queue *queue)
{
	return queue->length;
}
*/
