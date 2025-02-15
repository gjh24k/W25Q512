/**
  ******************************************************************************
  * @file    rtt_log.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RTT_LOG_H_
#define RTT_LOG_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "segger_rtt/segger_rtt.h"
#include <stdarg.h>

/* Exported typedef ----------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/
#ifndef NULL
#define NULL 0
#endif

#ifdef DEBUG
#define RTT_DEBUG                1
#else
#define RTT_DEBUG                0
#endif

#define RTT_LOG_USE_RTC          0
#define RTT_LOG_BUFFER_INDEX     0


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void rtt_log_init(int (*get_time)(void*));
void rtt_print(const char* file, const char* function, const char * sFormat, ...);
void rtt_log(const char * sFormat, ...);
//void rtt_printhex(unsigned char* buffer, unsigned short len);
void rtt_printhex(unsigned char* buffer, unsigned short len, unsigned char asc);


#ifdef __cplusplus
}
#endif

#endif /* RTT_LOG_H_ */

/************************ (C) COPYRIGHT Minjie Electric *****END OF FILE****/
