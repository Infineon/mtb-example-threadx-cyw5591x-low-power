/*******************************************************************************
* File Name: le_app_event_handler.c
*
* Description:
*             Source file for handling Bluetooth stack events at the application level
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

/*******************************************************************************
*        Header Files
*******************************************************************************/
#include "le_app_event_handler.h"
#include "le_app_user_interface.h"
/*******************************************************************************
*        Variable Definitions
*******************************************************************************/
static uint16_t                bt_connection_id = 0;
volatile le_states current_le_state=LE_NOT_INITIALIZED;

/*******************************************************************************
*        Function Prototypes
*******************************************************************************/
static void le_app_init(void);

/*******************************************************************************
*        Function Definitions
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
                                          wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t wiced_result = WICED_BT_ERROR;
    wiced_bt_device_address_t bda = { 0 };
    wiced_bt_ble_advert_mode_t *p_adv_mode = NULL;
    switch (event)
    {
        case BTM_ENABLED_EVT:
            /* Bluetooth Controller and Host Stack Enabled */
            wiced_result = p_event_data->enabled.status;
            if (WICED_BT_SUCCESS == wiced_result)
            {
                current_le_state=LE_INITIALIZED_IDLE;
                printf("Bluetooth Stack initialization successful \r\n");
                wiced_result = wiced_bt_set_local_bdaddr((uint8_t *)cy_bt_device_address, BLE_ADDR_PUBLIC);
                if (WICED_BT_SUCCESS == wiced_result)
                {
                    wiced_bt_dev_read_local_addr(bda);
                    printf("Local Bluetooth Address: ");
                    print_bd_address(bda);
                }
                else
                {
                    printf("failed to set local Bluetooth address\r\n");
                }

                le_app_init();
            }
            else
            {
                printf( " Failed to start Bluetooth \r\n" );
            }

            break;

        case BTM_BLE_ADVERT_STATE_CHANGED_EVT:
            /* Advertisement State Changed */
            p_adv_mode = &p_event_data->ble_advert_state_changed;
            printf("Advertisement State Change: %s\r\n", get_bt_advert_mode_name(*p_adv_mode));

            if (BTM_BLE_ADVERT_OFF == *p_adv_mode)
            {
                /* Advertisement Stopped */
                printf("Advertisement stopped\r\n");

                /* Check connection status after advertisement stops */
                if(0 == bt_connection_id)
                {
                    current_le_state=LE_INITIALIZED_IDLE;
                }
                else
                {
                    current_le_state=LE_CONNECTED_IDLE;
                }
            }
            else
            {
                /* Advertisement Started */
                printf("Advertisement started\r\n");
                current_le_state=LE_ADVERTISING;
            }
            wiced_result = WICED_BT_SUCCESS;
            break;

        case BTM_BLE_CONNECTION_PARAM_UPDATE:
            printf("Connection parameter update status:%d, Connection Interval: %d, Connection Latency: %d, Connection Timeout: %d\r\n",
                                           p_event_data->ble_connection_param_update.status,
                                           p_event_data->ble_connection_param_update.conn_interval,
                                           p_event_data->ble_connection_param_update.conn_latency,
                                           p_event_data->ble_connection_param_update.supervision_timeout);
            wiced_result = WICED_BT_SUCCESS;
            break;

        default:
            printf("Unhandled Bluetooth Management Event: 0x%x %s\r\n", event, get_btm_event_name(event));
            break;
    }

    return wiced_result;
}

static void le_app_init(void)
{
    cy_rslt_t rslt = CY_RSLT_SUCCESS;
    wiced_bt_gatt_status_t gatt_status = WICED_BT_GATT_ERROR;
    wiced_result_t wiced_result = WICED_BT_ERROR;

    wiced_bt_set_pairable_mode(FALSE, FALSE);

    /* Set Advertisement Data */
    wiced_result = wiced_bt_ble_set_raw_advertisement_data(CY_BT_ADV_PACKET_DATA_SIZE, cy_bt_adv_packet_data);
    if ( WICED_BT_SUCCESS != wiced_result )
    {
    printf("Failed to set advertising data\r\n");
        CY_ASSERT(0);
    }

    /* Register with BT stack to receive GATT callback */
    gatt_status = wiced_bt_gatt_register(le_app_gatt_event_callback);

    if ( WICED_BT_GATT_SUCCESS != gatt_status )
    {
        printf("GATT event Handler registration failed status: %s \r\n",get_bt_gatt_status_name(gatt_status));
    }

    /* Initialize GATT Database */
    gatt_status = wiced_bt_gatt_db_init(gatt_database, gatt_database_len, NULL);

    if ( WICED_BT_GATT_SUCCESS != gatt_status )
    {
        printf("GATT database initialization failed status: %s \r\n",get_bt_gatt_status_name(gatt_status));
    }
#ifdef CYBSP_USER_LED1
    /* Initialize the PWM used for IAS alert level LED */
    cyhal_clock_t clock_temp = {CYHAL_CLOCK_BLOCK_CPU, 0, false};
    rslt = cyhal_pwm_init_adv(&ias_led_pwm, CYBSP_USER_LED1 , NC, CYHAL_PWM_RIGHT_ALIGN, true, 0u, true, &clock_temp);
    if (CY_RSLT_SUCCESS != rslt  )
    {
        printf("IAS LED PWM Initialization has failed! 0x%08" PRIx32 "\r\n", rslt);

#endif

    }
}

/**************************************************************************************************
* Function Name: le_adv_init
***************************************************************************************************
* Summary:
*   This function handles application level initialization tasks and is called from the BT
*   management callback once the LE stack enabled event (BTM_ENABLED_EVT) is triggered
*   This function is executed in the BTM_ENABLED_EVT management callback.
*
* Parameters:
*   None
*
* Return:
*  None
*
*************************************************************************************************/
void le_adv_init(void)
{
    wiced_result_t wiced_result = WICED_BT_ERROR;

    /* Start Undirected LE Advertisements on device startup.
     * The corresponding parameters are contained in 'app_bt_cfg.c' */
    wiced_result = wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);
    /* Failed to start advertisement. Stop program execution */
    if (WICED_BT_SUCCESS != wiced_result)
    {
        printf("failed to start advertisement! \r\n");
        CY_ASSERT(0);
    }
    else
    {
    current_le_state=LE_ADVERTISING;
        printf("\n***********************************************\r\n");
        printf("**Discover device with \"Low Power\" name*\r\n");
        printf("***********************************************\r\n\r\n");
    }

}

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
wiced_bt_gatt_status_t le_app_connect_handler(wiced_bt_gatt_connection_status_t *p_conn_status)
{
    wiced_bt_gatt_status_t gatt_status = WICED_BT_GATT_ERROR;

    if ( NULL != p_conn_status )
    {
        if ( p_conn_status->connected )
        {
            /* Device has connected */
            printf("Connected : BDA " );
            print_bd_address(p_conn_status->bd_addr);
            printf("Connection ID '%d' \r\n", p_conn_status->conn_id );

            /* Store the connection ID */
            bt_connection_id = p_conn_status->conn_id;

            /* Update the adv/conn state */
            current_le_state=LE_CONNECTED_IDLE;
        }
        else
        {
            /* Device has disconnected */
            printf("Disconnected : BDA " );
            print_bd_address(p_conn_status->bd_addr);
            printf("Connection ID '%d', Reason '%s'\r\n", p_conn_status->conn_id, get_bt_gatt_disconn_reason_name(p_conn_status->reason) );

            /* Set the connection id to zero to indicate disconnected state */
            bt_connection_id = 0;

            /* Update the adv/conn state */
            current_le_state=LE_INITIALIZED_IDLE;
            
            /* Set Alert level back to Low to stop PWM. Active PWM prevents device from sleep */
            app_ias_alert_level[0]=0;

            /* Turn Off the IAS LED on a disconnection */
            ias_led_update();
        }
        gatt_status = WICED_BT_GATT_SUCCESS ;
    }

    return gatt_status;
}

/* [] END OF FILE */
