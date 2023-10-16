#ifndef THREADS_H_
#define THREADS_H_

// TODO: necessary includes, if any
#include <stdio.h>
#include <setjmp.h>
#include <sys/mman.h>
// TODO: necessary defines, if any

// TODO: definition of struct thread
struct thread{
	void (*function) (void *);
	void* args;
	struct thread* next;
	void* stack_ptr;
	void* base_ptr;
	
};

struct thread *thread_create(void (*f)(void *), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);

#endif // THREADS_H_
