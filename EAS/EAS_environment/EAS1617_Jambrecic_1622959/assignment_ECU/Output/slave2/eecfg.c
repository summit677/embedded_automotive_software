#include "ee.h"



/***************************************************************************
 *
 * Stack definition for TriCore
 *
 **************************************************************************/
    #define STACK_1_SIZE 512 /* size = 512 bytes */
    #define STACK_2_SIZE 512 /* size = 512 bytes */

    EE_STACK_T EE_STACK_ATTRIB EE_tc_stack_1[EE_STACK_WLEN(STACK_1_SIZE)] EE_TC_FILL_STACK(EE_tc_stack_1);	/* Task 0 (TaskPwmM) */
    EE_STACK_T EE_STACK_ATTRIB EE_tc_stack_2[EE_STACK_WLEN(STACK_2_SIZE)] EE_TC_FILL_STACK(EE_tc_stack_2);	/* Task 1 (TaskPinM) */

    const EE_UREG EE_std_thread_tos[EE_MAX_TASK+1] = {
        0U,	/* dummy */
        1U,	/* TaskPwmM */
        2U 	/* TaskPinM */
    };

    struct EE_TC_TOS EE_tc_system_tos[EE_TC_SYSTEM_TOS_SIZE] = {
        {0, 0U},	/* Task   (dummy) */
        {EE_STACK_INITP(EE_tc_stack_1), 0U},	/* Task 0 (TaskPwmM) */
        {EE_STACK_INITP(EE_tc_stack_2), 0U} 	/* Task 1 (TaskPinM) */
    };

    EE_UREG EE_tc_active_tos = 0U; /* dummy */

    EE_tc_task_save_data EE_tc_tasks_RA[EE_MAX_TASK+1];



/***************************************************************************
 *
 * Kernel ( CPU 2 )
 *
 **************************************************************************/
    /* Definition of task's body */
    DeclareTask(TaskPwmM);
    DeclareTask(TaskPinM);

    const EE_THREAD_PTR EE_hal_thread_body[EE_MAX_TASK] = {
        &EE_oo_thread_stub,		 /* thread TaskPwmM */
        &EE_oo_thread_stub 		 /* thread TaskPinM */

    };

    EE_UINT32 EE_terminate_data[EE_MAX_TASK];

    /* ip of each thread body (ROM) */
    const EE_THREAD_PTR EE_terminate_real_th_body[EE_MAX_TASK] = {
        &FuncTaskPwmM,
        &FuncTaskPinM
    };
    /* ready priority */
    const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK] = {
        0x2U,		 /* thread TaskPwmM */
        0x1U 		 /* thread TaskPinM */
    };

    const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK] = {
        0x2U,		 /* thread TaskPwmM */
        0x1U 		 /* thread TaskPinM */
    };

    /* thread status */
    EE_TYPESTATUS EE_th_status[EE_MAX_TASK] = {
        SUSPENDED,
        SUSPENDED
    };

    /* next thread */
    EE_TID EE_th_next[EE_MAX_TASK] = {
        EE_NIL,
        EE_NIL
    };

    /* The first stacked task */
    EE_TID EE_stkfirst = EE_NIL;

    /* system ceiling */
    EE_TYPEPRIO EE_sys_ceiling= 0x0000U;

    /* remaining nact: init= maximum pending activations of a Task */
    /* MUST BE 1 for BCC1 and ECC1 */
    EE_TYPENACT   EE_th_rnact[EE_MAX_TASK] =
        { 1U, 1U};

    /* First task in the ready queue (initvalue = EE_NIL) */
    EE_TID EE_rq_first  = EE_NIL;

    #ifndef __OO_NO_CHAINTASK__
        /* The next task to be activated after a ChainTask. initvalue=all EE_NIL */
        EE_TID EE_th_terminate_nextask[EE_MAX_TASK] = {
            EE_NIL,
            EE_NIL
        };
    #endif



/***************************************************************************
 *
 * Event handling
 *
 **************************************************************************/
    EE_TYPEEVENTMASK EE_th_event_active[EE_MAX_TASK] =
        { 0U, 0U};    /* thread event already active */

    EE_TYPEEVENTMASK EE_th_event_waitmask[EE_MAX_TASK] =
        { 0U, 0U};    /* thread wait mask */

    EE_TYPEBOOL EE_th_waswaiting[EE_MAX_TASK] =
        { 0U, 0U};

    const EE_TYPEPRIO EE_th_is_extended[EE_MAX_TASK] =
        { 0U, 0U};

    /*
     * array to hold corresponding isr2 nesting levels.
    */
    EE_UREG EE_isr2_nesting_level[EE_MAX_ISR2];



/***************************************************************************
 *
 * AppMode
 *
 **************************************************************************/
    EE_TYPEAPPMODE EE_ApplicationMode;


/***************************************************************************
 *
 * Auto Start (TASK)
 *
 **************************************************************************/
    static const EE_TID EE_oo_autostart_task_mode_OSDEFAULTAPPMODE[EE_OO_AUTOSTART_TASK_MODE_OSDEFAULTAPPMODE_SIZE] = 
        { TaskPinM };

    const struct EE_oo_autostart_task_type EE_oo_autostart_task_data[EE_MAX_APPMODE] = {
        { 1U, EE_oo_autostart_task_mode_OSDEFAULTAPPMODE}
    };



/***************************************************************************
 *
 * Spinlock
 *
 **************************************************************************/
    TaskType EE_as_spinlocks_locker_task_or_isr2[EE_MAX_TASK + EE_MAX_ISR2] = {
        EE_NIL, EE_NIL
    };

