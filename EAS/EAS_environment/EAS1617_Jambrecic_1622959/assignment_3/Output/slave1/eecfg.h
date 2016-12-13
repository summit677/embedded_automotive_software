#ifndef RTDH_EECFG_H
#define RTDH_EECFG_H


#define RTDRUID_CONFIGURATOR_NUMBER 1278



/***************************************************************************
 *
 * Common defines ( CPU 1 )
 *
 **************************************************************************/

    /* TASK definition */
    #define EE_MAX_TASK 1
    #define TaskSlave1 0

    /* MUTEX definition */
    #define EE_MAX_RESOURCE 0U

    /* ALARM definition */
    #define EE_MAX_ALARM 0U

    /* SCHEDULING TABLE definition */
    #define EE_MAX_SCHEDULETABLE 0U

    /* COUNTER OBJECTS definition */
    #define EE_MAX_COUNTER_OBJECTS (EE_MAX_ALARM + EE_MAX_SCHEDULETABLE)

    /* COUNTER definition */
    #define EE_MAX_COUNTER 0U

    /* APPMODE definition */
    #define EE_MAX_APPMODE 1U

    /* CPUs */
    #define EE_MAX_CPU 3
    #define EE_CURRENTCPU 1

    /* Number of isr 2 */
    /* Remote notification requires an additional ISR2 */
    #define EE_MAX_ISR2   1
    #define EE_MAX_ISR_ID 0

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
#define __OO_ORTI_LASTERROR__
#define __OO_ORTI_SERVICETRACE__
#define __OO_ORTI_PRIORITY__
#define __OO_ORTI_RES_LOCKER_TASK__
#define __OO_ORTI_RES_ISLOCKED__
#define __OO_ORTI_STACK__
#define __OO_ORTI_ALARMTIME__
#define __OO_ORTI_RUNNINGISR2__
#define __RN__
#define __RN_TASK__
#define EE_AS_USER_SPINLOCKS__
#define __OO_ECC1__
#define __OO_EXTENDED_STATUS__
#define __MULTI__
#define __OO_NO_ALARMS__
#define __OO_NO_RESOURCES__
#define __OO_NO_EVENTS__
#define __OO_AUTOSTART_TASK__
#define __ALLOW_NESTED_IRQ__

#endif



/***************************************************************************
 *
 * Remote Notification
 *
 **************************************************************************/
    #define EE_MAX_RN 3
    #define rn_TaskMaster 0
    #define rn_TaskSlave1 1
    #define rn_TaskSlave2 2

    /* remote threads */
    #define TaskMaster ((EE_TID)rn_TaskMaster+(EE_TID)EE_REMOTE_TID)
    #define TaskSlave2 ((EE_TID)rn_TaskSlave2+(EE_TID)EE_REMOTE_TID)



/***************************************************************************
 *
 * Spin lock IDs
 *
 **************************************************************************/
    #define EE_MAX_SPINLOCK_USER 2

    #define EE_SPINLOCK_CORE0 4U	 /* master */
    #define EE_SPINLOCK_CORE1 3U	 /* slave1 */
    #define EE_SPINLOCK_CORE2 2U	 /* slave2 */
    #define SpinlockScreen 0U
    #define SpinlockLed 1U
    #define EE_MAX_SPINLOCK 5
    #define EE_MAX_SPINLOCK_OS 5


    /* System stack size */
    #define EE_SYS_STACK_SIZE     4096



/***************************************************************************
 *
 * Vector size defines
 *
 **************************************************************************/
    #define EE_OO_AUTOSTART_TASK_MODE_OSDEFAULTAPPMODE_SIZE 1
    #define EE_TC_SYSTEM_TOS_SIZE 2


#endif

