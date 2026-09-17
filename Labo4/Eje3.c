#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

uint32_t g_ui32SysClock;

volatile uint8_t numero = 0;

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(
        GPIO_PORTN_BASE,
        0x0F,
        numero
    );

    numero++;

    if(numero > 9)
    {
        numero = 0;
    }
}

int main(void)
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

    GPIOPinTypeGPIOOutput(
        GPIO_PORTN_BASE,
        0x0F
    );

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }

    GPIOPinTypeGPIOInput(
        GPIO_PORTJ_BASE,
        0x01
    );

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = g_ui32SysClock;

    TimerLoadSet(
        TIMER0_BASE,
        TIMER_A,
        ui32Period - 1
    );

    IntEnable(INT_TIMER0A);

    TimerIntEnable(
        TIMER0_BASE,
        TIMER_TIMA_TIMEOUT
    );

    IntMasterEnable();

    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
        if(GPIOPinRead(GPIO_PORTJ_BASE, 0x01) == 0)
        {
            // 0,5 segundos
            TimerLoadSet(
                TIMER0_BASE,
                TIMER_A,
                (g_ui32SysClock / 2) - 1
            );
        }
        else
        {
            // 1 segundo
            TimerLoadSet(
                TIMER0_BASE,
                TIMER_A,
                g_ui32SysClock - 1
            );
        }
    }
}