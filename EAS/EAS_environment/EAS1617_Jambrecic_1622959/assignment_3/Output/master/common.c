#include "ee.h"


/***************************************************************************
 *
 * Remote Notification
 *
 **************************************************************************/
    const EE_UINT8 EE_SHARED_CDATA EE_rn_cpu[EE_MAX_RN] =
        { 0U, 1U, 2U};

    #if defined( __RN_EVENT__ ) || defined( __RN_TASK__ )
        const EE_TID EE_SHARED_CDATA EE_rn_task[EE_MAX_RN] = {
            0,		 /* cpu_0 - thread TaskMaster */
            0,		 /* cpu_1 - thread TaskSlave1 */
            0		 /* cpu_2 - thread TaskSlave2 */
        };
    #endif

    /* For each RN: The type of notification that must be used
     * EE_RN_COUNTER, EE_RN_EVENT, EE_RN_TASK,
     * EE_RN_FUNC, EE_RN_SEM
     */
    EE_TYPERN_NOTIFY EE_SHARED_IDATA EE_rn_type[EE_MAX_RN][2] = {
        {0U, 0U},		 /* cpu_0 - thread TaskMaster */
        {0U, 0U},		 /* cpu_1 - thread TaskSlave1 */
        {0U, 0U}		 /* cpu_2 - thread TaskSlave2 */
    };

    /* For each RN: The counter number if EE_RN_COUNTER, or -1 */
    #ifdef __RN_COUNTER__
        const EE_TYPECOUNTER EE_SHARED_CDATA EE_rn_counter[EE_MAX_RN] =
            {-1, -1, -1};
    #endif

    #ifdef __RN_EVENT__ 
        EE_TYPEEVENTMASK EE_SHARED_IDATA EE_rn_event[EE_MAX_RN][2] = {
            {0U, 0U},
            {0U, 0U},
            {0U, 0U}
        };
    #endif

    EE_TYPERN EE_SHARED_IDATA EE_rn_next[EE_MAX_RN][2] = {
            {-1, -1},
            {-1, -1},
            {-1, -1}
    };

    EE_UREG EE_SHARED_IDATA EE_rn_pending[EE_MAX_RN][2] = {
        {0U, 0U},
        {0U, 0U},
        {0U, 0U}
    };

    #ifdef __RN_FUNC__
        /* const EE_ADDR EE_SHARED_CDATA EE_rn_func[EE_MAX_RN] = {(EE_ADDR)f,...};*/
    #endif

    #ifdef __RN_SEM__
        /*EE_SEM * const EE_SHARED_CDATA EE_rn_sem[EE_MAX_RN];*/
    #endif

    EE_TYPERN EE_SHARED_IDATA EE_rn_first[EE_MAX_CPU][2] = {
        { -1, -1}, { -1, -1}, { -1, -1}
    };

    const EE_TYPESPIN EE_SHARED_CDATA EE_rn_spin[EE_MAX_CPU] = { EE_SPINLOCK_CORE0, EE_SPINLOCK_CORE1, EE_SPINLOCK_CORE2};

    EE_TYPERN_SWITCH EE_SHARED_IDATA EE_rn_switch[EE_MAX_CPU] = { 0U, 0U, 0U};



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
        INVALID_CORE_ID,
        INVALID_CORE_ID
    };
    SpinlockIdType volatile EE_SHARED_IDATA EE_as_spinlocks_last[EE_MAX_CPU] = {
        INVALID_SPINLOCK,
        INVALID_SPINLOCK,
        INVALID_SPINLOCK
    };
    SpinlockIdType volatile EE_SHARED_IDATA EE_as_spinlocks_stack[EE_MAX_SPINLOCK_USER] = {
        INVALID_SPINLOCK,
        INVALID_SPINLOCK
    };

