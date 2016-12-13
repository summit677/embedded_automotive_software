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

 
/*
 * The ready queue is structured as a multi-level queue. The first level is for the vres; for 
 * each vres we have the queue of the served tasks. The queues of tasks are ordered in function
 * of the scheduling algorithm used upon the vres.
 *
 *   | VRES_1 | --> (Task1_1)-->(Task2_1)...
 *   | VRES_2 | --> (Task1_2)-->(Task2_2)...
 *      ...
 *   | VRES_n | --> (Task1_n)-->(Task2_n)...
 *
 *
 * When a task is executing it is not in the ready queue, whereas its vres remains in the ready queue.
 * This is to simplify the operations on the global stacked queue.
 *
 * The first level (queue of vres) is implemented with a double-linked list.
 *
 * EE_rqfirst is the first task of the first vres;
 * EE_rqfirst_vres is the first vres.
 */
 
#include "ee_internal.h"

#ifndef __PRIVATE_RQ_INSERT_VRES__

void EE_rq_insert_vres(EE_TYPECONTRACT v)
{
	  int prio;
      EE_TYPECONTRACT p,q;
	  
	  p = EE_NIL;
	  q = EE_rq_queryfirst_vres();
	  prio = EE_vres[v].absdline;

	  while ((q != EE_NIL) && 
		 (EE_STIME)(prio - EE_vres[q].absdline) >= 0
		) {
		p = q;
		q = EE_vres[q].next;
	  }
	  
	  EE_vres[v].prev = p; 

	  if (p != EE_NIL)
		EE_vres[p].next = v;
	  else {
		EE_rqfirst_vres = v;
		EE_rqfirst = EE_vres[v].task;
	  }

	  EE_vres[v].next = q;
	  if(q!=EE_NIL) EE_vres[q].prev = v;
	  
	  EE_vres[v].status2 |= EE_VRES_INRQ; /* set flag */
}

#endif

//--------------------------------------------------------------------

#ifndef __PRIVATE_INSERT_IN_VRES__

void EE_insert_in_vres(EE_TID t)
{
	int prio;
	EE_TID p,q;
	p = EE_NIL;
    q = EE_vres[EE_th[t].vres].task; // first in VRES's RQ
	
	switch(EE_ct[EE_th[t].vres].sched_algo)
    {
	  case EE_SCHED_EDF:
		prio = EE_th[t].absdline; 

		while ((q != EE_NIL) && 
		       (EE_STIME)(prio - EE_th[q].absdline) >= 0
		      ) {
		      p = q;
		      q = EE_th[q].next;
	  	}
	  break;

	  case EE_SCHED_FP:	
		prio = EE_th[t].reldline; 

  		while ((q != EE_NIL) && (prio <= EE_th[q].reldline)) {
		      p = q;
		      q = EE_th[q].next;
	  	}
	
	  break;
    }

    if (p != EE_NIL)
	  EE_th[p].next = t;
    else 
    {
	  EE_vres[EE_th[t].vres].task = t;
	  if(EE_th[t].vres==EE_rqfirst_vres) EE_rqfirst=t;
    }

    EE_th[t].next = q;
	
}

#endif

//--------------------------------------------------------------------

#ifndef __PRIVATE_RQ_INSERT__

void EE_rq_insert(EE_TID t)
{
 
  if(!(EE_vres[EE_th[t].vres].status2 & EE_VRES_INRQ))
	  EE_rq_insert_vres(EE_th[t].vres);
  
  /* insert task into VRES's RQ */
  EE_insert_in_vres(t);
  
  
}

#endif
