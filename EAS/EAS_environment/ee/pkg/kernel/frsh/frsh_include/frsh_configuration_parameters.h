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
//frsh_configuration_parameters.h
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

/* This is a reduced version used for ERIKA Enterprise */

#ifndef _FRSH_CONFIGURATION_PARAMETERS_H_
#define _FRSH_CONFIGURATION_PARAMETERS_H_

/* #define FRSH_ADMISSION_TEST_IS_ENABLED        false */
/* #define FRSH_AUTOMATIC_PRIO_ASSIGN_ENABLE false */
/* #define FRSH_DISTRIBUTED_MODULE_SUPPORTED false */
/* #define FRSH_RESOURCE_ID_DEFAULT 0 */
/* #define FRSH_CPU_ID_DEFAULT 0 */
/* #define FRSH_NETWORK_ID_DEFAULT 0 */

/* /\** Maximum number of accepted contracts (vres) **\/ */
/* #define FRSH_MAX_N_VRES                    EE_MAX_CONTRACT */

/* /\** Maximum number of threads that may be scheduled by the framework **\/ */
/* #define FRSH_MAX_N_THREADS                 EE_MAX_TASK */

/* /\** */
/*  * Maximum number of critical sections that can be stored in a */
/*  * contract parameters object */
/*  **\/ */
/* #define FRSH_MAX_N_CRITICAL_SECTIONS       0 */

/* /\** */
/*  * Maximum number of memory areas that can be specified for a */
/*  * wite operation in a critical section */
/*  **\/ */
/* #define FRSH_MAX_N_MEMORY_AREAS            0 */

/* /\** */
/*  * Maximum number of utilization values (pairs of budget and period) */
/*  * that can be stored in a contract parameters object */
/*  **\/ */
/* #define FRSH_MAX_N_UTILIZATION_VALUES      0 */

/* /\** Number of importance levels for spare capacity allocation **\/ */
/* #define FRSH_N_IMPORTANCE_LEVELS           0 */

/**
 * Maximum number of synchronization objects
 **/
#define FRSH_MAX_N_SYNCH_OBJECTS           EE_MAX_SEM

/* /\** Maximum number of shared objects **\/ */
/* #define FRSH_MAX_N_SHARED_OBJECTS          0 */

/* /\** Maximum number of send and receive endpoints in a single node **\/ */
/* #define FRSH_MAX_N_ENDPOINTS 10 */



#define FRSH_CONTRACT_LABEL_MAXLENGTH      15

#endif /* _FRSH_CONFIGURATION_PARAMETERS_H_ */

