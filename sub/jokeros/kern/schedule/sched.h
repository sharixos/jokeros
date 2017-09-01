#ifndef __KERN_SCHEDULE_SCHED_H__
#define __KERN_SCHEDULE_SCHED_H__

#include <types.h>
#include <list.h>
#include <proc.h>

typedef struct {
    unsigned int expires;			// when it is time
    struct proc_struct *proc;
    list_entry_t timer_link;
} timer_t;

#define le2timer(le, member)            \
	to_struct((le), timer_t, member)

static inline timer_t *
timer_init(timer_t *timer, struct proc_struct *proc, int expires) {
    timer->expires = expires;
    timer->proc = proc;
    list_init(&(timer->timer_link));
    return timer;
}

struct run_queue;

// The introduction of scheduling classes is borrowed from linux, and makes the 
// core scheduler quite extensible. These classes (the scheduler modules) encapsulate
// the scheduling policies
struct sched_class {
	// the name of sched_class
	const char *name;
	// Init the run queue
	void (*init)(struct run_queue *rq);
	// put the proc into runqueue, and this function must be called with rq_lock
	void (*enqueue)(struct run_queue *rq, struct proc_struct *proc);
	// get the proc out runqueue, and this function must be called with rq_lock
	void (*dequeue)(struct run_queue *rq, struct proc_struct *proc);
	// choose the next runnable task
	struct proc_struct *(*pick_next)(struct run_queue *rq);
	// dealer of the time-tick
	void (*proc_tick)(struct run_queue *rq, struct proc_struct *proc);
	/* for SMP support in the future
	*	load_balance
	*	void (*load_balance)(struct run_queue *rq);
	*	get some proc from this rq, used in load_balance,
	*	return value is the num of gotten proc
	* 	int (*get_proc)(struct run_queue *rq, struct proc_struct *procs_moved[]);
	*/
};

#define le2rq(le, member)			\
	to_struct((le), struct run_queue, member)

struct run_queue {
	list_entry_t run_list;
	unsigned int proc_num;
	int max_time_slice;
	list_entry_t rq_link;
};

void sched_init(void);
void wakeup_proc(struct proc_struct *proc);
void schedule(void);
void add_timer(timer_t *timer);
void del_timer(timer_t *timer);
void run_timer_list(void);

#endif /* !__KERN_SCHEDULE_SCHED_H__ */
