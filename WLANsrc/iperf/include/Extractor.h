/*
* Copyright 2025, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*/

/*---------------------------------------------------------------
 * Copyright (c) 1999,2000,2001,2002,2003
 * The Board of Trustees of the University of Illinois
 * All Rights Reserved.
 *---------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software (Iperf) and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 *
 * Redistributions of source code must retain the above
 * copyright notice, this list of conditions and
 * the following disclaimers.
 *
 *
 * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimers in the documentation and/or other materials
 * provided with the distribution.
 *
 *
 * Neither the names of the University of Illinois, NCSA,
 * nor the names of its contributors may be used to endorse
 * or promote products derived from this Software without
 * specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ________________________________________________________________
 * National Laboratory for Applied Network Research
 * National Center for Supercomputing Applications
 * University of Illinois at Urbana-Champaign
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________
 *
 * Extractor.h
 * by Ajay Tirumala (tirumala@ncsa.uiuc.edu)
 * -------------------------------------------------------------------
 * Extract data from a file, used to measure the transfer rates
 * for various stream formats.
 *
 * E.g. Use a gzipped file to measure the transfer rates for
 * compressed data
 * Use an MPEG file to measure the transfer rates of
 * Multimedia data formats
 * Use a plain BMP file to measure the transfer rates of
 * Uncompressed data
 *
 * This is beneficial especially in measuring bandwidth across WAN
 * links where data compression takes place before data transmission
 * ------------------------------------------------------------------- */

/* IPERF_MODIFIED Start */
#if !defined(_EXTRACTOR_H) && !defined(NO_FILE_IO)
/* IPERF_MODIFIED End */
#define _EXTRACTOR_H

#include <stdlib.h>
#include <stdio.h>
#include "Settings.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Constructor
     * @arg fileName   Name of the file
     * @arg size       Block size for reading
     */
    void Extractor_Initialize( char *fileName, int size, thread_Settings *mSettings );

    /**
     * Constructor
     * @arg fp         File Pointer
     * @arg size       Block size for reading
     */
    void Extractor_InitializeFile( FILE *fp, int size, thread_Settings *mSettings );


    /*
     * Fetches the next data block from
     * the file
     * @arg block     Pointer to the data read
     * @return        Number of bytes read
     */
    int Extractor_getNextDataBlock( char *block, thread_Settings *mSettings );


    /**
     * Function which determines whether
     * the file stream is still readable
     * @return true, if readable; false, if not
     */
    int Extractor_canRead( thread_Settings *mSettings );

    /**
     * This is used to reduce the read size
     * Used in UDP transfer to accomodate the
     * the header (timestamp)
     * @arg delta         Size to reduce
     */
    void Extractor_reduceReadSize( int delta, thread_Settings *mSettings );

    /**
     * Destructor
     */
    void Extractor_Destroy( thread_Settings *mSettings );
#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif
