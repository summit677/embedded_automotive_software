#include "ee.h"


/***************************************************************************
 *
 * Remote Procedure Call
 *
 **************************************************************************/
    EE_TYPESPIN const EE_SHARED_CDATA EE_as_core_spinlocks[EE_MAX_CPU] = {
        EE_SPINLOCK_CORE0,
        EE_SPINLOCK_CORE1,
        EE_SPINLOCK_CORE2
    };

    EE_TYPEASREMOTEID const EE_SHARED_CDATA EE_as_rpc_tasks[EE_AS_RPC_TASKS_SIZE] = {
        { OS_CORE_ID_0, 0U},
        { OS_CORE_ID_1, 0U},
        { OS_CORE_ID_1, 1U},
        { OS_CORE_ID_2, 0U},
        { OS_CORE_ID_2, 1U}
    };

    EE_TYPEASREMOTEID const EE_SHARED_CDATA EE_as_rpc_alarms[EE_AS_RPC_ALARMS_SIZE] = {
        { OS_CORE_ID_0, 0U},
        { OS_CORE_ID_0, 1U}
    };

    EE_TYPEASREMOTEID const EE_SHARED_CDATA EE_as_rpc_counters[EE_AS_RPC_COUNTERS_SIZE] = {
        { OS_CORE_ID_0, 0U}
    };

    EE_TYPEASREMOTEIDCONSTREF const EE_SHARED_CDATA EE_as_rpc_services_table[EE_AS_RPC_SERVICES_TABLE_SIZE] = {
        &EE_as_rpc_tasks[0], &EE_as_rpc_alarms[0], &EE_as_rpc_counters[0], 0U, 0U
    };

    EE_TYPEASRPC volatile EE_SHARED_IDATA EE_as_rpc_RAM[EE_MAX_CPU] = {
        { INVALID_CORE_ID, 0U, {0U}, {0U}, {0U}, E_OK},
        { INVALID_CORE_ID, 0U, {0U}, {0U}, {0U}, E_OK},
        { INVALID_CORE_ID, 0U, {0U}, {0U}, {0U}, E_OK}
    };



/***************************************************************************
 *
 * Spin locks
 *
 **************************************************************************/

    EE_TYPESPINSTATUS EE_SHARED_UDATA EE_hal_spinlock_status[EE_MAX_SPINLOCK];



/***************************************************************************
 *
 * Slave core StartUp Address
 *
 **************************************************************************/
    extern void EE_CPU1_START_ADDR ( void );
    extern void EE_CPU2_START_ADDR ( void );

    EE_ADDR const EE_SHARED_CDATA EE_as_core_start_addresses[EE_MAX_CPU -1] = {
        &EE_CPU1_START_ADDR,
        &EE_CPU2_START_ADDR
    };



/***************************************************************************
 *
 * Spin locks
 *
 **************************************************************************/
    CoreIdType volatile EE_SHARED_IDATA EE_as_spinlocks_locker_core[EE_MAX_SPINLOCK_USER] = {
        INVALID_CORE_ID
    };
    SpinlockIdType volatile EE_SHARED_IDATA EE_as_spinlocks_last[EE_MAX_CPU] = {
        INVALID_SPINLOCK,
        INVALID_SPINLOCK,
        INVALID_SPINLOCK
    };
    SpinlockIdType volatile EE_SHARED_IDATA EE_as_spinlocks_stack[EE_MAX_SPINLOCK_USER] = {
        INVALID_SPINLOCK
    };

