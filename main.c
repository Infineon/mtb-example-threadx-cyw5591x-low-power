/*******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the low power Application.
*
* Related Document: See README.md
*
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
* Header Files
*******************************************************************************/
#include "WLANsrc/wifi_functions.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cyabs_rtos.h"
#include "wifi_functions.h"
#include <le_app_event_handler.h>
#include <le_app_utils.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "wiced_bt_stack.h"
#include "GeneratedSource/cycfg_bt_settings.h"
#include "cy_retarget_io.h"
#include "low_power_functions.h"
/*******************************************************************************
* Macros
*******************************************************************************/
#define UART_TASK_NAME            ("UART")
#define UART_TASK_STACK_SIZE      16*4096
#define UART_TASK_PRIORITY        (CY_RTOS_PRIORITY_NORMAL)
#define UART_INT_PRIORITY          (3)

/* Thread variables*/
cy_thread_t uart_task_handle;
cy_thread_t iperf_task_handle;
cy_thread_t network_idle_task_handle;

/* Queue for communication between UART ISR and UART Task*/
cy_queue_t xUARTQueue;

/* Queue size for UART task*/
#define QUEUE_SIZE                          (1)

/*******************************************************************************
* Global Variables
*******************************************************************************/

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
/*UART interrupt handler*/
void   uart_interrupt_handler      (void *handler_arg,
                                    cyhal_uart_event_t event);
void   uart_task                   (cy_thread_arg_t pvParameters);

/*App feature and utility functions*/
void   display_menu                (void);

/*******************************************************************************
* Function Definitions
*******************************************************************************/


/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
* This is the main function for CPU. It...
*    1. Initializes the BSP
*    2. Enables Global interrupt
*    3. Creates UART task for handling user commands
*    4. Resgisters SysPM callback for low power
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }


    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                 CY_RETARGET_IO_BAUDRATE);
    /* retarget-io init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

   /* Enable global interrupts */
    __enable_irq();

    display_menu();

    /*Register PM callback*/
    cyhal_syspm_register_callback(&pm_callback_data);

    /*Set up wake source*/
    setup_wake_source();


    /* Create a queue capable of storing unsigned integer values.
       this is used for communicating between UART ISR and the UART task */
    result = cy_rtos_queue_init(&xUARTQueue,QUEUE_SIZE,sizeof(uint8_t) );

    /*Check the status of Queue creation*/
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Failed to create Queue for communication between UART ISR and Task!!");
        CY_ASSERT(0);
    }

    /*Create uart task*/
    result = cy_rtos_thread_create(&uart_task_handle,
                                   &uart_task,
                                   UART_TASK_NAME,
                                   NULL,
                                   UART_TASK_STACK_SIZE,
                                   UART_TASK_PRIORITY,
                                   0);
    if (result != CY_RSLT_SUCCESS)
    {
    printf("UART thread creation failed \r\n");
        CY_ASSERT(0);
    }

    /* Register a callback function and set it to fire for any received UART characters */
    cyhal_uart_register_callback(&cy_retarget_io_uart_obj, uart_interrupt_handler,NULL);
    cyhal_uart_enable_event(&cy_retarget_io_uart_obj, CYHAL_UART_IRQ_RX_NOT_EMPTY, UART_INT_PRIORITY, TRUE);
}

/**
* Function Name:
* uart_interrupt_handler
*
* Function Description:
* @brief  This function handles the UART interrupts and pushes the input to the
*         UART queue for processing by uart_task
*
* @param  void *handler_arg:                 Not used
*         cyhal_uart_event_t event:          Not used
*
* @return  None
*
*/
void uart_interrupt_handler(void *handler_arg, cyhal_uart_event_t event)
{
    (void) handler_arg;
    uint8_t readbyte;

    /* Read one byte from the buffer with a 100ms timeout */
    cyhal_uart_getc(&cy_retarget_io_uart_obj , &readbyte, 100);

    /* Post the byte. */
    cy_rtos_queue_put(&xUARTQueue, &readbyte, 0);
}


/**
* Function Name:
* uart_task
*
* Function Description:
* @brief  This function runs the UART task which processes the received commands via
*         Terminal.
*
* @param  cy_thread_arg_t arg   : Not used
*
* @return None
*
*/

void uart_task(cy_thread_arg_t arg)
{
    uint8_t readbyte = 0;
    cy_rslt_t rslt = CY_RSLT_SUCCESS;
    wiced_result_t wiced_result;

    /*Sleep locked to allow user to manipulate system state*/
    if(true != is_sleep_lock_taken)
    {
        cyhal_syspm_lock_deepsleep();
        is_sleep_lock_taken=true;
    }

    for(;;)
    {
        rslt = cy_rtos_queue_get( &xUARTQueue, &(readbyte), CY_RTOS_NEVER_TIMEOUT);
        if (CY_RSLT_SUCCESS == rslt)
        {
            printf("Received character %c \r\n",readbyte);
            switch (readbyte) {
            case '1':
                if (WLAN_NOT_INITIALIZED == current_wlan_state)
                {
                    lp_wlan_init();
                }
                else
                {
                    printf("WLAN is already Initialized!! \r\n");
                }
                break;
            case '2':
                if(WLAN_NOT_INITIALIZED == current_wlan_state)
                {
                    lp_wlan_init();
                }

                if(WLAN_IDLE == current_wlan_state)
                {
                    /*connect to the AP*/
                    connect_to_wifi_ap();
                    /*start the lpa network task*/
                    rslt = cy_rtos_create_thread(&network_idle_task_handle,
                            network_idle_task,
                            "network_idle_task",
                            NULL,
                            NETWORK_IDLE_TASK_STACK_SIZE,
                            (cy_thread_priority_t)(CY_RTOS_PRIORITY_LOW),
                            0);

                    if (rslt != CY_RSLT_SUCCESS)
                    {
                        CY_ASSERT(0);
                    }
                }
                else
                {
                    printf("WLAN is already associated \r\n");
                }
                break;
            case '3':
                if(WLAN_NOT_INITIALIZED == current_wlan_state)
                {
                    lp_wlan_init();

                }

                if (WLAN_IDLE == current_wlan_state)
                {
                    /*connect to the AP*/
                    connect_to_wifi_ap();
                    /*start the lpa network task*/
                    rslt = cy_rtos_create_thread(&network_idle_task_handle,
                            network_idle_task,
                            "network_idle_task",
                            NULL,
                            NETWORK_IDLE_TASK_STACK_SIZE,
                            (cy_thread_priority_t)(CY_RTOS_PRIORITY_LOW),
                            0);

                    if (rslt != CY_RSLT_SUCCESS)
                    {
                        CY_ASSERT(0);
                    }
                }

                if (WLAN_ACTIVE != current_wlan_state)
                {
                    printf("Starting iperf session!! \r\n");
                    rslt = cy_rtos_create_thread(&iperf_task_handle,
                            iperf_util_thread_app,
                            "iperf_task",
                            NULL,
                            IPERF_TASK_STACK_SIZE,
                            (cy_thread_priority_t)(CY_RTOS_PRIORITY_HIGH),
                            0);
                    if (rslt != CY_RSLT_SUCCESS)
                    {
                        printf("failed to create iperf thread \r\n");
                    }
                }
                else{
                    printf("iperf session is already active \r\n");
                }
                break;
            case '4':
                if (current_le_state==LE_NOT_INITIALIZED)
                {
                    printf("Initializing Bluetooth!! \r\n");
                       /* Register call back and configuration with stack */
                       wiced_result = wiced_bt_stack_init(le_app_management_callback, &cy_bt_cfg_settings);
                       /* Check if stack initialization was successful */
                       if( WICED_BT_SUCCESS != wiced_result)
                       {
                           printf("Bluetooth Stack Initialization failed!! \r\n");
                       }
                }
                else{
                    printf("Bluetooth is already initialized \r\n");
                }
                break;
            case '5':
                if (current_le_state==LE_NOT_INITIALIZED)
                {
                    printf("Initializing Bluetooth!! \r\n");
                    /* Register call back and configuration with stack */
                    wiced_result = wiced_bt_stack_init(le_app_management_callback, &cy_bt_cfg_settings);
                    /* Check if stack initialization was successful */
                    if( WICED_BT_SUCCESS != wiced_result)
                    {
                        printf("Bluetooth Stack Initialization failed!! \r\n");
                    }

                    /*The delay is for allowing the Bluetooth stack enough time to initialize*/
                    cy_rtos_delay_milliseconds(1000);
                }

                if(current_le_state == LE_INITIALIZED_IDLE)
                {
                    printf("Starting advertisements!! \r\n");
                    le_adv_init();
                }
                break;
            case '6':
                if(false == is_sleep_lock_taken)
                {
                    cyhal_syspm_lock_deepsleep();
                    is_sleep_lock_taken=true;
                    printf("Sleep locked \r\n");
                }
                else{
                    printf("Sleep lock already taken \r\n");
                }
                break;
            case '7':
                if(true == is_sleep_lock_taken)
                {
                    cyhal_syspm_unlock_deepsleep();
                    is_sleep_lock_taken=false;
                    /*Enable wake event again*/
                    printf("Sleep unlocked \r\n");
                }
                else{
                    printf("Sleep is already allowed \r\n");
                }
                break;
            case '8':
                if(WLAN_CONNECTED == current_wlan_state || WLAN_ACTIVE ==current_wlan_state)
                {
                    rslt = cy_wcm_disconnect_ap();
                    if(CY_RSLT_SUCCESS == rslt)
                    {
                        printf("disconnected from the AP \r\n");
                        current_wlan_state=WLAN_IDLE;
                    }
                }
                else
                {
                    printf("Not associated to an AP \r\n");
                }

                break;
            case '9':
                if(WLAN_NOT_INITIALIZED == current_wlan_state)
                {
                    lp_wlan_init();

                }
                rslt = cy_wcm_start_scan(scan_callback, NULL, NULL);
                if(CY_RSLT_SUCCESS == rslt)
                {
                    printf("Scan started....\r\n");
                }
                break;
            case 'h':
                display_menu();
                break;
            default:
                printf("Invalid Input \r\n");
                break;
            }
        }
    }
}

/**
 * Function Name:
 * display_menu
 *
 * Function Description:
 * @brief  Function to print the Menu.
 *
 * @param  None
 *
 * @return None
 *
 */
void display_menu(void)
{
    printf("*****************************************************************************\r\n "
           "                  Low Power Application Start                                \r\n"
           "*****************************************************************************\r\n");
    printf("********************* Available Commands ************************************\r\n");
    printf("**1) Press '1' to initialize WLAN.  \r\n");
    printf("**2) Press '2' to connect to an AP, Enter AP SSID and password when prompted. \r\n");
    printf("**3) Press '3' to start iperf session. \r\n");
    printf("**4) Press '4' to initialize Bluetooth. \r\n");
    printf("**5) Press '5' to start Bluetooth LE advertisements. \r\n");
    printf("**6) Press '6' to lock sleep. \r\n");
    printf("**7) Press '7' to allow sleep. \r\n");
    printf("**8) Press '8' to disconnect from the AP. \r\n");
    printf("**9) Press '9' any time in application to start scan. \r\n");
    printf("*11) Press 'h' any time in application to print the menu. \r\n");
    printf("*****************************************************************************\r\n");
}

/* [] END OF FILE */
