/******************************************************************************
 * File Name:   tcp_server.h
 *
 * Description: This file contains declaration of task and functions related to
 * TCP server operation.
 *
 * Related Document: See README.md
 *
 *
 *******************************************************************************
 * Copyright 2020-2023, Cypress Semiconductor Corporation (an Infineon company) or
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
 *******************************************************************************/
#pragma once

#ifndef WIFI_TASK_H
#define WIFI_TASK_H
#include "cy_wcm.h"
/* Low Power Assistant header files. */
#include "network_activity_handler.h"

/* Wi-Fi Host Driver (WHD) header files. */
#include "whd_wifi_api.h"
#include "cyabs_rtos.h"

/*******************************************************************************
 * Macros
 ********************************************************************************/

#define MAKE_IP_PARAMETERS(a, b, c, d) ((((uint32_t)d) << 24) | \
                                        (((uint32_t)c) << 16) | \
                                        (((uint32_t)b) << 8) |  \
                                        ((uint32_t)a))

#define IP_ADDR_BUFFER_SIZE (20u)

#define WIFI_INTERFACE_TYPE CY_WCM_INTERFACE_TYPE_STA
#define IPV4_ADDR_SIZE          4

/* Wi-Fi Credentials: Modify WIFI_SSID, WIFI_PASSWORD, and WIFI_SECURITY_TYPE
 * to match your Wi-Fi network credentials.
 * Note: Maximum length of the Wi-Fi SSID and password is set to
 * CY_WCM_MAX_SSID_LEN and CY_WCM_MAX_PASSPHRASE_LEN as defined in cy_wcm.h file.
 */
#define WIFI_SSID "LowPower"
#define WIFI_PASSWORD "password"
#define WIFI_BAND  CY_WCM_WIFI_BAND_ANY
/* Security type of the Wi-Fi access point. See 'cy_wcm_security_t' structure
 * in "cy_wcm.h" for more details.
 */
#define WIFI_SECURITY_TYPE  CY_WCM_SECURITY_WPA3_WPA2_PSK
/* Maximum number of connection retries to a Wi-Fi network. */
#define MAX_WIFI_CONN_RETRIES (10u)

/* Wi-Fi re-connection time interval in milliseconds */
#define WIFI_CONN_RETRY_INTERVAL_MSEC (1000u)


/* Interrupt priority of the user button. */
#define USER_BTN_INTR_PRIORITY (5)

/* Debounce delay for user button. */
#define DEBOUNCE_DELAY_MS (50)

#define IPERF_TASK_STACK_SIZE      4096
#define NETWORK_IDLE_TASK_STACK_SIZE      1024


/*lpa defines*/
/*
 * Safe delay to avoid race conditions when switching between
 * offload enable and disable states when network stack is suspended
 * and resumed by the offload manager.
 */
#define NETWORK_SUSPEND_DELAY_MS           (100)

/* This macro specifies the interval in milliseconds that the device monitors
 * the network for inactivity. If the network is inactive for duration lesser
 * than INACTIVE_WINDOW_MS in this interval, the MCU does not suspend the network
 * stack and informs the calling function that the MCU wait period timed out
 * while waiting for network to become inactive.
 */
#define NETWORK_INACTIVE_INTERVAL_MS              3000u

/* This macro specifies the continuous duration in milliseconds for which the
 * network has to be inactive. If the network is inactive for this duaration,
 * the MCU will suspend the network stack. Now, the MCU will not need to service
 * the network timers which allows it to stay longer in sleep/deepsleep.
 */
#define NETWORK_INACTIVE_WINDOW_MS                2000u

/* This macro defines the power-save mode that the WLAN device has to be
 * configured to. The valid values for this macro are POWERSAVE_WITH_THROUGHPUT,
 * POWERSAVE_WITHOUT_THROUGHPUT, POWERSAVE_DISABLED
 * which are defined in the enumeration wlan_powersave_mode_t.
 */
#define WLAN_POWERSAVE_MODE               POWERSAVE_WITH_THROUGHPUT

/* This macro specifies the duration in milliseconds for which the STA stays
 * awake after receiving frames from AP in PM2 mode. The delay value must be set
 * to a multiple of 10 and not equal to zero. Minimum value is 10u. Maximum
 * value is 2000u.
 */
#define RETURN_TO_SLEEP_MS                (10u)

/* NOTE: USER_LED is defined in template BSPs, and select an LED that is not
 * connected to P6_VDD (MCU supply) on the board so that the current consumption
 * measurement for MCU is not affected by the LED current.
 */

#define APP_INFO( x )           do { printf("Info: "); printf x;} while(0);
#define ERR_INFO( x )           do { printf("Error: "); printf x;} while(0);
#define CHECK_RESULT( x )       do { if(CY_RSLT_SUCCESS != x) { CY_ASSERT(0); } } while(0);

/* This data type enlists enumerations that correspond to the different
 * power-save modes available. They are,
 * POWERSAVE_WITHOUT_THROUGHPUT:This mode corresponds to (legacy) 802.11 PS-Poll
 * mode and should be used to achieve the lowest power consumption possible when
 * the Wi-Fi device is primarily passively listening to the network.
 *
 * POWERSAVE_WITH_THROUGHPUT:This mode attempts to increase throughput by
 * waiting for a timeout period before returning to sleep rather than returning
 * to sleep immediately.
 *
 * POWERSAVE_DISABLED: Powersave mode is disabled.
 */
enum wlan_powersave_mode_t
{
   POWERSAVE_WITHOUT_THROUGHPUT,
   POWERSAVE_WITH_THROUGHPUT,
   POWERSAVE_DISABLED
};

#define MAX_SCAN_RESULTS                        (108)

#define MAX_SECURITY_STRING_LENGTH              (15)

#define SECURITY_OPEN                           "OPEN"
#define SECURITY_WEP_PSK                        "WEP-PSK"
#define SECURITY_WEP_SHARED                     "WEP-SHARED"
#define SECURITY_WEP_TKIP_PSK                   "WEP-TKIP-PSK"
#define SECURITY_WPA_TKIP_PSK                   "WPA-TKIP-PSK"
#define SECURITY_WPA_AES_PSK                    "WPA-AES-PSK"
#define SECURITY_WPA_MIXED_PSK                  "WPA-MIXED-PSK"
#define SECURITY_WPA2_AES_PSK                   "WPA2-AES-PSK"
#define SECURITY_WPA2_TKIP_PSK                  "WPA2-TKIP-PSK"
#define SECURITY_WPA2_MIXED_PSK                 "WPA2-MIXED-PSK"
#define SECURITY_WPA2_FBT_PSK                   "WPA2-FBT-PSK"
#define SECURITY_WPA3_SAE                       "WPA3-SAE"
#define SECURITY_WPA2_WPA_AES_PSK               "WPA2-WPA-AES-PSK"
#define SECURITY_WPA2_WPA_MIXED_PSK             "WPA2-WPA-MIXED-PSK"
#define SECURITY_WPA3_WPA2_PSK                  "WPA3-WPA2-PSK"
#define SECURITY_WPA_TKIP_ENT                   "WPA-TKIP-ENT"
#define SECURITY_WPA_AES_ENT                    "WPA-AES-ENT"
#define SECURITY_WPA_MIXED_ENT                  "WPA-MIXED-ENT"
#define SECURITY_WPA2_TKIP_ENT                  "WPA2-TKIP-ENT"
#define SECURITY_WPA2_AES_ENT                   "WPA2-AES-ENT"
#define SECURITY_WPA2_MIXED_ENT                 "WPA2-MIXED-ENT"
#define SECURITY_WPA2_FBT_ENT                   "WPA2-FBT-ENT"
#define SECURITY_IBSS_OPEN                      "IBSS-OPEN"
#define SECURITY_WPS_SECURE                     "WPS-SECURE"
#define SECURITY_UNKNOWN                        "UNKNOWN"

#define PRINT_SCAN_TEMPLATE()                   printf("\n----------------------------------------------------------------------------------------------------\n" \
                                                "  #                  SSID                  RSSI   Channel       MAC Address              Security\n" \
                                                "----------------------------------------------------------------------------------------------------\n");

/*******************************************************************************
* Global Variables
*******************************************************************************/

typedef enum WLAN_STATES
{
   WLAN_NOT_INITIALIZED,
   WLAN_IDLE,
   WLAN_CONNECTED,
   WLAN_ACTIVE
}wlan_states;

extern cy_thread_t wifi_task_handle;
extern cy_thread_t iperf_task_handle;
extern volatile wlan_states current_wlan_state;
extern cy_queue_t xUARTQueue;

/*******************************************************************************
 * Function Prototypes
 ********************************************************************************/
cy_rslt_t          connect_to_wifi_ap(void);
void               iperf_util_thread_app(cy_thread_arg_t arg);
void               lp_wlan_init(void);
void               network_idle_task(cy_thread_arg_t arg);
extern int         iperf( int argc, char **argv );
extern cy_rslt_t   cy_wcm_get_whd_interface(cy_wcm_interface_t interface_type, whd_interface_t *whd_iface);
void               scan_callback(cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status);
#endif /* WIFI_TASK_H */
