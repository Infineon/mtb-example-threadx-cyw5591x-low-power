/******************************************************************************
* File Name:   app_gatts.c
*
* Description: This file contains GATT related functions for all devices
*
* Related Document: None
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
#include "le_app_gatts.h"
#include "le_app_event_handler.h"
#include "le_app_utils.h"

/*******************************************************************************
*        Variable Definitions
*******************************************************************************/
/*******************************************************************************
*        Function Prototypes
*******************************************************************************/
static wiced_bt_gatt_status_t le_app_server_handler(wiced_bt_gatt_attribute_request_t *p_attr_req);
static wiced_bt_gatt_status_t le_app_read_handler(uint16_t conn_id,
                                                    wiced_bt_gatt_opcode_t opcode,
                                                    wiced_bt_gatt_read_t *p_read_req,
                                                    uint16_t len_req);
static wiced_bt_gatt_status_t le_app_write_handler(uint16_t conn_id,
                                                    wiced_bt_gatt_opcode_t opcode,
                                                    wiced_bt_gatt_write_req_t *p_write_req,
                                                    uint16_t len_req);
static wiced_bt_gatt_status_t le_app_gatt_req_read_by_type_handler(uint16_t conn_id,
                                                                   wiced_bt_gatt_opcode_t opcode,
                                                                   wiced_bt_gatt_read_by_type_t *p_read_req,
                                                                   uint16_t len_requested);
static wiced_bt_gatt_status_t le_app_set_value(uint16_t attr_handle,
                                                uint8_t *p_val,
                                                uint16_t len);
static gatt_db_lookup_table_t  *le_app_find_by_handle(uint16_t handle);
/*******************************************************************************
*        Function Definitions
*******************************************************************************/

/**************************************************************************************************
* Function Name: le_app_gatt_event_callback
***************************************************************************************************
* Summary:
*   This function handles GATT events from the BT stack.
*
* Parameters:
*   wiced_bt_gatt_evt_t event                   : LE GATT event code of one byte length
*   wiced_bt_gatt_event_data_t *p_event_data    : Pointer to LE GATT event structures
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e in wiced_bt_gatt.h
*
**************************************************************************************************/
wiced_bt_gatt_status_t le_app_gatt_event_callback(wiced_bt_gatt_evt_t event,
                                                         wiced_bt_gatt_event_data_t *p_event_data)
{
    wiced_bt_gatt_status_t gatt_status = WICED_BT_GATT_ERROR;
    wiced_bt_gatt_attribute_request_t *p_attr_req = &p_event_data->attribute_request;
    /* Call the appropriate callback function based on the GATT event type, and pass the relevant event
     * parameters to the callback function */
    switch ( event )
    {
        case GATT_CONNECTION_STATUS_EVT:
            gatt_status = le_app_connect_handler( &p_event_data->connection_status );
            break;

        case GATT_ATTRIBUTE_REQUEST_EVT:
            gatt_status = le_app_server_handler( p_attr_req );
            break;

        case GATT_GET_RESPONSE_BUFFER_EVT: /* GATT buffer request, typically sized to max of bearer mtu - 1 */
            p_event_data->buffer_request.buffer.p_app_rsp_buffer =
            app_alloc_buffer(p_event_data->buffer_request.len_requested);
            p_event_data->buffer_request.buffer.p_app_ctxt = (void *)app_free_buffer;
            gatt_status = WICED_BT_GATT_SUCCESS;
            break;

        case GATT_APP_BUFFER_TRANSMITTED_EVT: /* GATT buffer transmitted event,  check \ref wiced_bt_gatt_buffer_transmitted_t*/
        {
            pfn_free_buffer_t pfn_free = (pfn_free_buffer_t)p_event_data->buffer_xmitted.p_app_ctxt;

            /* If the buffer is dynamic, the context will point to a function to free it. */
            if (pfn_free)
                pfn_free(p_event_data->buffer_xmitted.p_app_data);

            gatt_status = WICED_BT_GATT_SUCCESS;
        }
            break;

        default:
            gatt_status = WICED_BT_GATT_SUCCESS;
               break;
    }

    return gatt_status;
}

/**************************************************************************************************
* Function Name: le_app_server_handler
***************************************************************************************************
* Summary:
*   This function handles GATT server events from the BT stack.
*
* Parameters:
*  p_attr_req     Pointer to LE GATT connection status
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e in wiced_bt_gatt.h
*
**************************************************************************************************/
static wiced_bt_gatt_status_t le_app_server_handler (wiced_bt_gatt_attribute_request_t *p_attr_req)
{
    wiced_bt_gatt_status_t gatt_status = WICED_BT_GATT_ERROR;
    switch ( p_attr_req->opcode )
    {
        case GATT_REQ_READ:
        case GATT_REQ_READ_BLOB:
             /* Attribute read request */
            gatt_status = le_app_read_handler(p_attr_req->conn_id,p_attr_req->opcode,
                                           &p_attr_req->data.read_req,
                                           p_attr_req->len_requested);
             break;
        case GATT_REQ_WRITE:
        case GATT_CMD_WRITE:
             /* Attribute write request */
            gatt_status = le_app_write_handler(p_attr_req->conn_id, p_attr_req->opcode,
                                           &p_attr_req->data.write_req,
                                           p_attr_req->len_requested );

             break;
        case GATT_REQ_MTU:
            gatt_status = wiced_bt_gatt_server_send_mtu_rsp(p_attr_req->conn_id,
                                                       p_attr_req->data.remote_mtu,
                                                       CY_BT_MTU_SIZE);
             break;
        case GATT_HANDLE_VALUE_NOTIF:
        gatt_status = WICED_BT_GATT_SUCCESS;
            printf("Notification send complete\r\n");
             break;
        case GATT_REQ_READ_BY_TYPE:
            gatt_status = le_app_gatt_req_read_by_type_handler(p_attr_req->conn_id, p_attr_req->opcode,
                                                           &p_attr_req->data.read_by_type, p_attr_req->len_requested);
             break;

        default:
                printf("ERROR: Unhandled GATT Connection Request case: %d\r\n", p_attr_req->opcode);
                break;
    }

    return gatt_status;
}

/**************************************************************************************************
* Function Name: le_app_write_handler
***************************************************************************************************
* Summary:
*   This function handles Write Requests received from the client device
*
* Parameters:
*  @param conn_id       Connection ID
*  @param opcode        LE GATT request type opcode
*  @param p_write_req   Pointer to LE GATT write request
*  @param len_req       length of data requested
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e in wiced_bt_gatt.h
*
**************************************************************************************************/
static wiced_bt_gatt_status_t le_app_write_handler(uint16_t conn_id,
                                                    wiced_bt_gatt_opcode_t opcode,
                                                    wiced_bt_gatt_write_req_t *p_write_req,
                                                    uint16_t len_req)
{
    wiced_bt_gatt_status_t gatt_status = WICED_BT_GATT_INVALID_HANDLE;

    /* Attempt to perform the Write Request */
    gatt_status = le_app_set_value(p_write_req->handle,
                                    p_write_req->p_val,
                                    p_write_req->val_len);
    if( WICED_BT_GATT_SUCCESS != gatt_status )
    {
        printf("WARNING: GATT set attr status 0x%x\r\n", gatt_status);
    }
    else
    {
        if(GATT_REQ_WRITE == opcode)
        gatt_status = wiced_bt_gatt_server_send_write_rsp(conn_id, opcode, p_write_req->handle);
    }
    return (gatt_status);
}

/**************************************************************************************************
* Function Name: le_app_read_handler
***************************************************************************************************
* Summary:
*   This function handles Read Requests received from the client device
*
* Parameters:
* @param conn_id       Connection ID
* @param opcode        LE GATT request type opcode
* @param p_read_req    Pointer to read request containing the handle to read
* @param len_req       length of data requested
*
* Return:
*  wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e in wiced_bt_gatt.h
*
**************************************************************************************************/
static wiced_bt_gatt_status_t le_app_read_handler( uint16_t conn_id,
                                                    wiced_bt_gatt_opcode_t opcode,
                                                    wiced_bt_gatt_read_t *p_read_req,
                                                    uint16_t len_req)
{

    gatt_db_lookup_table_t  *puAttribute;
    int          attr_len_to_copy;
    uint8_t     *from;
    int          to_send;

    puAttribute = le_app_find_by_handle(p_read_req->handle);
    if ( NULL == puAttribute )
    {
        wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_req->handle,
                                            WICED_BT_GATT_INVALID_HANDLE);
        return WICED_BT_GATT_INVALID_HANDLE;
    }
    attr_len_to_copy = puAttribute->cur_len;
    if (p_read_req->offset >= puAttribute->cur_len)
    {
         wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_req->handle,
                                             WICED_BT_GATT_INVALID_OFFSET);
         return WICED_BT_GATT_INVALID_OFFSET;
     }
    to_send = MIN(len_req, attr_len_to_copy - p_read_req->offset);
    from = ((uint8_t *)puAttribute->p_data) + p_read_req->offset;
    return wiced_bt_gatt_server_send_read_handle_rsp(conn_id, opcode, to_send, from, NULL); /* No need for context, as buff not allocated */;
}

/**
 * Function Name:
 * le_app_gatt_req_read_by_type_handler
 *
 * Function Description:
 * @brief  Process read-by-type request from peer device
 *
 * @param conn_id       Connection ID
 * @param opcode        LE GATT request type opcode
 * @param p_read_req    Pointer to read request containing the handle to read
 * @param len_requested length of data requested
 *
 * @return wiced_bt_gatt_status_t  LE GATT status
 */
static wiced_bt_gatt_status_t le_app_gatt_req_read_by_type_handler(uint16_t conn_id,
                                                                   wiced_bt_gatt_opcode_t opcode,
                                                                   wiced_bt_gatt_read_by_type_t *p_read_req,
                                                                   uint16_t len_requested)
{
    gatt_db_lookup_table_t *puAttribute;
    uint16_t last_handle = 0;
    uint16_t attr_handle = p_read_req->s_handle;
    uint8_t *p_rsp = app_alloc_buffer(len_requested);
    uint8_t pair_len = 0;
    int used_len = 0;

    if (NULL == p_rsp)
    {
        printf("No memory, len_requested: %d!!\r\n",len_requested);
        wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, attr_handle, WICED_BT_GATT_INSUF_RESOURCE);
        return WICED_BT_GATT_INSUF_RESOURCE;
    }

    /* Read by type returns all attributes of the specified type, between the start and end handles */
    while (WICED_TRUE)
    {
        last_handle = attr_handle;
        attr_handle = wiced_bt_gatt_find_handle_by_type(attr_handle, p_read_req->e_handle,
                                                        &p_read_req->uuid);
        if (0 == attr_handle )
            break;

        if ( NULL == (puAttribute = le_app_find_by_handle(attr_handle)))
        {
            printf("found type but no attribute for %d \r\n",last_handle);
            wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_req->s_handle,
                                                WICED_BT_GATT_ERR_UNLIKELY);
            app_free_buffer(p_rsp);
            return WICED_BT_GATT_INVALID_HANDLE;
        }

        {
            int filled = wiced_bt_gatt_put_read_by_type_rsp_in_stream(p_rsp + used_len, len_requested - used_len, &pair_len,
                                                                attr_handle, puAttribute->cur_len, puAttribute->p_data);
            if (0 == filled)
            {
                break;
            }
            used_len += filled;
        }

        /* Increment starting handle for next search to one past current */
        attr_handle++;
    }

    if (0 == used_len)
    {
       printf("attr not found  start_handle: 0x%04x  end_handle: 0x%04x  Type: 0x%04x\r\n",
               p_read_req->s_handle, p_read_req->e_handle, p_read_req->uuid.uu.uuid16);
        wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_req->s_handle, WICED_BT_GATT_INVALID_HANDLE);
        app_free_buffer(p_rsp);
        return WICED_BT_GATT_INVALID_HANDLE;
    }

    /* Send the response */
    return wiced_bt_gatt_server_send_read_by_type_rsp(conn_id, opcode, pair_len, used_len, p_rsp, (void *)app_free_buffer);
}



/**************************************************************************************************
* Function Name: le_app_set_value
***************************************************************************************************
* Summary:
*   This function handles writing to the attribute handle in the GATT database using the
*   data passed from the BT stack. The value to write is stored in a buffer
*   whose starting address is passed as one of the function parameters
*
* Parameters:
* @param attr_handle  GATT attribute handle
* @param p_val        Pointer to LE GATT write request value
* @param len          length of GATT write request
*
*
* Return:
*   wiced_bt_gatt_status_t: See possible status codes in wiced_bt_gatt_status_e in wiced_bt_gatt.h
*
**************************************************************************************************/
static wiced_bt_gatt_status_t le_app_set_value(uint16_t attr_handle,
                                                uint8_t *p_val,
                                                uint16_t len)
{
    wiced_bool_t isHandleInTable = WICED_FALSE;
    wiced_bool_t validLen = WICED_FALSE;
    wiced_bt_gatt_status_t gatt_status = WICED_BT_GATT_INVALID_HANDLE;
    /* Check for a matching handle entry */
    for (int i = 0; i < app_gatt_db_ext_attr_tbl_size; i++)
    {
        if (app_gatt_db_ext_attr_tbl[i].handle == attr_handle)
        {
            /* Detected a matching handle in external lookup table */
            isHandleInTable = WICED_TRUE;

            /* Check if the buffer has space to store the data */
            validLen = (app_gatt_db_ext_attr_tbl[i].max_len >= len);

            if (validLen)
            {
                /* Value fits within the supplied buffer; copy over the value */
                app_gatt_db_ext_attr_tbl[i].cur_len = len;
                memcpy(app_gatt_db_ext_attr_tbl[i].p_data, p_val, len);
                gatt_status = WICED_BT_GATT_SUCCESS;

                /* Add code for any action required when this attribute is written.
                 * In this case, we update the IAS led based on the IAS alert
                 * level characteristic value */

                switch ( attr_handle )
                {
                    case HDLC_IAS_ALERT_LEVEL_VALUE:
                        printf("Alert Level = %d\r\n", app_ias_alert_level[0]);
#ifdef CYBSP_USER_LED1
                        ias_led_update();
#endif
                        break;

                    /* The application is not going to change its GATT DB,
                     * So this case is not handled */
                    case HDLD_GATT_SERVICE_CHANGED_CLIENT_CHAR_CONFIG:
                        break;
                }
            }
            else
            {
                /* Value to write does not meet size constraints */
                gatt_status = WICED_BT_GATT_INVALID_ATTR_LEN;
            }
            break;
        }
    }

    if (!isHandleInTable)
    {
        /* TODO: Add code to read value for handles not contained within generated lookup table.
         * This is a custom logic that depends on the application, and is not used in the
         * current application. If the value for the current handle is successfully written in the
         * below code snippet, then set the result using:
         * res = WICED_BT_GATT_SUCCESS; */
        switch ( attr_handle )
        {
            default:
                /* The write operation was not performed for the indicated handle */
                printf("Write Request to Invalid Handle: 0x%x\r\n", attr_handle);
                gatt_status = WICED_BT_GATT_WRITE_NOT_PERMIT;
                break;
        }
    }

    return gatt_status;
}


/*******************************************************************************
 * Function Name : le_app_find_by_handle
 * *****************************************************************************
 * Summary : @brief  Find attribute description by handle
 *
 * @param handle    handle to look up
 *
 * @return gatt_db_lookup_table_t   pointer containing handle data
 ******************************************************************************/
static gatt_db_lookup_table_t  *le_app_find_by_handle(uint16_t handle)
{
    int i;

    for (i = 0; i < app_gatt_db_ext_attr_tbl_size; i++)
    {
        if (handle == app_gatt_db_ext_attr_tbl[i].handle )
        {
            return (&app_gatt_db_ext_attr_tbl[i]);
        }
    }

    return NULL;
}
/* [] END OF FILE */
