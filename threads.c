// TODO: necessary includes, if any
#include <stdio.h>
#include <setjmp.h>
#include <sys/mman.h>
#include <stdlib.h>

// TODO: necessary defines, if any

// TODO: definition of struct thread
struct thread{
	void (*function) (void *);
	void* args;
	struct thread* next;
	void* stack_ptr;
	void* base_ptr;
    jmp_buf env;
    int running;
};

struct thread* current = NULL;
struct thread* last = NULL;

void thread_exit(void);

struct thread *thread_create(void (*f)(void *), void *arg){
    struct thread* thread = (struct thread*) malloc(sizeof(struct thread));
    if(thread == NULL){
        exit(1);
    }
	thread->function = f;
	thread -> args = arg;
	thread->stack_ptr = mmap(NULL, 8388608, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_GROWSDOWN, -1, 0);
	thread->base_ptr = thread->stack_ptr;
    thread->running = 0;
	return thread;
}

void thread_add_runqueue(struct thread *t){
    if(current == NULL){
        current = t;
        current->next = current;
    } 
    else{
        struct thread* curr = current;
        for(;;){
            if(curr->next == current){
                curr->next = t;
                t->next = current;
                break;
            }else{
                curr = curr->next;
            } 
        }
    }
}

void dispatch(void){
    __asm__ ("movq %%rsp, %0\n\t movq %%rbp, %1"
            : "=rm" (last->stack_ptr), "=rm" (last->base_ptr)
            :
            :
    );
    if(!current->running){
        __asm__ ("movq %0, %%rsp\n\t moveq %1, %%rbp"
                : 
                : "rm" (current->stack_ptr), "rm" (current->base_ptr)
                : "rsp", "rbp"
        );
    }
    else if(current->running && current->stack_ptr != current->base_ptr){
        longjmp(current->env, 1);
    }else{
        thread_exit();
    }
}

void schedule(void){
    last = current;
    current = current->next;
}

void thread_yield(void){
    if(setjmp(current->env)) return;
    schedule();
    dispatch();
}   


void thread_exit(void){
    if(munmap(current->stack_ptr, 8388608)){
        exit(1);
    }else{
        if(current->next == current){
            free(current);
            return;
        }else{
            struct thread* next = current->next;
            free(current);
            current = next;
            last->next = next;
            dispatch();
        }

    }
}

void thread_start_threading(void){
    schedule();
    dispatch();
}

