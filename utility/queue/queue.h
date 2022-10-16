
/**
 * @file queue.h
 *
 * @copyright This file is a part of the project hivelang and is distributed under MIT license that
 * should have been included with the project. If not, see https://choosealicense.com/licenses/mit/
 *
 * @author jorisb
 *
 * @date 2022-09-26
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

struct QNode
{
	void* data;
	struct QNode* next;
};

struct Queue
{
	struct QNode* front;
	struct QNode* back;
	signed long long count;
};

struct Queue Queue_create(
	void);

void Queue_destroy(
	struct Queue* const queue);

void Queue_enqueue(
	struct Queue* const queue,
	void* data);

void* Queue_dequeue(
	struct Queue* const queue);

void* Queue_peek(
	struct Queue* const queue);

#endif
