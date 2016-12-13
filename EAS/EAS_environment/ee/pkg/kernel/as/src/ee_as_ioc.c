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

 /** @file      ee_as_ioc.c
  *  @brief     Autosar API and data structures for IOC module
  *  @author    Francesco Esposito
  *  @date      2012
  */

#include "ee_internal.h"
#include <string.h>

/*===========================================================
            Static Functions (Private Implementation)
===========================================================*/

static EE_UINT8 * EE_IOC_Buffer_push_and_get_write_addr(EE_IOC_Buffer *cbuffer,
EE_UINT16 data_length)
{
    /* Circular arithmetic to evaluate "push index" */
    EE_UINT16 write_index;
	EE_UINT8 * result = NULL;

	if (cbuffer != NULL) {
		write_index = (cbuffer->head + cbuffer->counter) %
    		cbuffer->buffer_length;
    	/* Increment Count (new element added)*/
    	cbuffer->counter += data_length;

		result = (EE_UINT8 *)cbuffer->memory + write_index;
	}

    /* evaluate the addres where write to */
    return (result);
}

static EE_UINT8 * EE_IOC_Buffer_pop_and_get_read_addr(EE_IOC_Buffer *cbuffer,
EE_UINT16 data_length)
{
    /* evaluate the address where read to */
    EE_UINT8 * read_addr = NULL;

	if (cbuffer != NULL) {
		read_addr = (EE_UINT8 *)cbuffer->memory + cbuffer->head;
    	/* Circular arithmetic to evaluate new head */
    	cbuffer->head = (cbuffer->head + data_length) % cbuffer->buffer_length;
    	/* Decrement count (element removed) */
    	cbuffer->counter -= data_length;
	}

    return read_addr;
}

/**
    @brief Check if the buffer is empty.

    Checks the counter of the buffer is equal to zero.
    
    @param cbuffer: c-buffer struct pointer
    @param num_bytes: number of bytes that buffer should contains at least
    
    @return 1 if buffer contains more or equal bytes of num_bytes
            0 otherwise    
**/
static EE_UINT8 EE_IOC_Buffer_contains(const EE_IOC_Buffer *cbuffer,
EE_UINT16 num_bytes)
{
	EE_UINT8 result = 0U;

	if (cbuffer != NULL) {
		if (cbuffer->counter >= num_bytes) {
			result = 1U;	/* True */
		}
		else {
			result = 0U;	/* False */
		}
	}

    return (result);
}

/**
    @brief Check if the buffer is full.
    
    Compare the counter of the buffer with the max size.
    
    @param cbuffer: c-buffer struct pointer
    @return 1 if in buffer are available more or equal bytes of num_bytes
            0 otherwise     
**/
static EE_UINT8 EE_IOC_Buffer_available(const EE_IOC_Buffer *cbuffer,
EE_UINT16 num_bytes)
{
    EE_UINT8 result = 0U;

	if (cbuffer != NULL) {
		if ((cbuffer->buffer_length - cbuffer->counter) >= num_bytes) {
			result = 1U;	/* True */
		}
		else {
			result = 0U;	/* False */
		}
	}

    return (result);
}

/*static EE_IOC_BufferError EE_IOC_Buffer_init(EE_IOC_Buffer * cbuffer,
EE_UINT16 buffer_length, EE_UINT8 * memory)
{
    EE_IOC_BufferError error = EE_CBUFF_OK;

    if(cbuffer == EE_NULL_CBUFF) {
        error =  EE_CBUFF_ERR_NULL;
    }

    if(error == EE_CBUFF_OK){
        cbuffer->head           = 0;
        cbuffer->counter        = 0;
        cbuffer->buffer_length  = buffer_length;
        cbuffer->memory         = memory;
    }

    return error;
}*/

/* This functions puts a message in the buffer (push in queue) */
static EE_IOC_BufferError EE_IOC_Buffer_push(EE_IOC_Buffer * cbuffer,
const EE_UINT8 * ele, EE_UINT16 data_length)
{
    EE_UINT8 * write_addr;
    EE_IOC_BufferError error = (EE_IOC_BufferError)EE_CBUFF_OK;

    if((cbuffer == EE_NULL_CBUFF) || (ele == NULL) || (data_length == 0U)) {
        return ((EE_IOC_BufferError)EE_CBUFF_ERR_NULL);
    }

    if(EE_IOC_Buffer_available(cbuffer, data_length) ==
	(EE_UINT8)EE_CBUFF_OK) {
        /* Mark current buffer as buffer having rejected data */
        cbuffer->data_rejected = EE_CBUFF_DATA_REJECTED;

        /* Set return value to warn about a full buffer */
        error = (EE_IOC_BufferError)EE_CBUFF_ERR_TOO_MANY;
#if !defined (BUFFER_OVERRUN_ALLOWED)
        return (error);
#endif
    }

    write_addr = EE_IOC_Buffer_push_and_get_write_addr(cbuffer, data_length);

	if (write_addr != NULL) {
		if(data_length > 1U) {
		    EE_INT32 offset = (EE_INT8 *)write_addr -
				(EE_INT8 *)cbuffer->memory;

		    /* write in array boundaries */
		    if((offset + (EE_INT32)data_length) <=
			(EE_INT32)cbuffer->buffer_length) {
		        (void)memcpy(write_addr, ele, (size_t)data_length);
		    }
		    else { /* write outside array boundaries */
		        EE_INT32 bytes_before_the_end =
					(EE_INT32)cbuffer->buffer_length - offset;

		        EE_INT32 remaining_bytes = (EE_INT32)data_length -
					bytes_before_the_end;

		        (void)memcpy(write_addr, ele, (size_t)bytes_before_the_end);
		        (void)memcpy(cbuffer->memory,
					(const EE_INT8 *)ele + bytes_before_the_end,
					(size_t)remaining_bytes);
		    }
		}
		else {
		    ((EE_INT8 *)write_addr)[0] = ((const EE_INT8 *)ele)[0];
		}
	}

    return error;
}

static EE_IOC_BufferError EE_IOC_Buffer_pop(EE_IOC_Buffer *cbuffer,
EE_UINT8 * ele, EE_UINT16 data_length)
{
    EE_IOC_BufferError error = (EE_IOC_BufferError)EE_CBUFF_OK;

    if((cbuffer == EE_NULL_CBUFF) || (ele == NULL) || (data_length == 0U)) {
        error =  (EE_IOC_BufferError)EE_CBUFF_ERR_NULL;
    }

    if(error == (EE_IOC_BufferError)EE_CBUFF_OK) {
        if(EE_IOC_Buffer_contains(cbuffer, data_length) ==
		(EE_UINT8)EE_CBUFF_OK) {
            error = (EE_IOC_BufferError)EE_CBUFF_ERR_TOO_FEW;
        } else {
            EE_UINT8 * read_addr = EE_IOC_Buffer_pop_and_get_read_addr(cbuffer,
            data_length);

			if ((read_addr != NULL) && (ele != NULL)) {
		        if(data_length > 1U){
					if (cbuffer != NULL) {
				        EE_INT32 offset = (EE_INT8 *)read_addr -
				        (EE_INT8 *)cbuffer->memory;

				        /* read in array boundaries */
				        if(((EE_UINT16)offset + data_length) <=
						cbuffer->buffer_length) {
				            (void)memcpy(ele, read_addr, (size_t)data_length);
				        } else { /* read outside array boundaries */
				            EE_INT32 bytes_before_the_end =
								(EE_INT32)cbuffer->buffer_length - offset;

				            EE_INT32 remaining_bytes = (EE_INT32)data_length -
				            bytes_before_the_end;

				            (void)memcpy(ele, read_addr,
								(size_t)bytes_before_the_end);
				            (void)memcpy((EE_INT8 *)ele + bytes_before_the_end,
				            	cbuffer->memory, (size_t)remaining_bytes);
				        }
					}
		        } else {
		            ((EE_INT8 *)ele)[0] = ((EE_INT8 *)read_addr)[0];
		        }
			}
        }
    }

    return error;
}

/*===========================================================
        Global Functions (Public Autosar Interface)
===========================================================*/
EE_INT8 IocSend(EE_UINT8 id, EE_IOC_DATA_PTR_CONST data)
{
    /* Initialise return value of the following internal library call */
    EE_INT8 result;

    /*
     * Use id to access auto-generated data
     */
    result = (EE_INT8)EE_IOC_Buffer_push(&EE_ioc_enqueued_buffer[id],
        (const EE_UINT8*)data, (EE_UINT16)EE_ioc_enqueued_buffer[id].lenght);

    /* Buffer is full */
    if (EE_CBUFF_ERR_TOO_MANY == result) {
        return((EE_INT8)IOC_E_LIMIT);
    }

    return((EE_INT8)IOC_E_OK);
}

EE_INT8 IocWrite(EE_UINT8 id, EE_IOC_DATA_PTR_CONST data)
{
    /*
     * Use id to access auto-generated data
     */
	if (data != NULL) {
    	(void)memcpy(EE_ioc_enqueued_buffer[id].memory, data,
        	(size_t)(EE_ioc_enqueued_buffer[id].lenght));
	}

    /* Always return IOC_E_OK */
    return((EE_INT8)IOC_E_OK);
}

EE_INT8 IocReceive(EE_UINT8 id, EE_IOC_DATA_PTR data)
{
    /* Initialise return value of the following internal library call */
    EE_INT8 result;

    /*
     * Use id to access auto-generated data
     */
    result = (EE_INT8)EE_IOC_Buffer_pop(&EE_ioc_enqueued_buffer[id],
			(EE_UINT8*)data, (EE_UINT16)EE_ioc_enqueued_buffer[id].lenght);

    /* Check if IOC refused a previous IocSend_XXX */
    if ((EE_CBUFF_DATA_REJECTED == EE_ioc_enqueued_buffer[id].data_rejected)
		&& (result == EE_CBUFF_OK)) {

        /* a data has been extracted from the queue, now current buffer
        can get back to normal if its previous status was "rejected" */
        EE_ioc_enqueued_buffer[id].data_rejected = EE_CBUFF_DATA_ACCEPTED;

        return((EE_INT8)IOC_E_LOST_DATA);
    }

    /* Buffer is empty */
    if (EE_CBUFF_ERR_TOO_FEW == result) {
        return((EE_INT8)IOC_E_NO_DATA);
    }

    /* result is or-ed with result (since a lost data might be occurred) */
    return((EE_INT8)IOC_E_OK);
}

EE_INT8 IocRead(EE_UINT8 id, EE_IOC_DATA_PTR data)
{
    /*
     * Use id to access auto-generated data
     */
	if (data != NULL) {
    	(void)memcpy(data, EE_ioc_enqueued_buffer[id].memory,
        	(size_t)(EE_ioc_enqueued_buffer[id].lenght));
	}

    /* Always return IOC_E_OK */
    return((EE_INT8)IOC_E_OK);
}

EE_INT8 IocEmptyQueue(EE_UINT8 id)
{
    /* Empty buffer identified by id */
    EE_ioc_enqueued_buffer[id].head = 0U;
    EE_ioc_enqueued_buffer[id].counter = 0U;

    /* Mark this buffer as available (since it has been emptied) */
    EE_ioc_enqueued_buffer[id].data_rejected = EE_CBUFF_DATA_ACCEPTED;

    /* Always return IOC_E_OK */
    return((EE_INT8)IOC_E_OK);
}

void IocWriteSingleElement(EE_UINT8 id, EE_IOC_DATA_PTR_CONST data,
EE_UINT8 size, EE_UINT8 parameter_number)
{
	if (data != NULL) {
    	(void)memcpy(EE_ioc_enqueued_buffer[id].memory +
			(parameter_number * size), data, (size_t)size);
	}
}

void IocReadSingleElement(EE_UINT8 id, EE_IOC_DATA_PTR data, EE_UINT8 size,
EE_UINT8 parameter_number)
{
	if (data != NULL) {
    	(void)memcpy(data, EE_ioc_enqueued_buffer[id].memory +
			(parameter_number * size), (size_t)size);
	}
}

#ifdef EE_MASTER_CPU
/** @brief Flag used to signal a IOC request */
EE_BIT EE_SHARED_UDATA EE_as_rpc_IOC[ EE_MAX_CPU ];
#endif /* EE_MASTER_CPU */

void EE_as_IOC_signal_callbacks( EE_UINT8 id )
{
  /* Unitializated locals */
  register  CoreIdType                ioc_cpu;
  register  EE_TYPESPIN               spinlock_id;
  /* Initialized locals */
  register  EE_FREG const  flag     = EE_hal_begin_nested_primitive();

  /* Get the core that shall receive the notification */
  ioc_cpu = EE_as_rpc_IOC_channels[ id ];

  /* Select the Right Spinlock Where Synchronize on */
  spinlock_id = EE_as_core_spinlocks[ ioc_cpu ];

  /* Enter RPC Critical Section */
  EE_hal_spin_in( spinlock_id );

  /* Signal the IOC Request */
  EE_as_rpc_IOC[ ioc_cpu ] = 1U;

  if ( EE_as_rpc_serving[ ioc_cpu ] == 0U )
  {
    /* In any case we shall assure that EE_hal_IRQ_interprocessor can
       handle multiple sequential calls */
    EE_hal_IRQ_interprocessor( (EE_UINT8)ioc_cpu );
  }

  /* Exit RPC Critical Section */
  EE_hal_spin_out(spinlock_id);

  EE_hal_end_nested_primitive(flag);
}

