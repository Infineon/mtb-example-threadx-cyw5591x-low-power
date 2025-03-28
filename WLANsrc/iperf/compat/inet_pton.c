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

/* IPERF_MODIFIED Start */
#include "headers.h"
#include "iperf_sockets.h"
/* IPERF_MODIFIED End */
#include "inet_aton.h"
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Copyright (C) 1996-2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef HAVE_INET_PTON
#define NS_INT16SZ       2
#define NS_INADDRSZ     4
#define NS_IN6ADDRSZ    16

/* int
 * isc_net_pton(af, src, dst)
 *      convert from presentation format (which usually means ASCII printable)
 *      to network format (which is usually some kind of binary format).
 * return:
 *      1 if the address was valid for the specified address family
 *      0 if the address wasn't valid (`dst' is untouched in this case)
 * author:
 *      Paul Vixie, 1996.
 */
int
inet_pton(int af,
          const char *src,
          void *dst) {
    switch ( af ) {
        case AF_INET:
            return(inet_pton4(src, dst));
#ifdef HAVE_IPV6
        case AF_INET6:
            return(inet_pton6(src, dst));
#endif
        default:
            return 0;
    }
    /* NOTREACHED */
}

/* int
 * inet_pton4(src, dst)
 *      like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *      1 if `src' is a valid dotted quad, else 0.
 * notice:
 *      does not touch `dst' unless it's returning 1.
 * author:
 *      Paul Vixie, 1996.
 */
int
inet_pton4(src, dst)
const char *src;
unsigned char *dst;
{
    static const char digits[] = "0123456789";
    int saw_digit, octets, ch;
    unsigned char tmp[NS_INADDRSZ], *tp;

    saw_digit = 0;
    octets = 0;
    *(tp = tmp) = 0;
    while ( (ch = *src++) != '\0' ) {
        const char *pch;

        if ( (pch = strchr(digits, ch)) != NULL ) {
            unsigned int new = *tp * 10 + (pch - digits);

            if ( new > 255 )
                return(0);
            *tp = new;
            if ( ! saw_digit ) {
                if ( ++octets > 4 )
                    return(0);
                saw_digit = 1;
            }
        } else if ( ch == '.' && saw_digit ) {
            if ( octets == 4 )
                return(0);
            *++tp = 0;
            saw_digit = 0;
        } else
            return(0);
    }
    if ( octets < 4 )
        return(0);
    memcpy(dst, tmp, NS_INADDRSZ);
    return(1);
}

/* int
 * inet_pton6(src, dst)
 *      convert presentation level address to network order binary form.
 * return:
 *      1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *      (1) does not touch `dst' unless it's returning 1.
 *      (2) :: in a full address is silently ignored.
 * credit:
 *      inspired by Mark Andrews.
 * author:
 *      Paul Vixie, 1996.
 */
#ifdef HAVE_IPV6
int
inet_pton6(src, dst)
const char *src;
unsigned char *dst;
{
    static const char xdigits_l[] = "0123456789abcdef",
    xdigits_u[] = "0123456789ABCDEF";
    unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
    const char *xdigits, *curtok;
    int ch, saw_xdigit;
    unsigned int val;

    memset((tp = tmp), '\0', NS_IN6ADDRSZ);
    endp = tp + NS_IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if ( *src == ':' )
        if ( *++src != ':' )
            return(0);
    curtok = src;
    saw_xdigit = 0;
    val = 0;
    while ( (ch = *src++) != '\0' ) {
        const char *pch;

        if ( (pch = strchr((xdigits = xdigits_l), ch)) == NULL )
            pch = strchr((xdigits = xdigits_u), ch);
        if ( pch != NULL ) {
            val <<= 4;
            val |= (pch - xdigits);
            if ( val > 0xffff )
                return(0);
            saw_xdigit = 1;
            continue;
        }
        if ( ch == ':' ) {
            curtok = src;
            if ( !saw_xdigit ) {
                if ( colonp )
                    return(0);
                colonp = tp;
                continue;
            }
            if ( tp + NS_INT16SZ > endp )
                return(0);
            *tp++ = (unsigned char) (val >> 8) & 0xff;
            *tp++ = (unsigned char) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
        if ( ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
             inet_pton4(curtok, tp) > 0 ) {
            tp += NS_INADDRSZ;
            saw_xdigit = 0;
            break;  /* '\0' was seen by inet_pton4(). */
        }
        return(0);
    }
    if ( saw_xdigit ) {
        if ( tp + NS_INT16SZ > endp )
            return(0);
        *tp++ = (unsigned char) (val >> 8) & 0xff;
        *tp++ = (unsigned char) val & 0xff;
    }
    if ( colonp != NULL ) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;
        int i;

        for ( i = 1; i <= n; i++ ) {
            endp[- i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if ( tp != endp )
        return(0);
    memcpy(dst, tmp, NS_IN6ADDRSZ);
    return(1);
}
#endif

#endif /* HAVE_INET_PTON */

#ifdef __cplusplus
} /* end extern "C" */
#endif
