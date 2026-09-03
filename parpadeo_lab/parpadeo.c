#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1)
    {
    }
}
#endif

int main(void)
{
    int counter = 0;
    uint32_t ledN;
    uint32_t ledF;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOPadConfigSet(GPIO_PORTJ_BASE,
                     GPIO_PIN_0 | GPIO_PIN_1,
                     GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);

    while(1)
    {
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0)
        {
            if(counter < 15)
            {
                counter++;
            }

            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0);
            SysCtlDelay(400000);
        }

        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0)
        {
            if(counter > 0)
            {
                counter--;
            }

            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0);
            SysCtlDelay(400000);
        }

        ledN = 0;
        ledF = 0;

        if(counter & 1)
            ledN |= GPIO_PIN_1;

        if(counter & 2)
            ledN |= GPIO_PIN_0;

        if(counter & 4)
            ledF |= GPIO_PIN_4;

        if(counter & 8)
            ledF |= GPIO_PIN_0;

        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, ledN);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, ledF);
    }
}
