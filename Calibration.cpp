#include "OpenT12.h"

float    PTemp[FixNum] = { TempP1, TempP2, TempP3, TempP4 }; //温度拟合系数
uint32_t Calibration_Base[FixNum] = {100, 150, 200, 250, 300, 320 ,350, 400, 450 };
uint32_t Calibration_Input[FixNum] = { 0 };

//显示曲线系数
void ShowCurveCoefficient(void) {
    Clear();
    char buffer[50];
    for (long i = 0;i < 4;i++) {
        sprintf(buffer, "P[%d]=%.8f\n", i, PTemp[i]);
        Disp.setCursor(12, i * 12 + 8);
        Disp.print(buffer);
    }
    Display();
    while (!sys_KeyProcess()) {
        //ESP.wdtFeed();
    }
}

//校准界面
void CalibrationTemperature(void) {
    Clear();
    char buffer[50];
    uint8_t key;
    uint16_t SetADC = 0;
    int ADC,LastADC;
    sys_Counter_Set(0, 4095, 1, 0);
    for (uint8_t i = 0;i < FixNum;) {
        Clear();

        SetADC = sys_Counter_Get();

        ADC = GetADC0();
        //加热
        if (ADC!=-1) {
            LastADC = ADC;
            
            if (LastADC < SetADC) SetPOWER(255);
            else SetPOWER(0);
        }
        

        sprintf(buffer, "运行时间:%ld", millis());
        Disp.setCursor(0, 1);
        Disp.print(buffer);

        sprintf(buffer, "设定：%d 目标:%d", SetADC, Calibration_Base[i]);
        Disp.setCursor(0, 13);
        Disp.print(buffer);

        sprintf(buffer, "ADC:%d", LastADC);
        Disp.setCursor(0, 25);
        Disp.print(buffer);

        //Progress_Bar(i, 0, FixNum, 0, 64-8, 128, 8, 1);

        Display();


        if (sys_KeyProcess()) {
            delay(50);
            Calibration_Input[i] = SetADC;
            i++;
        }
    }
    //关闭功率管输出
    SetPOWER(0);
    //进行曲线拟合
    polyfit(FixNum, Calibration_Input, Calibration_Base, 3, PTemp);
    Pop_Windows("曲线拟合完成!");
    delay(800);
    ShowCurveCoefficient();
}

//********** 曲线拟合程序 **********
//曲线拟合算法来至https://blog.csdn.net/m0_37362454/article/details/82456616 by欧阳小俊

/*============================================================
  高斯消元法计算得到 n 次多项式的系数
  n: 系数的个数
  ata: 线性矩阵
  sumxy: 线性方程组的Y值
  p: 返回拟合的结果
  ============================================================*/
void gauss_solve(long n, float A[], float x[], float b[])
{
  long i, j, k, r;
  float max;
  for (k = 0; k < n - 1; k++)
  {
    max = fabs(A[k * n + k]);   // find maxmum
    r = k;
    for (i = k + 1; i < n - 1; i++)
    {
      if (max < fabs(A[i * n + i]))
      {
        max = fabs(A[i * n + i]);
        r = i;
      }
    }
    if (r != k)
    {
      for (i = 0; i < n; i++) //change array:A[k]&A[r]
      {
        max = A[k * n + i];
        A[k * n + i] = A[r * n + i];
        A[r * n + i] = max;
      }
      max = b[k];                  //change array:b[k]&b[r]
      b[k] = b[r];
      b[r] = max;
    }

    for (i = k + 1; i < n; i++)
    {
      for (j = k + 1; j < n; j++)
        A[i * n + j] -= A[i * n + k] * A[k * n + j] / A[k * n + k];
      b[i] -= A[i * n + k] * b[k] / A[k * n + k];
    }
  }

  for (i = n - 1; i >= 0; x[i] /= A[i * n + i], i--)
  {
    for (j = i + 1, x[i] = b[i]; j < n; j++)
      x[i] -= A[i * n + j] * x[j];
  }
}

/*==================polyfit(n,x,y,poly_n,a)===================*/
/*=======拟合y=a0+a1*x+a2*x^2+……+apoly_n*x^poly_n========*/
/*=====n是数据个数 xy是数据值 poly_n是多项式的项数======*/
/*===返回a0,a1,a2,……a[poly_n]，系数比项数多一（常数项）=====*/
void polyfit(long n, long x[], long y[], long poly_n, float p[])
{
  long i, j;
  float *tempx, *tempy, *sumxx, *sumxy, *ata;

  tempx = (float *)calloc(n , sizeof(float));
  sumxx = (float *)calloc((poly_n * 2 + 1) , sizeof(float));
  tempy = (float *)calloc(n , sizeof(float));
  sumxy = (float *)calloc((poly_n + 1) , sizeof(float));
  ata = (float *)calloc( (poly_n + 1) * (poly_n + 1) , sizeof(float) );
  for (i = 0; i < n; i++)
  {
    tempx[i] = 1;
    tempy[i] = y[i];
  }
  for (i = 0; i < 2 * poly_n + 1; i++)
  {
    for (sumxx[i] = 0, j = 0; j < n; j++)
    {
      sumxx[i] += tempx[j];
      tempx[j] *= x[j];
    }
  }
  for (i = 0; i < poly_n + 1; i++)
  {
    for (sumxy[i] = 0, j = 0; j < n; j++)
    {
      sumxy[i] += tempy[j];
      tempy[j] *= x[j];
    }
  }
  for (i = 0; i < poly_n + 1; i++)
  {
    for (j = 0; j < poly_n + 1; j++)
    {
      ata[i * (poly_n + 1) + j] = sumxx[i + j];
    }
  }
  gauss_solve(poly_n + 1, ata, p, sumxy);

}