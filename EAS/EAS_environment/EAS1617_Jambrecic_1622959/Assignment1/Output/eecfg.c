#include "ee.h"



/***************************************************************************
 *
 * Stack definition for TriCore
 *
 **************************************************************************/
    #define STACK_1_SIZE 256 /* size = 256 bytes */
    #define STACK_2_SIZE 256 /* size = 256 bytes */
    #define STACK_3_SIZE 256 /* size = 256 bytes */
    #define STACK_4_SIZE 256 /* size = 256 bytes */

    EE_STACK_T EE_STACK_ATTRIB EE_tc_stack_1[EE_STACK_WLEN(STACK_1_SIZE)] EE_TC_FILL_STACK(EE_tc_stack_1);	/* Task 0 (Task_Event) */
    EE_STACK_T EE_STACK_ATTRIB EE_tc_stack_2[EE_STACK_WLEN(STACK_2_SIZE)] EE_TC_FILL_STACK(EE_tc_stack_2);	/* Task 1 (Task_Alarm) */
    EE_STACK_T EE_STACK_ATTRIB EE_tc_stack_3[EE_STACK_WLEN(STACK_3_SIZE)] EE_TC_FILL_STACK(EE_tc_stack_3);	/* Task 2 (Task_Mix) */
    EE_STACK_T EE_STACK_ATTRIB EE_tc_stack_4[EE_STACK_WLEN(STACK_4_SIZE)] EE_TC_FILL_STACK(EE_tc_stack_4);	/* irq stack */

    const EE_UREG EE_std_thread_tos[EE_MAX_TASK+1] = {
        0U,	/* dummy */
        1U,	/* Task_Event */
        2U,	/* Task_Alarm */
        3U 	/* Task_Mix */
    };

    struct EE_TC_TOS EE_tc_system_tos[EE_TC_SYSTEM_TOS_SIZE] = {
        {0, 0U},	/* Task   (dummy) */
        {EE_STACK_INITP(EE_tc_stack_1), 0U},	/* Task 0 (Task_Event) */
        {EE_STACK_INITP(EE_tc_stack_2), 0U},	/* Task 1 (Task_Alarm) */
        {EE_STACK_INITP(EE_tc_stack_3), 0U} 	/* Task 2 (Task_Mix) */
    };

    EE_UREG EE_tc_active_tos = 0U; /* dummy */

    EE_tc_task_save_data EE_tc_tasks_RA[EE_MAX_TASK+1];

    /* stack used only by IRQ handlers */
    struct EE_TOS EE_tc_IRQ_tos = {
        EE_STACK_INITP(EE_tc_stack_4)
    };



/***************************************************************************
 *
 * HW counter definition
 *
 **************************************************************************/
    const EE_oo_counter_hw_ROM_type EE_counter_hw_ROM[EE_COUNTER_HW_ROM_SIZE] = {
        {1000U}         /* system_counter */
    };



/***************************************************************************
 *
 * Kernel ( CPU 0 )
 *
 **************************************************************************/
    /* Definition of task's body */
    DeclareTask(Task_Event);
    DeclareTask(Task_Alarm);
    DeclareTask(Task_Mix);

    const EE_THREAD_PTR EE_hal_thread_body[EE_MAX_TASK] = {
        &EE_oo_thread_stub,		 /* thread Task_Event */
        &EE_oo_thread_stub,		 /* thread Task_Alarm */
        &EE_oo_thread_stub 		 /* thread Task_Mix */

    };

    EE_UINT32 EE_terminate_data[EE_MAX_TASK];

    /* ip of each thread body (ROM) */
    const EE_THREAD_PTR EE_terminate_real_th_body[EE_MAX_TASK] = {
        &FuncTask_Event,
        &FuncTask_Alarm,
        &FuncTask_Mix
    };
    /* ready priority */
    const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK] = {
        0x1U,		 /* thread Task_Event */
        0x2U,		 /* thread Task_Alarm */
        0x4U 		 /* thread Task_Mix */
    };

    const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK] = {
        0x1U,		 /* thread Task_Event */
        0x2U,		 /* thread Task_Alarm */
        0x4U 		 /* thread Task_Mix */
    };

    /* thread status */
    EE_TYPESTATUS EE_th_status[EE_MAX_TASK] = {
        SUSPENDED,
        SUSPENDED,
        SUSPENDED
    };

    /* next thread */
    EE_TID EE_th_next[EE_MAX_TASK] = {
        EE_NIL,
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
        { 1U, 1U, 1U};

    /* First task in the ready queue (initvalue = EE_NIL) */
    EE_TID EE_rq_first  = EE_NIL;

    #ifndef __OO_NO_CHAINTASK__
        /* The next task to be activated after a ChainTask. initvalue=all EE_NIL */
        EE_TID EE_th_terminate_nextask[EE_MAX_TASK] = {
            EE_NIL,
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
        { 0U, 0U, 0U};    /* thread event already active */

    EE_TYPEEVENTMASK EE_th_event_waitmask[EE_MAX_TASK] =
        { 0U, 0U, 0U};    /* thread wait mask */

    EE_TYPEBOOL EE_th_waswaiting[EE_MAX_TASK] =
        { 0U, 0U, 0U};

    const EE_TYPEPRIO EE_th_is_extended[EE_MAX_TASK] =
        { 1U, 0U, 1U};



/***************************************************************************
 *
 * Counters
 *
 **************************************************************************/
    const EE_oo_counter_ROM_type EE_counter_ROM[EE_COUNTER_ROM_SIZE] = {
        {2147483647U, 1U, 1U}         /* system_counter */
    };

    EE_oo_counter_RAM_type       EE_counter_RAM[EE_MAX_COUNTER] = {
        {0U, (EE_TYPECOUNTEROBJECT)-1}
    };


/***************************************************************************
 *
 * Alarms
 *
 **************************************************************************/
    const EE_oo_alarm_ROM_type EE_alarm_ROM[EE_ALARM_ROM_SIZE] = {
        {0U},
        {1U}
    };


/***************************************************************************
 *
 * Counter Objects
 *
 **************************************************************************/
    const EE_oo_counter_object_ROM_type   EE_oo_counter_object_ROM[EE_COUNTER_OBJECTS_ROM_SIZE] = {

        {system_counter, Alarm, EE_ALARM },
        {system_counter, Alarm_Mix, EE_ALARM }
    };

    EE_oo_counter_object_RAM_type EE_oo_counter_object_RAM[EE_COUNTER_OBJECTS_ROM_SIZE];


/***************************************************************************
 *
 * Alarms and Scheduling Tables actions
 *
 **************************************************************************/
    const EE_oo_action_ROM_type   EE_oo_action_ROM[EE_ACTION_ROM_SIZE] = {

        {EE_ACTION_TASK    , Task_Alarm, 0U, (EE_VOID_CALLBACK)NULL, (EE_TYPECOUNTER)-1 },
        {EE_ACTION_EVENT   , Task_Mix, Event_Mix, (EE_VOID_CALLBACK)NULL, (EE_TYPECOUNTER)-1 }
    };



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
        { Task_Event, Task_Mix };

    const struct EE_oo_autostart_task_type EE_oo_autostart_task_data[EE_MAX_APPMODE] = {
        { 2U, EE_oo_autostart_task_mode_OSDEFAULTAPPMODE}
    };


/***************************************************************************
 *
 * Auto Start (ALARM)
 *
 **************************************************************************/
    static const EE_TYPEALARM EE_oo_autostart_alarm_mode_OSDEFAULTAPPMODE[EE_OO_AUTOSTART_ALARM_MODE_OSDEFAULTAPPMODE_SIZE] =
        { Alarm, Alarm_Mix };

    const struct EE_oo_autostart_alarm_type EE_oo_autostart_alarm_data[EE_MAX_APPMODE] = {
        { 2U, EE_oo_autostart_alarm_mode_OSDEFAULTAPPMODE}
    };


/***************************************************************************
 *
 * Init alarms parameters (ALARM)
 *
 **************************************************************************/

    const EE_TYPETICK EE_oo_autostart_alarm_increment[EE_MAX_ALARM] =
        {500U, 1U };

    const EE_TYPETICK EE_oo_autostart_alarm_cycle[EE_MAX_ALARM] =
        {500U, 500U };

