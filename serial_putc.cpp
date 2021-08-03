#include <Arduino.h>
#include "serial_putc.h"
int fputc(int c, FILE* f)
{
    Serial.write(c);
    return c;
}