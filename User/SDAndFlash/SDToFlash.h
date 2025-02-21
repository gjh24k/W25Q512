#ifndef __SDTOFLASH_H
#define __SDTOFLASH_H


#define app_log(...)          rtt_print(__FILE__, __FUNCTION__, __VA_ARGS__)

void SDToFlashTest(void);
void TraversalDirectory(const char * path);
#endif
