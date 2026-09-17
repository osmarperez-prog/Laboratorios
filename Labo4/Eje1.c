
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


// Global variables

uint32_t g_ui32SysClock;

//*****************************************************************************
// Timer 0A interrupt handler.

void
Timer0IntHandler(void)
{
    //
    // Clear the timer interrupt.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Toggle LED PN1.
    // PN1 = 0x02
    //
    GPIOPinWrite(GPIO_PORTN_BASE,
                 0x02,
                 GPIOPinRead(GPIO_PORTN_BASE, 0x02) ^ 0x02);
}

//*****************************************************************************
// Main


int
main(void)
{
    uint32_t ui32Period;

    g_ui32SysClock = SysCtlClockFreqSet(
        (SYSCTL_XTAL_25MHZ |
         SYSCTL_OSC_MAIN |
         SYSCTL_USE_PLL |
         SYSCTL_CFG_VCO_480),
        120000000);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);


    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }


    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x02);

    GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x00);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);


    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

  
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    //
    // Calculate the period.
  
    ui32Period = g_ui32SysClock;


    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

    IntEnable(INT_TIMER0A);

 
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    IntMasterEnable();


    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
    }
}