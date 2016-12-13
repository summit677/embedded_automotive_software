#ifndef RTDH_EECFG_H
#define RTDH_EECFG_H


#define RTDRUID_CONFIGURATOR_NUMBER 1278



/***************************************************************************
 *
 * Common defines ( CPU 0 )
 *
 **************************************************************************/

    /* TASK definition */
    #define EE_MAX_TASK 1
    #define TaskUi 0

    /* MUTEX definition */
    #define EE_MAX_RESOURCE 0U

    /* EVENT definition */
    #define event 0x1U

    /* ALARM definition */
    #define EE_MAX_ALARM 2U
    #define LCDUpdated 0U
    #define AlarmPinMonitor 1U

    /* SCHEDULING TABLE definition */
    #define EE_MAX_SCHEDULETABLE 0U

    /* COUNTER OBJECTS definition */
    #define EE_MAX_COUNTER_OBJECTS (EE_MAX_ALARM + EE_MAX_SCHEDULETABLE)

    /* CPU CLOCK definition */
    #define EE_CPU_CLOCK      200000000U

    /* COUNTER definition */
    #define EE_MAX_COUNTER 1U
    #define system_timer 0U

    /* APPMODE definition */
    #define EE_MAX_APPMODE 1U

    /* CPUs */
    #define EE_MAX_CPU 3
    #define EE_CURRENTCPU 0

    /* Number of isr 2 */
    /* Remote procedure call requires an additional ISR2 */
    #define EE_MAX_ISR2   4
    #define EE_MAX_ISR_ID 3

#ifndef __DISABLE_EEOPT_DEFINES__


/***************************************************************************
 *
 * User options
 *
 **************************************************************************/
#define __ASSERT__
#define EE_EXECUTE_FROM_RAM
#define EE_ICACHE_ENABLED
#define EE_DCACHE_ENABLED
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
#define __MSRP__
#define EE_APPKIT_TC2X7
#define EE_TRICORE__
#define EE_TC29X__
#define EE_BUILD_SINGLE_ELF
#define EE_GNU__
#define EE_START_CPU1
#define EE_START_CPU2
#define ENABLE_SYSTEM_TIMER
#define __OO_ORTI_LASTERROR__
#define __OO_ORTI_SERVICETRACE__
#define __OO_ORTI_PRIORITY__
#define __OO_ORTI_RES_LOCKER_TASK__
#define __OO_ORTI_RES_ISLOCKED__
#define __OO_ORTI_STACK__
#define __OO_ORTI_ALARMTIME__
#define __OO_ORTI_RUNNINGISR2__
#define EE_AS_RPC__
#define EE_AS_USER_SPINLOCKS__
#define __OO_ECC1__
#define __OO_EXTENDED_STATUS__
#define __OO_HAS_STARTUPHOOK__
#define __OO_HAS_SHUTDOWNHOOK__
#define __MULTI__
#define __OO_NO_RESOURCES__
#define __OO_AUTOSTART_ALARM__
#define __ALLOW_NESTED_IRQ__

#endif



/***************************************************************************
 *
 * Remote tasks
 *
 **************************************************************************/
    #define TaskPwm ((EE_TID)1U + (EE_TID)EE_REMOTE_TID)
    #define TaskPin ((EE_TID)2U + (EE_TID)EE_REMOTE_TID)
    #define TaskPwmM ((EE_TID)3U + (EE_TID)EE_REMOTE_TID)
    #define TaskPinM ((EE_TID)4U + (EE_TID)EE_REMOTE_TID)


/***************************************************************************
 *
 * Remote alarms
 *
 **************************************************************************/


/***************************************************************************
 *
 * Remote counters
 *
 **************************************************************************/


/***************************************************************************
 *
 * Remote OsApplication
 *
 **************************************************************************/


/***************************************************************************
 *
 * Remote Schedule Tables
 *
 **************************************************************************/


/***************************************************************************
 *
 * Spin lock IDs
 *
 **************************************************************************/
    #define EE_MAX_SPINLOCK_USER 1

    #define EE_SPINLOCK_CORE0 3U	 /* master */
    #define EE_SPINLOCK_CORE1 2U	 /* slave1 */
    #define EE_SPINLOCK_CORE2 1U	 /* slave2 */
    #define s 0U
    #define EE_MAX_SPINLOCK 4
    #define EE_MAX_SPINLOCK_OS 4


    /* System stack size */
    #define EE_SYS_STACK_SIZE     4096



/***************************************************************************
 *
 * HW counter definition
 *
 **************************************************************************/
#define EE_MAX_COUNTER_HW 1
#define EE_SYSTEM_TIMER   system_timer
#define EE_SYSTEM_TIMER_DEVICE   EE_TC_STM_SR0

#define EE_TC_2_ISR EE_tc_system_timer_handler
#define EE_TC_2_ISR_PRI EE_ISR_PRI_2
#define EE_TC_2_ISR_CAT 2
#define EE_ISR2_ID_EE_tc_system_timer_handler 2

/* Max ISR priority */
#define EE_TC_MAX_ISR_ID     EE_ISR_PRI_2



/***************************************************************************
 *
 * Counter defines
 *
 **************************************************************************/
#define OSMAXALLOWEDVALUE_system_timer 2147483647U
#define OSTICKSPERBASE_system_timer    1U
#define OSMINCYCLE_system_timer        1U

#define OSMAXALLOWEDVALUE 2147483647U
#define OSTICKSPERBASE    1U
#define OSMINCYCLE        1U
#define OSTICKDURATION    100000U



/***************************************************************************
 *
 * Vector size defines
 *
 **************************************************************************/
    #define EE_ACTION_ROM_SIZE 2
    #define EE_ALARM_ROM_SIZE 2
    #define EE_AS_RPC_ALARMS_SIZE 2
    #define EE_AS_RPC_COUNTERS_SIZE 1
    #define EE_AS_RPC_SERVICES_TABLE_SIZE 5
    #define EE_AS_RPC_TASKS_SIZE 5
    #define EE_COUNTER_HW_ROM_SIZE 1
    #define EE_COUNTER_OBJECTS_ROM_SIZE 2
    #define EE_COUNTER_ROM_SIZE 1
    #define EE_OO_AUTOSTART_ALARM_MODE_OSDEFAULTAPPMODE_SIZE 1
    #define EE_TC_SYSTEM_TOS_SIZE 2


#endif

