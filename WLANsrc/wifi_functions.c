/******************************************************************************
 * File Name:   wifi_functions.c
 *
 * Description: This file contains declaration of task and functions related to
 *              WLAN operation.
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

/* Header file includes */
#include "cy_retarget_io.h"

#include "iperf_sockets.h"
#include "cy_network_mw_core.h"
#include "cyhal_uart.h"
/* RTOS header file */
#include "cyabs_rtos.h"


/* Wi-Fi connection manager header files */
#include "cy_wcm.h"

/* Standard C header file */
#include <stdio.h>
#include <string.h>

/* TCP server task header file. */
#include "cy_nw_helper.h"

/* Standard C header files */
#include <inttypes.h>
#include "wifi_functions.h"


static void event_callback(cy_wcm_event_t event, cy_wcm_event_data_t *event_data);
void print_scan_result(cy_wcm_scan_result_t *result);

/*******************************************************************************
 * Global Variables
 ********************************************************************************/
/* variable to track WLAN states*/
volatile wlan_states current_wlan_state=WLAN_NOT_INITIALIZED;
static cy_wcm_associated_ap_info_t ap_info;
/* Size of the peer socket address. */
uint32_t peer_addr_len;

/* This variable holds the value of the total number of the scan results that is
 * available in the scan callback function in the current scan.
 */
uint32_t num_scan_result;

static void print_ip4(uint32_t ip)
{
    unsigned char bytes[IPV4_ADDR_SIZE];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;

    printf("IP Address: %d.%d.%d.%d\n", bytes[0], bytes[1], bytes[2], bytes[3]);
}

/*******************************************************************************
 * Function Name: lp_wlan_init()
 *******************************************************************************
 * Summary:
 * Initializes WCM and wlan
 *
 *******************************************************************************/
void lp_wlan_init(void)
{
    cy_rslt_decode_t result;
    cy_wcm_config_t wifi_config = {.interface = WIFI_INTERFACE_TYPE};

    /* Initialize Wi-Fi connection manager. */
    result.raw = cy_wcm_init(&wifi_config);
    cy_wcm_register_event_callback(&event_callback);

    if (result.raw != CY_RSLT_SUCCESS)
    {

        printf("Wi-Fi Connection Manager initialization failed! Error code: 0x%08" PRIx32 "\n", (uint32_t)result.raw);
        CY_ASSERT(0);
    }
    printf("Wi-Fi Connection Manager initialized.\r\n");
    current_wlan_state=WLAN_IDLE;
}

static void event_callback(cy_wcm_event_t event, cy_wcm_event_data_t *event_data)
    {
        printf("######### Received event changed from wcm, event = %d #######\r\n", event);
        if(event == CY_WCM_EVENT_DISCONNECTED)
        {
            printf("Network is down! \n");
            current_wlan_state=WLAN_IDLE;
        }

        if(event == CY_WCM_EVENT_RECONNECTED)
        {
            printf("Network is up again! \n");
            current_wlan_state=WLAN_CONNECTED;
        }

        if(event == CY_WCM_EVENT_CONNECTING)
        {
            printf("Connecting to AP ... \n");
        }

        if(event == CY_WCM_EVENT_CONNECTED)
        {
            printf("Connected to AP and network is up !! \n");
            current_wlan_state=WLAN_CONNECTED;
        }

        if(event == CY_WCM_EVENT_CONNECT_FAILED)
        {
            printf("Connection to AP Failed ! \n");
            current_wlan_state=WLAN_IDLE;
        }

        if(event == CY_WCM_EVENT_IP_CHANGED)
        {
            cy_wcm_ip_address_t ip_addr;
            cy_wcm_get_ip_addr(CY_WCM_INTERFACE_TYPE_STA, &ip_addr);

            if(event_data->ip_addr.version == CY_WCM_IP_VER_V4)
            {
                print_ip4(event_data->ip_addr.ip.v4);
            }
        }

        if(event == CY_WCM_EVENT_STA_JOINED_SOFTAP)
        {
            printf("joined softAP \r\n");
            current_wlan_state=WLAN_CONNECTED;
        }

        if(event == CY_WCM_EVENT_STA_LEFT_SOFTAP)
        {
            printf("disconnected from softAP \r\n");
            current_wlan_state=WLAN_IDLE;
        }
    }

/*******************************************************************************
 * Function Name: read_uart_input
 *******************************************************************************
 * Summary:
 *  Function to read user input from UART terminal.
 *
 * Parameters:
 *  uint8_t* input_buffer_ptr: Pointer to input buffer
 *
 * Return:
 *  None
 *
 *******************************************************************************/
void read_uart_input(uint8_t *input_buffer_ptr)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    uint8_t *ptr = input_buffer_ptr;

    do
    {
        result = cy_rtos_queue_get( &xUARTQueue, ptr, CY_RTOS_NEVER_TIMEOUT);
        if (result == CY_RSLT_SUCCESS)
        {
            if ((*ptr == '\r') || (*ptr == '\n'))
            {
                printf("\n");
            }
            else
            {
                /* Echo the received character */
                cyhal_uart_putc(&cy_retarget_io_uart_obj, *ptr);
                if (*ptr != '\b')
                {
                    ptr++;
                }
                else if (ptr != input_buffer_ptr)
                {
                    ptr--;
                }
            }
        }

    } while ((*ptr != '\r') && (*ptr != '\n'));

    /* Terminate string with NULL character. */
    *ptr = '\0';
}

/*******************************************************************************
 * Function Name: connect_to_wifi_ap()
 *******************************************************************************
 * Summary:
 *  Connects to Wi-Fi AP using the user-provided credentials, retries up to a
 *  configured number of times until the connection succeeds.
 *
 *******************************************************************************/
cy_rslt_t connect_to_wifi_ap(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Note: Maximum length of the Wi-Fi SSID and password is set to
     * CY_WCM_MAX_SSID_LEN and CY_WCM_MAX_PASSPHRASE_LEN as defined in cy_wcm.h
     * file.
     */
    uint8_t ssid_input[CY_WCM_MAX_SSID_LEN];
    uint8_t password_input[CY_WCM_MAX_PASSPHRASE_LEN];

    /* Variables used by Wi-Fi connection manager.*/
    cy_wcm_connect_params_t wifi_conn_param;

    cy_wcm_ip_address_t ip_address;

    /* Variable to track the number of connection retries to the Wi-Fi AP specified
     * by WIFI_SSID macro.
     */
    int conn_retries = 0;


    /* Clear connection parameters */
    memset(&wifi_conn_param, 0, sizeof(cy_wcm_connect_params_t));

    /* Clear the SSID input buffer. */
    memset(ssid_input, 0, CY_WCM_MAX_SSID_LEN);

    printf("Please Enter SSID: \r\n");
    read_uart_input(ssid_input);
    memcpy(wifi_conn_param.ap_credentials.SSID, ssid_input, sizeof(ssid_input));

    /* Clear the Password input buffer. */
    memset(password_input, 0, CY_WCM_MAX_PASSPHRASE_LEN);

    /* For open security no need to input password*/
    if (WIFI_SECURITY_TYPE != CY_WCM_SECURITY_OPEN) {

      printf("Please Enter Password: \r\n");
      read_uart_input(password_input);
      memcpy(wifi_conn_param.ap_credentials.password, password_input,
             sizeof(password_input));
    }
    else
    {
      printf("Security settings for Access point is set to open, skipping "
             "password \r\n");
    }

    wifi_conn_param.ap_credentials.security = WIFI_SECURITY_TYPE;
    wifi_conn_param.band = WIFI_BAND;

#ifdef USE_STATIC_IP
    unsigned char gw[16] = "192.168.39.153";
    unsigned char nmask[16] = "255.255.255.0";
    unsigned char static_ipaddr[16] = "192.168.39.170";
    cy_wcm_ip_setting_t static_ip_settings;
    memset(&static_ip_settings, 0, sizeof(cy_wcm_ip_setting_t));
    cy_nw_str_to_ipv4((const char *)static_ipaddr, (cy_nw_ip_address_t *)&static_ip_settings.ip_address);
    cy_nw_str_to_ipv4((const char *)gw, (cy_nw_ip_address_t *)&static_ip_settings.gateway);
    cy_nw_str_to_ipv4((const char *)nmask, (cy_nw_ip_address_t *)&static_ip_settings.netmask);
    wifi_conn_param.static_ip_settings = &static_ip_settings;
#endif

    printf("Connecting to Wi-Fi Network: %s\n", wifi_conn_param.ap_credentials.SSID);

    /* Join the Wi-Fi AP. */
    for (conn_retries = 0; conn_retries < MAX_WIFI_CONN_RETRIES; conn_retries++)
    {
    memset(&ip_address, 0x00, sizeof(cy_wcm_ip_address_t));
        result = cy_wcm_connect_ap(&wifi_conn_param, &ip_address);

        if (result == CY_RSLT_SUCCESS)
        {
            printf("Successfully connected to Wi-Fi network '%s'.\n",
                   wifi_conn_param.ap_credentials.SSID);
            if (CY_WCM_IP_VER_V4 == ip_address.version)
            {
                print_ip4(ip_address.ip.v4);
            }
            current_wlan_state=WLAN_CONNECTED;
            cy_wcm_get_associated_ap_info(&ap_info);
            printf("Connected AP details Channel: %d, Channel width: %u, Signal strength(dBm): %d \r\n", ap_info.channel,ap_info.channel_width,ap_info.signal_strength);
            return result;
        }

        printf("Connection to Wi-Fi network failed with error code 0x%08" PRIx32 "\n."
               "Retrying in %d ms...\n",
               (uint32_t)result, WIFI_CONN_RETRY_INTERVAL_MSEC);

        uint8_t type = CY_RSLT_GET_TYPE(result);

        // See the "Modules" section for possible values
        uint16_t module_id = CY_RSLT_GET_MODULE(result);

        // Specific error codes are defined by each module
        uint16_t error_code = CY_RSLT_GET_CODE(result);

        printf("type=%d, module=%d, code=%d\n", type, module_id, error_code);

        cy_rtos_delay_milliseconds(WIFI_CONN_RETRY_INTERVAL_MSEC);
    }

    /* Stop retrying after maximum retry attempts. */
    printf("Exceeded maximum Wi-Fi connection attempts\n");

    return result;
}

/*******************************************************************************
 * Function Name: iperf_util_thread_app()
 *******************************************************************************
 * Summary:
 *  Starts Iperf server for simulating WLAN traffic.
 *
 *******************************************************************************/
void iperf_util_thread_app(cy_thread_arg_t arg)
{
    iperf_network_init(CY_WCM_INTERFACE_TYPE_STA);

    /*Supress warnings*/
    (void)arg;

    /* Iperf server settings*/
    int argc=8;
    char *new_argv[] = { "iperf_thread","-s","-w","20k","-p","5001","-i","1"};

    current_wlan_state=WLAN_ACTIVE;
    /*Start iperf session*/
    iperf( argc, new_argv);

    printf("IPERF Session End \r\n");

    current_wlan_state=WLAN_CONNECTED;
    cy_rtos_delay_milliseconds(1000);
    cy_rtos_thread_terminate(&iperf_task_handle);

}

/*******************************************************************************
 * Function Name: network_idle_task
 *******************************************************************************
 * Summary:
 * The task initializes the Wi-Fi, LPA (Low-Power Assist middleware) and the OLM
 * (Offload Manager). The Wi-Fi then joins with Access Point with the provided
 * SSID and PASSWORD.After successfully connecting to the network the task
 * suspends the Netx network stack indefinitely which helps RTOS to enter the
 * Idle state, and then eventually into deep-sleep power mode. The MCU will stay
 * in deep-sleep power mode till the network stack resumes. The network stack
 * resumes whenever any Tx/Rx activity detected in the EMAC interface (path
 * between Wi-Fi driver and network stack).
 *
 * Parameters:
 *  void *arg: Task specific arguments. Never used.
 *
 * Return:
 *  static void: Should never return.
 *
 ******************************************************************************/
void network_idle_task(cy_thread_arg_t arg)
{
    /*Variable to store network interface pointer*/
     NX_IP* wifi;

    /*cy_rslt_t result;*/
    /* Obtain the pointer to the NetX network interface. This pointer is used to
     * access the Wi-Fi driver interface to configure the WLAN power-save mode.
     */
    wifi = (NX_IP*)cy_network_get_nw_interface(CY_NETWORK_WIFI_STA_INTERFACE, 0U);

    while( true )
       {
           /* Suspend the network stack */
           wait_net_suspend(wifi,
                            CY_RTOS_NEVER_TIMEOUT,
                            NETWORK_INACTIVE_INTERVAL_MS,
                            NETWORK_INACTIVE_WINDOW_MS);

           /*
            * Safe delay to avoid race conditions when switching between
            * offload enable and disable states when network stack is suspended
            * and resumed by the offload manager.
            */
           cy_rtos_delay_milliseconds(NETWORK_SUSPEND_DELAY_MS);
       }
}

/*******************************************************************************
 * Function Name: scan_callback
 *******************************************************************************
 * Summary: The callback function which accumulates the scan results and forwards
 * them to print function.
 *
 * Parameters:
 *  cy_wcm_scan_result_t *result_ptr: Pointer to the scan result
 *  void *user_data: User data.
 *  cy_wcm_scan_status_t status: Status of scan completion.
 *
 * Return:
 *  void
 *
 ******************************************************************************/
void scan_callback(cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status)
{
    if (status == CY_WCM_SCAN_INCOMPLETE)
    {
        /* print only if data is available */
        if((strlen((const char *)result_ptr->SSID) != 0))
        {
            num_scan_result++;
            print_scan_result(result_ptr);
        }
    }
    else if((CY_WCM_SCAN_COMPLETE == status || MAX_SCAN_RESULTS == num_scan_result))
    {
        printf("==================================Scan Complete========================== \r\n");

        /* Reset the number of scan results to 0 for the next scan.*/
        num_scan_result = 0;
    }
}

/*******************************************************************************
 * Function Name: print_scan_result
 *******************************************************************************
 * Summary: This function prints the scan result accumulated by the scan
 * handler.
 *
 *
 * Parameters:
 *  cy_wcm_scan_result_t *result: Pointer to the scan result.
 *
 * Return:
 *  void
 *
 ******************************************************************************/
void print_scan_result(cy_wcm_scan_result_t *result)
{
    char* security_type_string;

    /* Convert the security type of the scan result to the corresponding
     * security string
     */
    switch (result->security)
    {
    case CY_WCM_SECURITY_OPEN:
        security_type_string = SECURITY_OPEN;
        break;
    case CY_WCM_SECURITY_WEP_PSK:
        security_type_string = SECURITY_WEP_PSK;
        break;
    case CY_WCM_SECURITY_WEP_SHARED:
        security_type_string = SECURITY_WEP_SHARED;
        break;
    case CY_WCM_SECURITY_WPA_TKIP_PSK:
        security_type_string = SECURITY_WEP_TKIP_PSK;
        break;
    case CY_WCM_SECURITY_WPA_AES_PSK:
        security_type_string = SECURITY_WPA_AES_PSK;
        break;
    case CY_WCM_SECURITY_WPA_MIXED_PSK:
        security_type_string = SECURITY_WPA_MIXED_PSK;
        break;
    case CY_WCM_SECURITY_WPA2_AES_PSK:
        security_type_string = SECURITY_WPA2_AES_PSK;
        break;
    case CY_WCM_SECURITY_WPA2_TKIP_PSK:
        security_type_string = SECURITY_WPA2_TKIP_PSK;
        break;
    case CY_WCM_SECURITY_WPA2_MIXED_PSK:
        security_type_string = SECURITY_WPA2_MIXED_PSK;
        break;
    case CY_WCM_SECURITY_WPA2_FBT_PSK:
        security_type_string = SECURITY_WPA2_FBT_PSK;
        break;
    case CY_WCM_SECURITY_WPA3_SAE:
        security_type_string = SECURITY_WPA3_SAE;
        break;
    case CY_WCM_SECURITY_WPA3_WPA2_PSK:
        security_type_string = SECURITY_WPA3_WPA2_PSK;
        break;
    case CY_WCM_SECURITY_IBSS_OPEN:
        security_type_string = SECURITY_IBSS_OPEN;
        break;
    case CY_WCM_SECURITY_WPS_SECURE:
        security_type_string = SECURITY_WPS_SECURE;
        break;
    case CY_WCM_SECURITY_UNKNOWN:
        security_type_string = SECURITY_UNKNOWN;
        break;
    case CY_WCM_SECURITY_WPA2_WPA_AES_PSK:
        security_type_string = SECURITY_WPA2_WPA_AES_PSK;
        break;
    case CY_WCM_SECURITY_WPA2_WPA_MIXED_PSK:
        security_type_string = SECURITY_WPA2_WPA_MIXED_PSK;
        break;
    case CY_WCM_SECURITY_WPA_TKIP_ENT:
        security_type_string = SECURITY_WPA_TKIP_ENT;
        break;
    case CY_WCM_SECURITY_WPA_AES_ENT:
        security_type_string = SECURITY_WPA_AES_ENT;
        break;
    case CY_WCM_SECURITY_WPA_MIXED_ENT:
        security_type_string = SECURITY_WPA_MIXED_ENT;
        break;
    case CY_WCM_SECURITY_WPA2_TKIP_ENT:
        security_type_string = SECURITY_WPA2_TKIP_ENT;
        break;
    case CY_WCM_SECURITY_WPA2_AES_ENT:
        security_type_string = SECURITY_WPA2_AES_ENT;
        break;
    case CY_WCM_SECURITY_WPA2_MIXED_ENT:
        security_type_string = SECURITY_WPA2_MIXED_ENT;
        break;
    case CY_WCM_SECURITY_WPA2_FBT_ENT:
        security_type_string = SECURITY_WPA2_FBT_ENT;
        break;
    default:
        security_type_string = SECURITY_UNKNOWN;
        break;
    }

    printf(" %2"PRIu32"   %-32s     %4d     %2d      %02X:%02X:%02X:%02X:%02X:%02X         %-15s\n",
           num_scan_result, result->SSID,
           result->signal_strength, result->channel, result->BSSID[0], result->BSSID[1],
           result->BSSID[2], result->BSSID[3], result->BSSID[4], result->BSSID[5],
           security_type_string);
}

/* [] END OF FILE */
