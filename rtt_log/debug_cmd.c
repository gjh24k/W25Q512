/**
  ******************************************************************************
  * @file    debug_cmd.c
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


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "rtt_log.h"
#include "debug_cmd.h"

#if RTT_DEBUG && RTT_DEBUG_CMD

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  char* cmd;
  cmd_cb func;
}debug_cmd_t;

/* Private define ------------------------------------------------------------*/
#define MAX_DEBUG_CMD_ENTRY       10

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static debug_cmd_t debug_cmd_array[MAX_DEBUG_CMD_ENTRY];
static unsigned debug_cmd_entries = 0;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Registers a debug command with its corresponding callback function.
 *
 * This function registers a debug command along with its corresponding callback
 * function. The debug command is provided as a null-terminated string in the
 * 'cmd' parameter. The callback function is provided in the 'func' parameter.
 *
 * @param cmd A null-terminated string representing the debug command.
 * @param func The callback function to be executed when the debug command is invoked.
 *
 * @return 0 if the debug command was successfully registered, or -1 if the maximum
 *         number of debug command entries (MAX_DEBUG_CMD_ENTRY) has been reached.
 *
 * @note The maximum number of debug command entries is defined by MAX_DEBUG_CMD_ENTRY.
 *       If the maximum is reached, an error message will be logged and -1 will be returned.
 */
int debug_cmd_register(char* cmd, cmd_cb func)
{
  if(debug_cmd_entries >= MAX_DEBUG_CMD_ENTRY) {
    app_log("ERROR: MAX_DEBUG_CMD_ENTRY");
    return -1;
  }
  debug_cmd_array[debug_cmd_entries].cmd = cmd;
  debug_cmd_array[debug_cmd_entries].func = func;
  debug_cmd_entries++;
  return 0;
}

/**
 * @brief Decodes a debug command from the given buffer and executes the corresponding function.
 *
 * This function iterates through the debug command array to find a match with the provided command
 * and then calls the associated function.
 *
 * @param cmd The debug command to be decoded and executed.
 */
void debug_cmd_decode_from_buffer(char* cmd)
{
  /**
   * Checks if the provided command is NULL and returns if true.
   */
  if(NULL == cmd) return;

  /**
   * Iterates through the debug command array to find a matching command and executes the associated function.
   */
  for(int i = 0; i < debug_cmd_entries; i++) {
    if(strstr(cmd, debug_cmd_array[i].cmd)) {
      debug_cmd_array[i].func(cmd);
      break;
    }
  }
}

/**
 * @brief Decodes debug commands received through RTT (Real-Time Transfer) and executes corresponding functions.
 *
 * This function reads data from the RTT log buffer, checks for incoming debug commands,
 * and calls the debug_cmd_decode_from_buffer function to execute the associated command.
 */
void debug_cmd_decode()
{
  /* Check RTT debug */
  char brdata[128];

  unsigned int br = SEGGER_RTT_Read(RTT_LOG_BUFFER_INDEX, brdata, sizeof(brdata));

  /**
   * Checks if data was read and the size is less than 128.
   */
  if(br && br < 128) {
    brdata[br] = 0;

    app_log("Received cmd: %s", brdata);
    debug_cmd_decode_from_buffer(brdata);
  }
}

#endif
   
#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT Minjie Electric *****END OF FILE****/
