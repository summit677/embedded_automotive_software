/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2013  Evidence Srl
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
 * Author: 2013 Alessandro Biondi
 * 
 */

#include "ee_internal.h"

#ifndef __PRIVATE_RQ_EXTRACT__

void EE_rq_extract_vres(EE_TYPECONTRACT v)
{	
	/* check for empty RQ */
	if(EE_rq_queryfirst_vres()==EE_NIL) return;
	if(!(EE_vres[v].status2 & EE_VRES_INRQ)) return;
	
	if(EE_vres[v].prev==EE_NIL) { // v is the head element
		EE_rqfirst_vres = EE_vres[v].next;
		if(EE_rqfirst_vres!=EE_NIL) EE_rqfirst = EE_vres[EE_rqfirst_vres].task;
		else EE_rqfirst = EE_NIL;
	}
	else EE_vres[EE_vres[v].prev].next = EE_vres[v].next;
	
	if(EE_vres[v].next!=EE_NIL)
		EE_vres[EE_vres[v].next].prev = EE_vres[v].prev;
    
	
	EE_vres[v].status2 &= ~EE_VRES_INRQ; //inRQ=0
}

//--------------------------------------------------------------------

void EE_rq_extract(EE_TID thread)
{
  EE_TID p,t;
  
  #ifdef DEBUG
  /* check for incorrect parameters */
  if((thread==EE_NIL) || (thread >= EE_MAX_TASK)) return;
  if((EE_th[thread].vres==EE_NIL) || (EE_th[thread].vres>=EE_MAX_CONTRACT)) return;
  if(!(EE_vres[EE_th[thread].vres].status2 & EE_VRES_INRQ)) return;
  #endif

  p = EE_NIL;
  t = EE_vres[EE_th[thread].vres].task;
  
  while ( (t != EE_NIL) && (t != thread)) {
    p = t;
    t = EE_th[t].next;
  }
  
  if (t == EE_NIL) /* the thread is not there */
    return;
  
  /* t == thread !!! */
  if ( p == EE_NIL )
  { // thread is the first in its queue
    
    if(EE_th[thread].vres==EE_rqfirst_vres) EE_rq_getfirst(); /* remove the first item in RQ */
    else 
    {
		EE_vres[EE_th[thread].vres].task = EE_th[thread].next;
		 
		// remove vres from RQ if its queues become empty
		//if(EE_vres[EE_th[thread].vres].task==EE_NIL && EE_vres[EE_th[thread].vres].stkfirst==EE_NIL)
		//	EE_rq_extract_vres(EE_th[thread].vres);
	} 
  }
  else 
  {
		/* remove an item in the middle of the ready queue */
		EE_th[p].next = EE_th[t].next;
  }
  
}

#endif
