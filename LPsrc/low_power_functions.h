/******************************************************************************
 * File Name:   low_power_functions.h
 *
 * Description: This file contains declaration of task and functions related to
 * low power configuration.
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

#ifndef LOW_POWER_FUNCTIONS_H_
#define LOW_POWER_FUNCTIONS_H_

#include "cyhal_gpio.h"
#include "cyhal_syspm.h"
#include "cycfg_pins.h"
#include "cybsp_types.h"
/*******************************************************************************
* Macros
*******************************************************************************/
#define GPIO_INTERRUPT_PRIORITY             (7)
#define GPIO_WAKE_SOURCE                     (CYBSP_USER_BTN)

/*******************************************************************************
* Global Variables
*******************************************************************************/
extern bool gpio_interrupt_wake;
extern volatile bool is_sleep_lock_taken;
extern volatile int irq_level_flag;
extern cyhal_syspm_callback_data_t pm_callback_data;
/*******************************************************************************
* Function Prototypes
********************************************************************************/
void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);
bool pm_callback(cyhal_syspm_callback_state_t state, cyhal_syspm_callback_mode_t mode, void *callback_arg);
void setup_wake_source(void);

#endif /* LOW_POWER_FUNCTIONS_H_ */
