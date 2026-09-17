#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"        
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"       
#include "driverlib/interrupt.h"   

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif
uint32_t FS =120000000*2;
uint8_t led_state = 0;

void timer0A_handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if(led_state == 0)
    {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1); 
        led_state = 1;
    }
    else
    {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x00);  
        led_state = 0;
    }
}
int main(void)
{
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // energía de los puertosx
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);


    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, 
                     GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE,TIMER_A, FS);
    IntMasterEnable();
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);


    while(1)
    {
    }
}
