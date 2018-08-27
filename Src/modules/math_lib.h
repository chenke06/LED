#ifndef __MATH_LIB_H
#define	__MATH_LIB_H


#include "stm32f10x.h"

#define BYTE0(dwTemp)  (*(char *)(&dwTemp))
#define BYTE1(dwTemp)  (*(char *)(&dwTemp + 1))
#define BYTE2(dwTemp)  (*(char *)(&dwTemp + 2))
#define BYTE3(dwTemp)  (*(char *)(&dwTemp + 3))


#endif

