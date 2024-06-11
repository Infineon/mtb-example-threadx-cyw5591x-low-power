/*******************************************************************************
* File Name: le_app_event_handler.h
*
* Description:
*   Header file for handling Bluetooth stack events at the application level
*
* Related Document: See Readme.md
*
*******************************************************************************
* Copyright 2021-2024, Cypress Semiconductor Corporation (an Infineon company) or
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

#ifndef LE_APP_EVENT_HANDLER_H_
#define LE_APP_EVENT_HANDLER_H_

/*******************************************************************************
*        Header Files
*******************************************************************************/
#include "GeneratedSource/cycfg_gap.h"
#include "cy_utils.h"
#include "le_app_gatts.h"
#include "le_app_user_interface.h"
#include "le_app_utils.h"
#include <inttypes.h>
/*******************************************************************************
*        Macro Definitions
*******************************************************************************/

/*******************************************************************************
*        External Variable Declarations
*******************************************************************************/
typedef enum LE_STATES
{
    LE_NOT_INITIALIZED,
    LE_INITIALIZED_IDLE,
    LE_ADVERTISING,
    LE_CONNECTED_IDLE,
    LE_ACTIVE
} le_states;

extern volatile le_states current_le_state;

/*******************************************************************************
*        Function Prototypes
*******************************************************************************/

/**************************************************************************************************
* Function Name: le_app_management_callback
***************************************************************************************************
* Summary:
*   This is a Bluetooth stack event handler function to receive management events from
*   the LE stack and process as per the application.
*
* Parameters:
*   wiced_bt_management_evt_t event             : LE event code of one byte length
*   wiced_bt_management_evt_data_t *p_event_data: Pointer to LE management event structures
*
* Return:
*  wiced_result_t: Error code from WICED_RESULT_LIST or BT_RESULT_LIST
*
*************************************************************************************************/
wiced_result_t le_app_management_callback(wiced_bt_management_evt_t event,
                                          wiced_bt_management_evt_data_t *p_event_data);

/**************************************************************************************************
* Function Name: le_app_connect_handler
***************************************************************************************************
* Summary:
*   This callback function handles connection status changes.
*
* Parameters:
*   wiced_bt_gatt_connection_status_t *p_conn_status  : Pointer to data that has connection details
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e in wiced_bt_gatt.h
*
**************************************************************************************************/
wiced_bt_gatt_status_t le_app_connect_handler(wiced_bt_gatt_connection_status_t *p_conn_status);

void le_adv_init(void);

#endif /* LE_APP_EVENT_HANDLER_H_ */
