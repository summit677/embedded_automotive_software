// -----------------------------------------------------------------------
//  Copyright (C) 2006 - 2008 FRESCOR consortium partners:
//
//    Universidad de Cantabria,              SPAIN
//    University of York,                    UK
//    Scuola Superiore Sant'Anna,            ITALY
//    Kaiserslautern University,             GERMANY
//    Univ. Politécnica  Valencia,           SPAIN
//    Czech Technical University in Prague,  CZECH REPUBLIC
//    ENEA                                   SWEDEN
//    Thales Communication S.A.              FRANCE
//    Visual Tools S.A.                      SPAIN
//    Rapita Systems Ltd                     UK
//    Evidence                               ITALY
//
//    See http://www.frescor.org for a link to partners' websites
//
//           FRESCOR project (FP6/2005/IST/5-034026) is funded
//        in part by the European Union Sixth Framework Programme
//        The European Union is not liable of any use that may be
//        made of this code.
//
//
//  based on previous work (FSF) done in the FIRST project
//
//   Copyright (C) 2005  Mälardalen University, SWEDEN
//                       Scuola Superiore S.Anna, ITALY
//                       Universidad de Cantabria, SPAIN
//                       University of York, UK
//
//   FSF API web pages: http://marte.unican.es/fsf/docs
//                      http://shark.sssup.it/contrib/first/docs/
//
//   This file is part of FRSH (FRescor ScHeduler)
//
//  FRSH is free software; you can redistribute it and/or modify it
//  under terms of the GNU General Public License as published by the
//  Free Software Foundation; either version 2, or (at your option) any
//  later version.  FRSH is distributed in the hope that it will be
//  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
//  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//  General Public License for more details. You should have received a
//  copy of the GNU General Public License along with FRSH; see file
//  COPYING. If not, write to the Free Software Foundation, 675 Mass Ave,
//  Cambridge, MA 02139, USA.
//
//  As a special exception, including FRSH header files in a file,
//  instantiating FRSH generics or templates, or linking other files
//  with FRSH objects to produce an executable application, does not
//  by itself cause the resulting executable application to be covered
//  by the GNU General Public License. This exception does not
//  however invalidate any other reasons why the executable file might be
//  covered by the GNU Public License.
// -----------------------------------------------------------------------
//frsh_error.h
//==============================================
//  ******** *******    ********  **      **
//  **///// /**////**  **//////  /**     /**
//  **      /**   /** /**        /**     /**
//  ******* /*******  /********* /**********
//  **////  /**///**  ////////** /**//////**
//  **      /**  //**        /** /**     /**
//  **      /**   //** ********  /**     /**
//  //       //     // ////////   //      //
//
// FRSH(FRescor ScHeduler), pronounced "fresh"
//==============================================


/* Note for ERIKA Enterprise users: This is a reduced version of the FRESCOR
   includes for the HR kernel.*/


#ifndef   	HR_FRESCOR_H_
#define   	HR_FRESCOR_H_



/* ---------------- Error codes ----------------*/
#define HR_NO_ERROR                            0

#define HR_ERR_BASE_VALUE                      0x4000

#define HR_ERR_TOO_MANY_TASKS                  0x4001
#define HR_ERR_BAD_ARGUMENT                    0x4002
#define HR_ERR_CONTRACT_REJECTED               0x4005
#define HR_ERR_NOT_SCHEDULED_CALLING_THREAD    0x4006
#define HR_ERR_NOT_BOUND                       0x4007
#define HR_ERR_NOT_CONTRACTED_VRES             0x4009
#define HR_ERR_NOT_SCHEDULED_THREAD            0x400A
#define HR_ERR_TOO_MANY_SERVICE_JOBS           0x400B
#define HR_ERR_INTERNAL_ERROR                  0x400F
#define HR_ERR_TOO_MANY_VRES                   0x4010
#define HR_ERR_INVALID_SCHEDULER_REPLY         0x4011
#define HR_ERR_TOO_MANY_PENDING_REPLENISHMENTS 0x4012
#define HR_ERR_SYSTEM_ALREADY_INITIALIZED      0x4013
#define HR_ERR_SCHED_POLICY_NOT_COMPATIBLE     0x4016
#define HR_ERR_VRES_WORKLOAD_NOT_COMPATIBLE    0x4017
#define HR_ERR_RESOURCE_ID_INVALID             0x4019
#define HR_ERR_TOO_LARGE                       0x401A
#define HR_ERR_BUFFER_FULL                     0x401B
#define HR_ERR_NO_SPACE                        0x401C
#define HR_ERR_NO_MESSAGES                     0x401D
#define HR_WRN_MODULE_NOT_SUPPORTED            0x401E
#define HR_ERR_NOT_INITIALIZED                 0x401F
#define HR_ERR_CONTRACT_LABEL_ALREADY_EXISTS   0x4021
#define HR_ERR_BUDGET_EXPIRED                  0x4022
#define HR_ERR_NOT_IMPLEMENTED                 0x4024
#define HR_ERR_CONTRACT_TYPE_NOT_COMPATIBLE    0x4025
#define HR_ERR_CAPACITY_NOT_DECREASING         0x4026
#define HR_ERR_CONTRACT_LABEL_UNKNOWN          0x4027
#define HR_ERR_OUT_OF_BUDGET 			 0x4028
#define HR_ERR_ALREADY_IN_HR		 0x4029

#define HR_ERR_LAST_VALUE                      0x4030

// for size_t
#include <string.h>

int hr_strerror (int error, char *message, size_t size);

/* ----------------------------------------------------------*/

/* ---------------- Configuration parameters ----------------*/

#define FRSH_CONTRACT_LABEL_MAXLENGTH      15

/* ----------------------------------------------------------*/
/* ------------------------ Time types ----------------------*/

#define hr_abs_time_t EE_TYPEABSDLINE
#define hr_rel_time_t EE_TYPERELDLINE

/* ----------------------------------------------------------*/
/* ------------------------ Core types ----------------------*/


/** identifier of a frsh thread **/
#define hr_thread_id_t EE_TID

#define hr_signal_t EE_UREG
#define hr_signal_info_t EE_UREG


/** Kind of workload expected in vres: bounded or indeterminate **/
typedef enum {
    HR_WT_BOUNDED       = 0,
    HR_WT_INDETERMINATE = 1,
    HR_WT_SYNCHRONIZED  = 2
} hr_workload_t;


#define HR_NULL_DEADLINE     0

#define hr_vres_id_t EE_TYPECONTRACT

/** Contract ressource type:  processor, network, memory **/
typedef enum {
  HR_RT_PROCESSOR = 0,
  HR_RT_NETWORK   = 1,
  HR_RT_MEMORY    = 2,
  HR_RT_DISK      = 3,
  HR_RT_FPGA      = 4
} hr_resource_type_t;

/** Ressource Id: processor_id or network_id **/
/**********************************************/
#define hr_resource_id_t EE_UREG

/** Kind of contract: regular, background or dummy **/
typedef enum {
        HR_CT_REGULAR    = 0,
        HR_CT_BACKGROUND = 1,
        HR_CT_DUMMY      = 2
} hr_contract_type_t;

/**
 *  Contract parameters type; it is an opaque type (i.e. the internal
 *  structure of this data type is implementation dependent). The user
 *  can access and modify the parameters of a contract only with the
 *  proper functions, and should never access the data directly.
 **/
#define  hr_contract_t EE_TYPECONTRACTSTRUCT

/* ----------------------------------------------------------*/



#endif 	    /* !HR_FRESCOR_H_ */
