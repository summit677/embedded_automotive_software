#ifndef RTDH_EECFG_H
#define RTDH_EECFG_H


#define RTDRUID_CONFIGURATOR_NUMBER 1278



/***************************************************************************
 *
 * Common defines ( CPU 0 )
 *
 **************************************************************************/

    /* TASK definition */
    #define EE_MAX_TASK 3
    #define Task_Event 0
    #define Task_Alarm 1
    #define Task_Mix 2

    /* MUTEX definition */
    #define EE_MAX_RESOURCE 0U

    /* EVENT definition */
    #define Event1 0x1U
    #define Event_Mix 0x2U

    /* ALARM definition */
    #define EE_MAX_ALARM 2U
    #define Alarm 0U
    #define Alarm_Mix 1U

    /* SCHEDULING TABLE definition */
    #define EE_MAX_SCHEDULETABLE 0U

    /* COUNTER OBJECTS definition */
    #define EE_MAX_COUNTER_OBJECTS (EE_MAX_ALARM + EE_MAX_SCHEDULETABLE)

    /* CPU CLOCK definition */
    #define EE_CPU_CLOCK      300000000U

    /* COUNTER definition */
    #define EE_MAX_COUNTER 1U
    #define system_counter 0U

    /* APPMODE definition */
    #define EE_MAX_APPMODE 1U

    /* CPUs */
    #define EE_MAX_CPU 1
    #define EE_CURRENTCPU 0

    /* Number of isr 2 */
    #define EE_MAX_ISR2   2
    #define EE_MAX_ISR_ID 3

#ifndef __DISABLE_EEOPT_DEFINES__


/***************************************************************************
 *
 * User options
 *
 **************************************************************************/
#define __ASSERT__
#define EE_EXECUTE_FROM_RAM
#define EE_BUILD_SINGLE_ELF
#define EE_USE_DISPLAY
#define EE_USE_LEDS
#define EE_USE_BUTTONS


/***************************************************************************
 *
 * Automatic options
 *
 **************************************************************************/
#define __RTD_CYGWIN__
#define EE_APPKIT_TC2X7
#define EE_TRICORE__
#define EE_TC29X__
#define EE_GNU__
#define ENABLE_SYSTEM_TIMER
#define __OO_ECC1__
#define __OO_EXTENDED_STATUS__
#define __IRQ_STACK_NEEDED__
#define __MULTI__
#define __OO_NO_RESOURCES__
#define __OO_AUTOSTART_TASK__
#define __OO_AUTOSTART_ALARM__
#define __ALLOW_NESTED_IRQ__

#endif

    /* System stack size */
    #define EE_SYS_STACK_SIZE     8192



/***************************************************************************
 *
 * ISR definition
 *
 **************************************************************************/
#define EE_TC_2_ISR button_isr
#define EE_TC_2_ISR_PRI EE_ISR_PRI_2
#define EE_TC_2_ISR_CAT 2
#define EE_ISR2_ID_button_isr 2


/***************************************************************************
 *
 * HW counter definition
 *
 **************************************************************************/
#define EE_MAX_COUNTER_HW 1
#define EE_SYSTEM_TIMER   system_counter
#define EE_SYSTEM_TIMER_DEVICE   EE_TC_STM_SR0

#define EE_TC_1_ISR EE_tc_system_timer_handler
#define EE_TC_1_ISR_PRI EE_ISR_PRI_1
#define EE_TC_1_ISR_CAT 2
#define EE_ISR2_ID_EE_tc_system_timer_handler 1

/* Max ISR priority */
#define EE_TC_MAX_ISR_ID     EE_ISR_PRI_2



/***************************************************************************
 *
 * Counter defines
 *
 **************************************************************************/
#define OSMAXALLOWEDVALUE_system_counter 2147483647U
#define OSTICKSPERBASE_system_counter    1U
#define OSMINCYCLE_system_counter        1U

#define OSMAXALLOWEDVALUE 2147483647U
#define OSTICKSPERBASE    1U
#define OSMINCYCLE        1U
#define OSTICKDURATION    1000000U



/***************************************************************************
 *
 * Vector size defines
 *
 **************************************************************************/
    #define EE_ACTION_ROM_SIZE 2
    #define EE_ALARM_ROM_SIZE 2
    #define EE_COUNTER_HW_ROM_SIZE 1
    #define EE_COUNTER_OBJECTS_ROM_SIZE 2
    #define EE_COUNTER_ROM_SIZE 1
    #define EE_OO_AUTOSTART_ALARM_MODE_OSDEFAULTAPPMODE_SIZE 2
    #define EE_OO_AUTOSTART_TASK_MODE_OSDEFAULTAPPMODE_SIZE 2
    #define EE_TC_SYSTEM_TOS_SIZE 4


#endif

