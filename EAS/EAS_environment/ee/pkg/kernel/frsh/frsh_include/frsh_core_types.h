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
//frsh_core_types.h
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

/* this is a reduced version of the frsh_core_types.h as defined by
   the IST FRESCOR Project
*/

#ifndef   FRSH_CORE_TYPES_H_
#define   FRSH_CORE_TYPES_H_

#include "frsh_time_types.h"


/** identifier of a frsh thread **/
#define frsh_thread_id_t EE_TID


/** thread attributes object **/
//typedef fosa_thread_attr_t frsh_thread_attr_t;

/**
 *  The type references a function that may become a thread's
 *  code
 **/
//typedef fosa_thread_code_t frsh_thread_code_t;


// EE does not handle signals
#define frsh_signal_t EE_UREG
#define frsh_signal_info_t EE_UREG


/** Kind of workload expected in vres: bounded or indeterminate **/
typedef enum {
    FRSH_WT_BOUNDED       = 0,
    FRSH_WT_INDETERMINATE = 1,
    FRSH_WT_SYNCHRONIZED  = 2
} frsh_workload_t;


#define FRSH_NULL_DEADLINE     0
// #define FRSH_NULL_SIGNAL       0 /* Defined at frsh_adaption.h */

/** Negotiation status: In_progress, rejected, admitted, not_requested **/
//typedef enum {FRSH_RS_IN_PROGRESS,
//            FRSH_RS_REJECTED,
//            FRSH_RS_ADMITTED,
//            FRSH_RS_NOT_REQUESTED} frsh_renegotiation_status_t;

/**
 *  Vres Id type, that identifies a vres created to manage a given
 *  contract
 **/
#define frsh_vres_id_t EE_TYPECONTRACT

/** Contract ressource type:  processor, network, memory **/
typedef enum {
  FRSH_RT_PROCESSOR = 0,
  FRSH_RT_NETWORK   = 1,
  FRSH_RT_MEMORY    = 2,
  FRSH_RT_DISK      = 3,
  FRSH_RT_FPGA      = 4
} frsh_resource_type_t;

/** Ressource Id: processor_id or network_id **/
/**********************************************/
#define frsh_resource_id_t EE_UREG

/** Kind of contract: regular, background or dummy **/
typedef enum {
        FRSH_CT_REGULAR    = 0,
        FRSH_CT_BACKGROUND = 1,
        FRSH_CT_DUMMY      = 2
} frsh_contract_type_t;

// The name of this constant has been updated to have the FRSH_ prefix
//typedef char frsh_contract_label_t[FRSH_CONTRACT_LABEL_MAXLENGTH + 1];

/**
 *  Contract parameters type; it is an opaque type (i.e. the internal
 *  structure of this data type is implementation dependent). The user
 *  can access and modify the parameters of a contract only with the
 *  proper functions, and should never access the data directly.
 **/
#define  frsh_contract_t EE_TYPECONTRACTSTRUCT


/** List of vres **/
//typedef  struct {
//  int             size;
//  frsh_vres_id_t  vres[FRSH_MAX_GROUP_OPS];
//} frsh_vres_group_t;

/** List of contracts to negotiate **/
//typedef struct {
//    int  size;
//    frsh_contract_t contracts[FRSH_MAX_GROUP_OPS];
//} frsh_contracts_group_t;

//typedef FRSH_GROUP_ID_T_OPAQUE frsh_group_id_t;

/**
 * An abstract synchronization object is defined by the application.
 * This object can be used by an application to wait for an event to
 * arrive by invoking the frsh_sychobj_wait() operation.  It
 * can also be used to signal the event either causing a waiting
 * vres to wake up, or the event to be queued if no vres is
 * waiting for it.
 **/
//typedef FRSH_SYNCHOBJ_HANDLE_T_OPAQUE frsh_synchobj_handle_t;



#endif   /* !FRSH_CORE_TYPES_H_ */
