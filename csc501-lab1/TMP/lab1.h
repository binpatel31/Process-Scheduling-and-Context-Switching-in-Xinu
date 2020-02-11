/* Practical 1 -- OS Constant */
#define RANDOMSCHED 1
#define LINUXSCHED 2

extern void setschedclass(int sched_class);
extern int getschedclass();
extern int schedule_class;
