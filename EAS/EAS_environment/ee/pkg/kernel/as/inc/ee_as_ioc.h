/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2012  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation,
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

 /** @file      ee_as_ioc.h
  *  @brief     APIs declarations for Autosar IOC module
  *  @author    Francesco Esposito
  *  @date      2012
  */

#ifndef INCLUDE_EE_IOC_AS_KERNEL__
#define INCLUDE_EE_IOC_AS_KERNEL__

#ifndef EE_AS_RPC__
#error IOC internally depends on AS RPC mechanism!
#endif /* EE_AS_RPC__ */

#ifdef EE_AS_INCLUDE_AUTOSAR_HEADERS
#include "rte.h"            /* To include RTE macros (e.g.: RTE_E_OK */
#else
/* TODO: Dummy values introduced to prevent user to add his personal header RTE
   file */
/* Since RTE does not (yet) exist, they are (temporary) defined as follow */
#define RTE_E_OK        0U      /* No error occurred */
#define RTE_E_NOK       1U      /* Error occurred */
#define RTE_E_NO_DATA   131U    /* No data available for reception */
#define RTE_E_LIMIT     130U    /* Buffer dimension */
#define RTE_E_LOST_DATA 64U     /* Buffer dimension */
#endif

#define IOC_PRIMITIVE_TYPE          0U  /**<  Denotes a primitive type
                                        (e.g. uint32) **/
#define IOC_NON_PRIMITIVE_TYPE      1U  /**<  Denotes other type
                                        (e.g. *TASKParams3) **/

#define IOC_UNQUEUED                0U  /**<  It denotes "unqueued" data
                                        (Last-Is-Best) **/
#define IOC_QUEUED                  1U  /**<  It denotes "queued" data
                                        (First-In-First-Out) **/

#define IOC_CALLBACK_YES            0U  /**<  Callback configured **/
#define IOC_CALLBACK_NOT            1U  /**<  Callback not configured **/

/* Return values remapping (from RTE codes to IOC codes) */
#define IOC_E_OK        RTE_E_OK
#define IOC_E_NOK       RTE_E_NOK
#define IOC_E_NO_DATA   RTE_E_NO_DATA
#define IOC_E_LIMIT     RTE_E_LIMIT
#define IOC_E_LOST_DATA RTE_E_LOST_DATA

typedef void * EE_IOC_DATA_PTR;
typedef const void * EE_IOC_DATA_PTR_CONST;

/* Uncomment the following line to allow buffer overrun. But notice that
it is not allowed for Autosar */
/*#define BUFFER_OVERRUN_ALLOWED  */

/** 
    @brief Cicular Buffers Type

    The implementation of the circular queue make use of the 
    <it>fill counter</it>  mechanism to solve the Full/Empty condition and 
    particulary the case with one index and a fill counter
    (no additional variable required).
**/
typedef struct EE_IOC_Buffer{
    EE_UINT8    id;             /**<  message id **/
    EE_UINT16   head;           /**<  head: points to first byte to be 
                                extracted **/
    EE_UINT16   counter;        /**<  counter: number of byte filled in 
                                buffer **/
    EE_UINT16   buffer_length;  /* IOC uses OsIocBufferLength */
    EE_UINT8 *  memory;         /**<  memory: points to the buffer memory
                                (supposed allocated byte dimension:
                                buffer_length) **/
    EE_INT8     data_rejected;  /**<  mark as buffer with "rejected data".
                                Field used by IocReceive_XXX **/
    EE_UINT8    lenght;         /**< Single data length (in bytes) **/
    EE_UINT8    callback;       /**< Ioc receive callback **/
} EE_IOC_Buffer;

typedef EE_UINT8 EE_IOC_BufferError;

/*================================================= 
          Symbols and macros definition
==================================================*/
/**
 @brief error codes
 @{
**/
#define EE_CBUFF_OK             (0x00)
#define EE_CBUFF_ERR_NULL       (-1)
#define EE_CBUFF_ERR_TOO_MANY   (-2)
#define EE_CBUFF_ERR_TOO_FEW    (-3)
#define EE_CBUFF_ERR_OOB        (-4)    /** Out of Bounds **/

#define EE_CBUFF_DATA_REJECTED  (-5)    /** to mark a buffer as buffer
                                        with rejected data **/
#define EE_CBUFF_DATA_ACCEPTED  (-6)    /** to mark a buffer as working in
                                        normal condition **/
/** @} **/

/** @brief CBuffer NULL pointer **/
#define EE_NULL_CBUFF           ((EE_IOC_Buffer *)0)

/**
    @brief Performs an "explicit" sender-receiver transmission of data
            elements with "event" semantic for a unidirectional 1:1 or N:1
            communication between OS-Applications located on the same or
            on different cores. IocSend API call is generated for "events"
            (queued First-In-First-Out, event semantics) semantics.
            Autosar reference: [OS718].

    @param id: is a unique identifier that references a unidirectional
            1:1 or N:1 communication.
    @param data: Data value to be sent over a communication identified

    @return IOC_E_OK: The data has been passed successfully to the
            communication service;
            IOC_E_LIMIT: IOC internal communication buffer is full (Case:
            Receiver is slower than sender)
**/
EE_INT8 IocSend(EE_UINT8 id, EE_IOC_DATA_PTR_CONST data);

/**
    @brief Performs an "explicit" sender-receiver transmission of data
            elements with "data" semantic for a unidirectional 1:1 or N:1
            communication between OS-Applications located on the same or
            on different cores. IocWrite API call is generated for "data"
            (unqueued (Last-is-Best, data semantics)) semantics.
            Autosar reference: [OS718].

    @param id: is a unique identifier that references a unidirectional
            1:1 or N:1 communication.
    @param data: Data value to be sent over a communication identified

    @return IOC_E_OK: The data has been passed successfully to the
            communication service;
**/
EE_INT8 IocWrite(EE_UINT8 id, EE_IOC_DATA_PTR_CONST data);

/**
    @brief Performs an "explicit" sender-receiver reception of data
            elements with "event" semantic for a unidirectional communication
            between OS-Applications located on the same or on different cores.
            Autosar reference: [OS718].

    @param id: is a unique identifier that references a unidirectional
            1:1 or N:1 communication.
    @param data: Data reference to be filled with the received data element.

    @return IOC_E_OK: Data was received successfully
            IOC_E_NO_DATA: No data is available for reception
            IOC_E_LOST_DATA: This Overlayed Error indicates that the IOC
            communication service refused an IOCSend request from sender
            due to an internal buffer overflow. There is no error in the data
            returned in parameter.
**/
EE_INT8 IocReceive(EE_UINT8 id, EE_IOC_DATA_PTR data);

/**
    @brief Performs an "explicit" sender-receiver reception of data
            elements with "data" semantic for a unidirectional communication
            between OS-Applications located on the same or on different cores.
            Autosar reference: [OS718].

    @param id: is a unique identifier that references a unidirectional
            1:1 or N:1 communication.
    @param data: Data reference to be filled with the received data element.

    @return IOC_E_OK: Data was received successfully
**/
EE_INT8 IocRead(EE_UINT8 id, EE_IOC_DATA_PTR data);

/**
    @brief In case of queued communication identified by the <IocId> in
            the function name, the content of the IOC internal communication
            queue shall be deleted.
            Autosar reference: [OS754].

    @param id: none

    @return IOC_E_OK: Data was received successfully
**/
EE_INT8 IocEmptyQueue(EE_UINT8 id);

/**
    @brief Performs an "explicit" sender-receiver transmission of data
            elements with "data" semantic for a unidirectional 1:1
            communication between OS-Applications located on the same
            or on different cores.

    @param id: List of parameters with data values to be sent over a
            communication identified by the <IocId>. The parameters will be
            passed by value for simple data elements and by reference for all
            other types.

    @return IOC_E_OK: The data has been passed successfully to the
            communication service.
**/
void IocWriteSingleElement(EE_UINT8 id, EE_IOC_DATA_PTR_CONST data,
EE_UINT8 size, EE_UINT8 parameter_number);

/**
    @brief Performs an "explicit" sender-receiver transmission of data
            elements with a "data" semantic for a unidirectional 1:1
            communication between OS-Applications located on the same or
            on different cores.


    @param id: List of data references to be filled with the received data
            elements. The specified order of the parameter shall match to
            the specified order in the corresponding send function.


    @return IOC_E_OK: Data was received successfully
**/
void IocReadSingleElement(EE_UINT8 id, EE_IOC_DATA_PTR data, EE_UINT8 size,
  EE_UINT8 parameter_number);

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
/** @brief List that tie IOC buffer IDs to his receiving core. It SHALL be
    generated by RT-Druid IOC Generator. If Erika IOC implementation is used
    without a generator this structure MUST provided in a project
    module (e.g. ioc_common.c) */
extern EE_TYPECOREID const EE_SHARED_CDATA EE_as_rpc_IOC_channels[];
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @brief Signal asynchronously the right core to start callback Sequence. */
void EE_as_IOC_signal_callbacks( EE_UINT8 id );

/** @brief Standard function that SHALL be generated by IOC generator that
    implement the IOC callbacks sequece for current core */
void EE_as_IOC_callback_sequence( void );

#endif /* INCLUDE_EE_IOC_AS_KERNEL__ */

