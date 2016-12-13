1. OSEK defines three processing levels (from highest to lowest priority ):
		• Interrupt level
		• Logical level for scheduler
		• Task level

Within the task level tasks are scheduled according to their user assigned priority.
Priority rules :
		• Interrupts precede tasks
		• The interrupt processing level consists of one or more interrupt priority levels
		• Interrupt service routines have a statically assigned interrupt priority level
		• Assignment of interrupt service routines to interrupt priority levels is dependent on 
OSEK/VDX Operating System
		• For task priorities and resource ceiling-priorities-bigger numbers mean higher priorities
		• The task’s priority is statically assigned by the user 

2. There are four conformance classes in OSEK:
	• BCC1 (only basic tasks, limited to one activation request per task and one task per priority, all tasks have different priorities)
	• BCC2 (like BCC1, plus more than one task per priority possible and multiple requesting of task activation allowed)
	• ECC1 (like BCC1, plus extended tasks)
	• ECC2 (like ECC1, plus more than one task per priority possible and multiple requesting of task activation allowed for basic tasks) 
	
3. Basic tasks only release the processor, if
	• they terminate,
	• the OSEK operating system switches to a higher-priority task, or
	• an interrupt occurs which causes the processor to switch to an interrupt service routine(ISR)

Extended tasks are distinguished from basic tasks by being allowed to use the operating system call WaitEvent, which may result in a waiting state.
The waiting state allows the processor to be released and to
be reassigned to a lower-priority task without the need to terminate the running extended task. 
In view of the operating system, management of extended tasks is more complex than management of basic tasks and requires more system resources.
The state model of basic tasks is nearly identical to the extended tasks state model. The only exception is that basic tasks do not have a waiting state

4. Scheduling policies in OSEK
 “Full preemptive”: All tasks are preemptable
	• It is the most reactive model because any task may be preempted. The highest priority Task is sure to get the CPU as soon as it is activated.

 “Full non preemptive”: All tasks are non-preemptable.
	• It is the most predictive model because a task which get the CPU will never be preempted. Scheduling is a straightforward and the OS memory
	footprint may be smaller.

 “Mixed”: Each task may be configured as preemptable or non-preemptable.
	• It is the most flexible model.
	• For instance, a very short task (in execution time) may be configured as non-preemptable because the context switch is longer than its execution.

5.Difference between category 1 and category 2 interrupts :
 Category 1 interrupts:
	• are very fast
	• stick to the hardware capabilities of the micro-controller
	• are not allowed to do a system call
	• usually difficult to port to another micro-controller

 Category 2 interrupts:
	• are not as fast as level 1 interrupts
	• are allowed to do some system calls (activate a task, get a resource, ...)