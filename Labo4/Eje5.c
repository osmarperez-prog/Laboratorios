#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

uint32_t g_ui32SysClock;

volatile uint8_t estado = 0;
volatile uint32_t segundos = 1;

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if(estado == 0)
    {
        // 00
        GPIOPinWrite(
            GPIO_PORTN_BASE,
            0x03,
            0x00
        );
    }
    else if(estado == 1)
    {
        // 01
        GPIOPinWrite(
            GPIO_PORTN_BASE,
            0x03,
            0x01
        );
    }
    else if(estado == 2)
    {
        // 10
        GPIOPinWrite(
            GPIO_PORTN_BASE,
            0x03,
            0x02
        );
    }

    estado++;

    if(estado > 2)
    {
        estado = 0;
    }
}

void TimerSetSeconds(uint32_t tiempo)
{
    segundos = tiempo;

    TimerLoadSet(
        TIMER0_BASE,
        TIMER_A,
        (g_ui32SysClock * segundos) - 1
    );
}


int main(void)
{
    uint32_t ui32Period;

    g_ui32SysClock = SysCtlClockFreqSet(
        (SYSCTL_XTAL_25MHZ |
         SYSCTL_OSC_MAIN |
         SYSCTL_USE_PLL |
         SYSCTL_CFG_VCO_480),
        120000000
    );

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    GPIOPinTypeGPIOOutput(
        GPIO_PORTN_BASE,
        0x03
    );

    GPIOPinWrite(
        GPIO_PORTN_BASE,
        0x03,
        0x00
    );

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    TimerConfigure(
        TIMER0_BASE,
        TIMER_CFG_PERIODIC
    );


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


    TimerEnable(
        TIMER0_BASE,
        TIMER_A
    );

    while(1)
    {

    }
}