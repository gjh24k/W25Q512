/**
  ******************************************************************************
  * @file    debug_cmd.h
  * @author  ray
  * @brief            
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 Minjie Electric.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DEBUG_CMD_H_
#define DEBUG_CMD_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* How to use this?
 * for example, we need to restart MCU from RTT debug
1. Implement the execution of specific commands.
void debug_cmd_restart(char* data)
{
  HAL_NVIC_SystemReset(); //stm32 HAL
}

2. register command in initial function
#if RTT_DEBUG && RTT_DEBUG_CMD
	debug_cmd_register("RESTART", debug_cmd_restart);
#endif

3. execute this in a loop
#if RTT_DEBUG && RTT_DEBUG_CMD
    debug_cmd_decode();
#endif

 */

/* Includes ------------------------------------------------------------------*/
#include <debug_cmd_if.h>

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef void (*cmd_cb)(char*);

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
int debug_cmd_register(char* cmd, cmd_cb func);
void debug_cmd_decode_from_buffer(char* cmd);
void debug_cmd_decode();

/* Private defines -----------------------------------------------------------*/
 
   
#ifdef __cplusplus
}
#endif

#endif /* DEBUG_CMD_H_ */

/************************ (C) COPYRIGHT Minjie Electric *****END OF FILE****/
