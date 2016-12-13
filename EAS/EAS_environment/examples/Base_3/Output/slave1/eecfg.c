#include "ee.h"



/***************************************************************************
 *
 * Stack definition for TriCore
 *
 **************************************************************************/
    #define STACK_1_SIZE 512 /* size = 512 bytes */

    EE_STACK_T EE_STACK_ATTRIB EE_tc_stack_1[EE_STACK_WLEN(STACK_1_SIZE)] EE_TC_FILL_STACK(EE_tc_stack_1);	/* Task 0 (TaskSlave1) */

    const EE_UREG EE_std_thread_tos[EE_MAX_TASK+1] = {
        0U,	/* dummy */
        1U 	/* TaskSlave1 */
    };

    struct EE_TC_TOS EE_tc_system_tos[EE_TC_SYSTEM_TOS_SIZE] = {
        {0, 0U},	/* Task   (dummy) */
        {EE_STACK_INITP(EE_tc_stack_1), 0U} 	/* Task 0 (TaskSlave1) */
    };

    EE_UREG EE_tc_active_tos = 0U; /* dummy */

    EE_tc_task_save_data EE_tc_tasks_RA[EE_MAX_TASK+1];



/***************************************************************************
 *
 * Kernel ( CPU 1 )
 *
 **************************************************************************/
    /* Definition of task's body */
    DeclareTask(TaskSlave1);

    const EE_THREAD_PTR EE_hal_thread_body[EE_MAX_TASK] = {
        &EE_oo_thread_stub 		 /* thread TaskSlave1 */

    };

    EE_UINT32 EE_terminate_data[EE_MAX_TASK];

    /* ip of each thread body (ROM) */
    const EE_THREAD_PTR EE_terminate_real_th_body[EE_MAX_TASK] = {
        &FuncTaskSlave1
    };
    /* ready priority */
    const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK] = {
        0x1U 		 /* thread TaskSlave1 */
    };

    const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK] = {
        0x1U 		 /* thread TaskSlave1 */
    };

    /* thread status */
    EE_TYPESTATUS EE_th_status[EE_MAX_TASK] = {
        SUSPENDED
    };

    /* next thread */
    EE_TID EE_th_next[EE_MAX_TASK] = {
        EE_NIL
    };

    /* The first stacked task */
    EE_TID EE_stkfirst = EE_NIL;

    /* system ceiling */
    EE_TYPEPRIO EE_sys_ceiling= 0x0000U;

    /* remaining nact: init= maximum pending activations of a Task */
    /* MUST BE 1 for BCC1 and ECC1 */
    EE_TYPENACT   EE_th_rnact[EE_MAX_TASK] =
        { 1U};

    /* First task in the ready queue (initvalue = EE_NIL) */
    EE_TID EE_rq_first  = EE_NIL;

    #ifndef __OO_NO_CHAINTASK__
        /* The next task to be activated after a ChainTask. initvalue=all EE_NIL */
        EE_TID EE_th_terminate_nextask[EE_MAX_TASK] = {
            EE_NIL
        };
    #endif



/***************************************************************************
 *
 * Event handling
 *
 **************************************************************************/
    EE_TYPEEVENTMASK EE_th_event_active[EE_MAX_TASK] =
        { 0U};    /* thread event already active */

    EE_TYPEEVENTMASK EE_th_event_waitmask[EE_MAX_TASK] =
        { 0U};    /* thread wait mask */

    EE_TYPEBOOL EE_th_waswaiting[EE_MAX_TASK] =
        { 0U};

    const EE_TYPEPRIO EE_th_is_extended[EE_MAX_TASK] =
        { 0U};



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
        { TaskSlave1 };

    const struct EE_oo_autostart_task_type EE_oo_autostart_task_data[EE_MAX_APPMODE] = {
        { 1U, EE_oo_autostart_task_mode_OSDEFAULTAPPMODE}
    };

