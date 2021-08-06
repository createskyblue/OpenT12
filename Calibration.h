#ifndef Calibration_H
#define Calibration_H
#include "Type.h"
//温度拟合
/*
 50-50
100-70
150-134
200-221
250-300
300-379
350-460
400-520
450-579
*/
#define TempP1 112.2143936157
#define TempP2 0.2472778559
#define TempP3 -0.0000419526
#define TempP4 0.0000000133

#define FixNum 9
extern float    PTemp[FixNum]; //温度拟合系数
extern uint32_t Calibration_Base[FixNum];
extern uint32_t Calibration_Input[FixNum];

void ShowCurveCoefficient(void);
void CalibrationTemperature(void);
void gauss_solve(long n, float A[], float x[], float b[]);
void polyfit(long n, long x[], long y[], long poly_n, float p[]);
#endif