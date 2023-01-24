#ifndef _KERNEL_H
#define _KERNEL_H 1

#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

void enable_paging();
void init_pics(int pic1, int pic2);

#ifdef __cplusplus
}
#endif

#endif