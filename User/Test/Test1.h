#ifndef __TEST1_H
#define __TEST1_H


#define app_log(...)          rtt_print(__FILE__, __FUNCTION__, __VA_ARGS__)


void FatfsTest(void);

#endif
