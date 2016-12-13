Remote notification internal documentation
------------------------------------------

This document was extracted from the document contained in the remote notification files.

Remote Notification
This library implements the remote notification procedure

Note that the implementation subsumes the Janus Architecture:
That is, interprocessor interrupts cannot be queued.

Symbols to be defined to enable remote notification:
__RN__           Remote Notification in general
__RN_COUNTER__   Counter RN
__RN_EVENT__     Event RN
__RN_TASK__      Task RN
__RN_FUNC__      Func RN
__RN_SEM__       Sem RN

Often it is useful to hide the fact that we are activating a remote
or a local task using something like "ActivateTask(mytask);", where
"mytask" is in general a symbol that refers to the task to activate
if it is local, or to a remote notification that executes the task
activation if it is remote. That feature is called "gateway".

The way I chose to implement the gateway is:

Local Tasks: local TID
Global Tasks: remote notification | EE_REMOTE_TID
Remote semaphores: a local semaphore must be created. s->first,
  that usually contains the first task that is blocked on the
  semaphore, will contain a remote notification number as used for
  Global tasks. (note that that value is != NIL)

Please note that in that way each remote task can have ONLY ONE
Gateway. This usually works because basic tasks do not use events, and
when using extended tasks the user can do both activations and event
setting

To reduce the time that the system runs with interrupt disabled and
spin lock used, I decided to use a switch mechanism. Basically, two
data structures exists. When a remote notification is sent, it is
queued in one unused data structure pointed by the switch
variable. When the interprocessor interrupt is raised, the two data
structures are switched, and the pending notifications are executed in
an environment that is equivalent to an ISR2. Thanks to the switch,
the spin lock is released, and the remote processor can continue to
queue its notifications, that will be executed at the next
interprocessor interrupt.

Constants to be defined by the application:
- EE_MAX_RN - the total number of remote notifications
- EE_MAX_CPU - the total number of CPUs

Types defined in this file that can be modified by the application
EE_TYPERN - a SIGNED integer - EE_INT8 should be enough in most cases
EE_TYPERN_SWITCH - a bit field with at least 3 bits
EE_TYPERN_NOTIFY - a bit field with at least 5 bits

These types should be required by other parts of the Kernel...
EE_UINT8
EE_TYPECOUNTER
EE_TYPEEVENTMASK
EE_TID
EE_ADDR
EE_SEM
EE_UREG
EE_TYPESPIN

