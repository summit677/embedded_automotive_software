/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2008  Evidence Srl
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

/*
 * Author: 2009 Paolo Gai
 * CVS: $Id: ee_cap.c,v 1.6 2008/07/18 09:53:55 tiberipa Exp $
 */

#include "ee_internal.h"
#include "frsh_error.h"


#ifndef __PRIVATE_FRSH_STRERROR__

#include <string.h>

static char *EE_frsh_strerror_table[] = 
{
  "too many tasks",
  "bad argument",
  "invalid synch obj handle",
  "no renegotiation requested",
  "contract rejected",
  "not scheduled calling thread",
  "not bound",
  "unknown scheduled thread",
  "not contracted vres",
  "not scheduled thread",
  "too many service jobs",
  "too many synch objs",
  "too many vres in synch obj",
  "too many events in synch obj",
  "internal error",
  "too many vres",
  "invalid scheduler reply",
  "too many pending replenishments",
  "system already initialized",
  "shared obj already initialized",
  "shared obj not initialized",
  "sched policy not compatible",
  "vres workload not compatible",
  "already bound",
  "resource id invalid",
  "too large",
  "buffer full",
  "no space",
  "no messages",
  "module not supported",
  "not initialized",
  "too many shared objs",
  "contract label already exists",
  "budget expired",
  "shared object not protected",
  "not implemented",
  "contract type not compatible",
  "capacity not decreasing",
  "contract label unknown "
};


/* Converts an error code to a string */
int EE_frsh_strerror(int error, char *message, size_t size)
{
  if (error>FRSH_ERR_LAST_VALUE || error<=FRSH_ERR_BASE_VALUE)
    return FRSH_ERR_BAD_ARGUMENT;
  
  if (message != NULL && size > 0) {
    /* let's hope the microcontroller has strncpy */
    strncpy(message,
	    EE_frsh_strerror_table[error - FRSH_ERR_BASE_VALUE - 1],
	    size);

    message[size] = '\0';
  }
  
  return FRSH_NO_ERROR;
}
#endif
