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

    /* ALARM definition */
    #define EE_MAX_ALARM 0U

    /* SCHEDULING TABLE definition */
    #define EE_MAX_SCHEDULETABLE 0U

    /* COUNTER OBJECTS definition */
    #define EE_MAX_COUNTER_OBJECTS (EE_MAX_ALARM + EE_MAX_SCHEDULETABLE)

    /* CPU CLOCK definition */
    #define EE_CPU_CLOCK      300000000U

    /* COUNTER definition */
    #define EE_MAX_COUNTER 0U

    /* APPMODE definition */
    #define EE_MAX_APPMODE 1U

    /* CPUs */
    #define EE_MAX_CPU 1
    #define EE_CURRENTCPU 0

    /* Number of isr 2 */
    #define EE_MAX_ISR2   0
    #define EE_MAX_ISR_ID 0

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
#define __OO_ECC1__
#define __OO_EXTENDED_STATUS__
#define __IRQ_STACK_NEEDED__
#define __MULTI__
#define __OO_NO_ALARMS__
#define __OO_NO_RESOURCES__
#define __OO_NO_EVENTS__
#define __OO_AUTOSTART_TASK__
#define __ALLOW_NESTED_IRQ__

#endif

    /* System stack size */
    #define EE_SYS_STACK_SIZE     8192



/***************************************************************************
 *
 * Vector size defines
 *
 **************************************************************************/
    #define EE_OO_AUTOSTART_TASK_MODE_OSDEFAULTAPPMODE_SIZE 2
    #define EE_TC_SYSTEM_TOS_SIZE 4


#endif

