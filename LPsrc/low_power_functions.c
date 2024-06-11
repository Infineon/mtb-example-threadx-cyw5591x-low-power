/*******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the Empty Application Example
*              to be used as starting template.
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
#include "low_power_functions.h"

/*******************************************************************************
* Global Variables
*******************************************************************************/

cyhal_gpio_callback_data_t cb_data =
{
.callback = gpio_interrupt_handler,
.callback_arg = NULL
};

cyhal_syspm_callback_data_t pm_callback_data =
{
    .callback = &pm_callback,
    .states = CYHAL_SYSPM_CB_CPU_DEEPSLEEP,
    .ignore_modes = CYHAL_SYSPM_CHECK_FAIL,
    .args = NULL,
    .next = NULL
};

bool gpio_interrupt_wake=false;
volatile bool is_sleep_lock_taken=false;
volatile int irq_level_flag=0;
/******************************************************************************
 * Function Definitions
 ******************************************************************************/
void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    if (irq_level_flag)
    {
        irq_level_flag = 0;
        cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, GPIO_INTERRUPT_PRIORITY, true);
    }
    else
    {
        irq_level_flag = 1;
        cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_RISE, GPIO_INTERRUPT_PRIORITY, true);
        gpio_interrupt_wake=true;
    }

}
/*******************************************************************************
 * Function Name: pm_callback()
 *******************************************************************************
 * Summary:
 *  This function receives the low power transition callbacks from SysPM Module
 * and allows user to take necessary action if needed.
 *
 *******************************************************************************/
bool pm_callback(cyhal_syspm_callback_state_t state, cyhal_syspm_callback_mode_t mode, void *callback_arg)
{
    (void)callback_arg;
    bool allow_transition = true;

    switch (mode)
    {
        case CYHAL_SYSPM_CHECK_READY:
            // Check to see if it is OK to change power modes.
            // If it is OK the change, make sure nothing will invalidate this and return true.
            // If it is not OK the change, return false.
            break;

        case CYHAL_SYSPM_CHECK_FAIL:
            // Undo anything done as part of CHECK_READY since it is not actually changing.
        break;

        case CYHAL_SYSPM_BEFORE_TRANSITION:
            // Do anything necessary to shut things down.
            break;

        case CYHAL_SYSPM_AFTER_TRANSITION:
            // Undo anything done as part of CHECK_READY or BEFORE_TRANSITION since it is coming
            // back up.
        if(true == gpio_interrupt_wake && irq_level_flag)
        {
            gpio_interrupt_wake=false;
            cyhal_syspm_lock_deepsleep();
            is_sleep_lock_taken=true;
        }
            break;

        case CYHAL_SYSPM_AFTER_DS_WFI_TRANSITION:
            // Do anything necessary after deepsleep wakeup and before interrupts getting enabled.
            break;
    }

    return allow_transition;
}

void setup_wake_source()
{
    cy_rslt_t result;
    printf("Setting up wake source \r\n");
    // Setup Wake-up source
    result = cyhal_gpio_init(GPIO_WAKE_SOURCE, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

    if (result != CY_RSLT_SUCCESS)
    {
        printf("GPIO Init failed \r\n");
    }

    /* Configure GPIO interrupt. */
    cyhal_gpio_register_callback(GPIO_WAKE_SOURCE, &cb_data);
    cyhal_gpio_enable_event(GPIO_WAKE_SOURCE, CYHAL_GPIO_IRQ_FALL, GPIO_INTERRUPT_PRIORITY, true);
    irq_level_flag = 0;

}
