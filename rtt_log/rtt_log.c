/**
  ******************************************************************************
  * @file    rtt_log.c
  * @author  Ray
  * @brief
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 Minjie Electric.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtt_log.h"
#include "string.h"
#include "stdio.h"

#if RTT_LOG_USE_RTC
#include "time.h"
int (*rtt_log_get_time)(void*);
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
int SEGGER_RTT_vprintf(unsigned BufferIndex, const char * sFormat, va_list * pParamList);

/* Private functions ---------------------------------------------------------*/
/**
 * @brief Initializes the RTT (Real-Time Transfer) logging functionality.
 *
 * This function initializes the RTT logging functionality for debugging purposes.
 * If the RTT_DEBUG flag is enabled, the function performs the initialization;
 * otherwise, it does nothing.
 *
 * The function configures the RTT buffer using the SEGGER_RTT_ConfigUpBuffer() function,
 * specifying the buffer index, NULL for read and write callbacks, 0 for buffer size,
 * and SEGGER_RTT_MODE_NO_BLOCK_SKIP mode.
 *
 * If the RTT_LOG_USE_RTC flag is enabled, the function sets the rtt_log_get_time function
 * pointer to the provided get_time function, which is responsible for retrieving the current time.
 *
 * @param get_time Function pointer to retrieve the current time (optional).
 */
void rtt_log_init(int (*get_time)(void*))
{
#if RTT_DEBUG
  SEGGER_RTT_ConfigUpBuffer(RTT_LOG_BUFFER_INDEX, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#if RTT_LOG_USE_RTC
  rtt_log_get_time = get_time;
#endif
#endif
}

/**
 * @brief Logs a formatted message to the RTT (Real-Time Transfer) buffer with file and function information.
 *
 * This function logs a formatted message to the RTT buffer for debugging purposes,
 * including the file name and function name where the log message is called.
 * If the RTT_DEBUG flag is enabled, the function performs the logging operation;
 * otherwise, it does nothing.
 *
 * If the RTT_LOG_USE_RTC flag is enabled, the function retrieves the current time
 * using the rtt_log_get_time() function and includes it in the log message.
 *
 * The function uses the SEGGER_RTT_printf() function to print the log message to
 * the RTT buffer. It supports variable arguments using the va_list mechanism.
 *
 * @param file The name of the source file where the log message is called.
 * @param function The name of the function where the log message is called.
 * @param sFormat The format string for the log message.
 * @param ... Additional arguments to be formatted according to the format string.
 */
void rtt_print(const char* file, const char* function, const char * sFormat, ...)
{
#if RTT_DEBUG
#if RTT_LOG_USE_RTC
  struct tm time;
  int res = rtt_log_get_time(&time);
  if(0 == res)
  {
    SEGGER_RTT_printf(RTT_LOG_BUFFER_INDEX, "\n[%04u/%02u/%02u %02u:%02u:%02u] ",
        time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
  }
#else
  SEGGER_RTT_printf(RTT_LOG_BUFFER_INDEX, "\n");
#endif

  va_list ParamList;
  va_start(ParamList, sFormat);
  SEGGER_RTT_printf(RTT_LOG_BUFFER_INDEX, "[%s %s] ", file, function);
  SEGGER_RTT_vprintf(RTT_LOG_BUFFER_INDEX, sFormat, &ParamList);
  va_end(ParamList);
#endif
}

/**
 * @brief Logs a formatted message to the RTT (Real-Time Transfer) buffer.
 *
 * This function logs a formatted message to the RTT buffer for debugging purposes.
 * If the RTT_DEBUG flag is enabled, the function performs the logging operation;
 * otherwise, it does nothing.
 *
 * If the RTT_LOG_USE_RTC flag is enabled, the function retrieves the current time
 * using the rtt_log_get_time() function and includes it in the log message.
 *
 * The function uses the SEGGER_RTT_printf() function to print the log message to
 * the RTT buffer. It supports variable arguments using the va_list mechanism.
 *
 * @param sFormat The format string for the log message.
 * @param ... Additional arguments to be formatted according to the format string.
 */
void rtt_log(const char * sFormat, ...)
{
#if RTT_DEBUG
#if RTT_LOG_USE_RTC
  struct tm time;
  int res = rtt_log_get_time(&time);
  if(0 == res)
  {
    SEGGER_RTT_printf(RTT_LOG_BUFFER_INDEX, "\n[%04u/%02u/%02u %02u:%02u:%02u]: ",
        time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
  }
#else
  SEGGER_RTT_printf(RTT_LOG_BUFFER_INDEX, "\n");
#endif

  va_list ParamList;
  va_start(ParamList, sFormat);
  SEGGER_RTT_vprintf(RTT_LOG_BUFFER_INDEX, sFormat, &ParamList);
  va_end(ParamList);
#endif
}

/**
 * @brief Prints data in hexadecimal format.
 *
 * This function prints the data in the specified buffer in hexadecimal format.
 * If the RTT_DEBUG flag is enabled, the function performs the printing operation;
 * otherwise, it does nothing.
 *
 * The function first checks if the buffer pointer is valid and if the length of
 * the data is non-zero. If either of these conditions is not met, an error message
 * is logged, and the function returns without performing any further operations.
 *
 * The function then proceeds to print the data in the buffer in hexadecimal format.
 * It uses a loop to iterate through the data, printing 16 bytes at a time. For each
 * iteration, it constructs a string containing the offset address in hexadecimal,
 * followed by the hexadecimal representation of the bytes. If the 'asc' parameter is
 * non-zero, it also prints the ASCII representation of the bytes.
 *
 * The printed output is logged using the 'rtt_log' function.
 *
 * @param buffer Pointer to the data buffer.
 * @param len Length of the data in the buffer.
 * @param asc Flag indicating whether to print the ASCII representation of the bytes.
 */
void rtt_printhex(unsigned char* buffer, unsigned short len, unsigned char asc)
{
#if RTT_DEBUG
  if(!buffer || !len)
  {
    rtt_log("[%s][%s][INVALID PARAMETER]", __FILE__, __LINE__);
    return;
  }

  rtt_log("Print %d data in HEX:");
  char cache[100];
  char tmp[50];
  char line_asc[16];
  for(int i = 0; i < len; i += 16)
  {
    memset(cache, 0, sizeof(cache));
    memset(tmp, 0, sizeof(tmp));
    snprintf(cache, sizeof(cache), "0x%04X ", i);
    for(int j = 0; j < ((len - i > 16) ? 16 : (len - i)); j++)
    {
      snprintf(tmp, sizeof(tmp), "%02X ", buffer[i + j]);
      strncat(cache, tmp, strlen(tmp));
    }
    if(asc)
    {
      if(len - i < 16)
      {
        for(int m = 0; m < 16 + i - len; m++)
        {
          strncat(cache, "   ", 4);
        }
      }
      strncat(cache, "| ", 3);
      char asc_len = (len - i > 16) ? 16 : (len - i);
      memcpy(line_asc, buffer + i, asc_len);
      for(int j = 0; j < asc_len; j++)
      {
        if(line_asc[j] < 32) {
          //control field, set to '.'
          line_asc[j] = '.';
        }
        strncat(cache, line_asc + j, 1);
      }
    }
    rtt_log(cache);
  }
#endif
}


#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT Minjie Electric *****END OF FILE****/
