/*******************************************************************************
* File Name: app_user_interface.c
*
* Description: Source file for application user interface (LEDs, Buttons) related
*              functionality
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
#include "le_app_user_interface.h"
#include "le_app_event_handler.h"
/*******************************************************************************
*        Variable Definitions
*******************************************************************************/
#ifdef CYBSP_USER_LED1
/* create PWM object for Alert LED */
cyhal_pwm_t ias_led_pwm;
#endif

/* PWM Duty Cycle of LED's for different states */
enum
{
    LED_ON_DUTY_CYCLE= 0,
    LED_BLINKING_DUTY_CYCLE= 50,
    LED_OFF_DUTY_CYCLE= 100
} led_duty_cycles;

/*******************************************************************************
*        Function Prototypes
*******************************************************************************/

/*******************************************************************************
*        Function Definitions
*******************************************************************************/

#ifdef CYBSP_USER_LED1
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
void ias_led_update(void)
{
    cy_rslt_t cy_result = CY_RSLT_SUCCESS;
    /* Stop the IAS led pwm, this is to ensure smooth transition */
    cyhal_pwm_stop(&ias_led_pwm);

    /* Update LED based on IAS alert level only when the device is connected */
    if(current_le_state==LE_CONNECTED_IDLE)
    {
        /* Update LED state based on IAS alert level. LED OFF for low level,
         * LED blinking for mid level, and LED ON for high level  */
        switch(app_ias_alert_level[0])
        {
            case IAS_ALERT_LEVEL_LOW:
                /*Nothing to do as PWM is already stopped*/
                break;

            case IAS_ALERT_LEVEL_MID:
                cy_result = cyhal_pwm_set_duty_cycle(&ias_led_pwm, LED_BLINKING_DUTY_CYCLE, IAS_LED_PWM_FREQUENCY);
                break;

            case IAS_ALERT_LEVEL_HIGH:
                cy_result = cyhal_pwm_set_duty_cycle(&ias_led_pwm, LED_ON_DUTY_CYCLE, IAS_LED_PWM_FREQUENCY);
                break;

            default:
                /* Consider any other level as High alert level */
                cy_result = cyhal_pwm_set_duty_cycle(&ias_led_pwm, LED_ON_DUTY_CYCLE, IAS_LED_PWM_FREQUENCY);
                break;
        }

        /* Check if update to PWM parameters is successful*/
        if (CY_RSLT_SUCCESS != cy_result)
        {
            printf("Failed to set duty cycle parameters!!");
        }

        cy_result = cyhal_pwm_start(&ias_led_pwm);
        /* Check if PWM started successfully */
        if (CY_RSLT_SUCCESS != cy_result)
        {
            printf("Failed to start PWM !!");
        }
    }
    
    /* In all other cases let the IAS LED be off */

}
#endif

/* [] END OF FILE */
