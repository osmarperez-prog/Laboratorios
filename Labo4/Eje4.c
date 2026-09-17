#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

uint32_t g_ui32SysClock;

volatile uint8_t counter = 0;

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(
        GPIO_PORTN_BASE,
        0x03,
        ((counter & 0x01) ? 0x02 : 0x00) |
        ((counter & 0x02) ? 0x01 : 0x00)
    );

    GPIOPinWrite(
        GPIO_PORTF_BASE,
        0x11,
        ((counter & 0x04) ? 0x10 : 0x00) |
        ((counter & 0x08) ? 0x01 : 0x00)
    );

    counter++;

    if(counter > 15)
    {
        counter = 0;
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


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    // PF0 y PF4 como salidas
    GPIOPinTypeGPIOOutput(
        GPIO_PORTF_BASE,
        0x11
    );

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }

    GPIOPinTypeGPIOInput(
        GPIO_PORTJ_BASE,
        0x01
    );

    // Pull-up interno
    GPIOPadConfigSet(
        GPIO_PORTJ_BASE,
        0x01,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );


    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    TimerConfigure(
        TIMER0_BASE,
        TIMER_CFG_PERIODIC
    );

    ui32Period = (g_ui32SysClock * 3) / 2;

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

    // Iniciar Timer
    TimerEnable(
        TIMER0_BASE,
        TIMER_A
    );

    while(1)
    {
        // Si se presiona PJ0
        if(GPIOPinRead(GPIO_PORTJ_BASE, 0x01) == 0)
        {
            // Cambiar a 3 segundos
            TimerLoadSet(
                TIMER0_BASE,
                TIMER_A,
                (g_ui32SysClock * 3) - 1
            );
        }
        else
        {
            // Regresar a 1.5 segundos
            TimerLoadSet(
                TIMER0_BASE,
                TIMER_A,
                ((g_ui32SysClock * 3) / 2) - 1
            );
        }
    }
}