/*******************************************************************************
* File Name: app_user_interface.h
*
* Description: Header file for application user interface (LEDs, Buttons) related
*              functionality
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

#ifndef LE_APP_USER_INTERFACE_H_
#define LE_APP_USER_INTERFACE_H_

/*******************************************************************************
*        Header Files
*******************************************************************************/
#include "cyhal.h"
#include "cycfg_pins.h"
#include "GeneratedSource/cycfg_gatt_db.h"
#include "cy_retarget_io.h"
/*******************************************************************************
*        Macro Definitions
*******************************************************************************/

/* PWM frequency of LED's in Hertz when blinking */
#define IAS_LED_PWM_FREQUENCY          (2)

/* IAS Alert Levels */
#define IAS_ALERT_LEVEL_LOW             (0u)
#define IAS_ALERT_LEVEL_MID             (1u)
#define IAS_ALERT_LEVEL_HIGH            (2u)

/*******************************************************************************
*        Variables
*******************************************************************************/
#ifdef CYBSP_USER_LED1
extern cyhal_pwm_t ias_led_pwm;
#endif

/*******************************************************************************
* Function Name: ias_led_update
********************************************************************************
*
* Summary:
*   This function updates the IAS alert level LED state based on LE
*   advertising/connection state
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void ias_led_update(void);

#endif /* LE_APP_USER_INTERFACE_H_ */

/* [] END OF FILE */
