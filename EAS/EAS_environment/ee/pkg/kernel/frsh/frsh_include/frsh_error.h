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


/* Note for ERIKA Enterprise users: This is a reduced version of the
   frsh_error.h file. In particular, the macros have been commented out. */


#ifndef   	FRSH_ERROR_H_
#define   	FRSH_ERROR_H_

/**
 * @file frsh_error.h
 **/

/**
 * addtogroup core
 *
 * @{
 **/

/* Error codes */
#define FRSH_NO_ERROR                            0

#define FRSH_ERR_BASE_VALUE                      0x4000

#define FRSH_ERR_TOO_MANY_TASKS                  0x4001
#define FRSH_ERR_BAD_ARGUMENT                    0x4002
#define FRSH_ERR_INVALID_SYNCH_OBJ_HANDLE        0x4003
#define FRSH_ERR_NO_RENEGOTIATION_REQUESTED      0x4004
#define FRSH_ERR_CONTRACT_REJECTED               0x4005
#define FRSH_ERR_NOT_SCHEDULED_CALLING_THREAD    0x4006
#define FRSH_ERR_NOT_BOUND                       0x4007
#define FRSH_ERR_UNKNOWN_SCHEDULED_THREAD        0x4008
#define FRSH_ERR_NOT_CONTRACTED_VRES             0x4009
#define FRSH_ERR_NOT_SCHEDULED_THREAD            0x400A
#define FRSH_ERR_TOO_MANY_SERVICE_JOBS           0x400B
#define FRSH_ERR_TOO_MANY_SYNCH_OBJS             0x400C
#define FRSH_ERR_TOO_MANY_VRES_IN_SYNCH_OBJ      0x400D
#define FRSH_ERR_TOO_MANY_EVENTS_IN_SYNCH_OBJ    0x400E
#define FRSH_ERR_INTERNAL_ERROR                  0x400F
#define FRSH_ERR_TOO_MANY_VRES                   0x4010
#define FRSH_ERR_INVALID_SCHEDULER_REPLY         0x4011
#define FRSH_ERR_TOO_MANY_PENDING_REPLENISHMENTS 0x4012
#define FRSH_ERR_SYSTEM_ALREADY_INITIALIZED      0x4013
#define FRSH_ERR_SHARED_OBJ_ALREADY_INITIALIZED  0x4014
#define FRSH_ERR_SHARED_OBJ_NOT_INITIALIZED      0x4015
#define FRSH_ERR_SCHED_POLICY_NOT_COMPATIBLE     0x4016
#define FRSH_ERR_VRES_WORKLOAD_NOT_COMPATIBLE    0x4017
#define FRSH_ERR_ALREADY_BOUND                   0x4018
#define FRSH_ERR_RESOURCE_ID_INVALID             0x4019
#define FRSH_ERR_TOO_LARGE                       0x401A
#define FRSH_ERR_BUFFER_FULL                     0x401B
#define FRSH_ERR_NO_SPACE                        0x401C
#define FRSH_ERR_NO_MESSAGES                     0x401D
#define FRSH_WRN_MODULE_NOT_SUPPORTED            0x401E
#define FRSH_ERR_NOT_INITIALIZED                 0x401F
#define FRSH_ERR_TOO_MANY_SHARED_OBJS            0x4020
#define FRSH_ERR_CONTRACT_LABEL_ALREADY_EXISTS   0x4021
#define FRSH_ERR_BUDGET_EXPIRED                  0x4022
#define FRSH_ERR_SHARED_OBJECT_NOT_PROTECTED     0x4023
#define FRSH_ERR_NOT_IMPLEMENTED                 0x4024
#define FRSH_ERR_CONTRACT_TYPE_NOT_COMPATIBLE    0x4025
#define FRSH_ERR_CAPACITY_NOT_DECREASING         0x4026
#define FRSH_ERR_CONTRACT_LABEL_UNKNOWN          0x4027
#define FRSH_ERR_OUT_OF_BUDGET 			 0x4028
#define FRSH_ERR_ALREADY_IN_FRSH		 0x4029

#define FRSH_ERR_LAST_VALUE                      0x4030

/* #define ERROR(nn,ss) do {if(nn>FRSH_ERR_BASE_VALUE) my_frsh_strerror(nn, ss); else perror(ss); exit (nn);} while (0) */

// for size_t
#include <string.h>

int frsh_strerror (int error, char *message, size_t size);
/* void my_frsh_strerror(int error, char *sss); */

/**
 * This str_helper is needed to ensure argument expansion,
 * see http://www.iar.com/p180591/p180591_eng.php
 **/
/* #define STR_HELPER(x) #x */

/**
 * PERROR_FRESCOR
 *
 * This macro checks the given error number and composes a messages accordingly.
 *
 * @param nn  Error number
 * @param ss  Error string (to be appended to FRSH or system error string)
 **/
/* #define PERROR_FRESCOR(_nn_,_ss_) do {   	\ */
/*  \ */
/*         char error_string[1024]; \ */
/*  \ */
/*         sprintf(error_string,  "File: %s, in function %s at line %d, error %d: %s\n", __FILE__, __FUNCTION__, __LINE__, (_nn_), _ss_); \ */
/*  \ */
/* 		if( (_nn_)>FRSH_ERR_BASE_VALUE ) {	\ */
/* 			my_frsh_strerror( (_nn_), error_string);	\ */
/* 		} else {				\ */
/* 			perror(error_string);			\ */
/* 		}					\ */
/* } while(0) */



/**
 * PERROR_AND_RETURN
 *
 * Macro that displays an error code and message and then returns from
 * the current function
 *
 * @param nn  Error number
 * @param ss  Error string (to be appended to FRSH or system error string)
 **/
/* #define PERROR_AND_RETURN(nn,ss) do {			\ */
/*         PERROR_FRESCOR(nn, ss); \ */
/* 		return (nn);				\ */
/* 	} while (0) */




/**
 * PERROR_AND_EXIT
 *
 * Macro that displays an error code and message and then aborts the
 * program.
 *
 * @param nn  Error number
 * @param ss  Error string (to be appended to FRSH or system error string)
 **/
/* #define PERROR_AND_EXIT(nn,ss) do {	\ */
/* 		PERROR_FRESCOR(nn,ss);		\ */
/* 		exit(nn);		\ */
/* 	} while (0) */


/**
 * PRW:  Perror and Return Wrapper
 *
 * Function that calls funccall and checks the result != 0.
 * In case of error it displays the error code with the function call
 * as extra error string and returns from the function.
 *
 * terror (int) needs to be visible in the point of call.
 *
 * @param funccall Code to execute that should return 0 in a non error
 *                 case.
 **/
/* #define PRW(funccall)  do { \ */
/*   if ( (terror = funccall ) != 0) \ */
/*   { \ */
/*      PERROR_AND_RETURN( terror, STR_HELPER(funccall) ); \ */
/*   } \ */
/* } while(0) */



/**
 * PXW:  Perror and eXit Wrapper
 *
 * Function that calls funccall and checks the result != 0.
 * In case of error it displays the error code with the function call
 * as extra error string and ABORTS the program.
 *
 * terror (int) needs to be visible in the point of call.
 *
 * @param funccall Code to execute that should return 0 in a non error
 *                 case.
 **/
/* #define PXW(funccall)  do { \ */
/*   if ( (terror = funccall ) != 0) \ */
/*   { \ */
/*      PERROR_AND_EXIT( terror, STR_HELPER(funccall) ); \ */
/*   } \ */
/* } while(0) */



/**
 * PERROR_KERN_AND_EXIT
 *
 * Function that displays an error code and message and then aborts the
 * program.
 *
 * @param nn  Error number
 * @param ss  Error string (to be appended to FRSH or system error string)
 **/
/* #define PERROR_KERN_AND_EXIT(nn, ss) do {		\ */
/*  		errno = errno ?	errno : (nn);		\ */
/* 		kern_printf(ss);			\ */
/*  		exit(nn);				\ */
/* 	} while (0) */




/*}*/

#endif 	    /* !FRSH_ERROR_H_ */
