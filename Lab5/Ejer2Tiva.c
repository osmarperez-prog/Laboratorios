#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"     
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

volatile uint32_t FS = 120000000; 
volatile uint8_t estado = 0;    

void timer0A_handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_A);

    switch(estado)
    {
        case 0:
            GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02);
            GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x00);
            estado = 1;
            break;

        case 1:
            GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01);
            GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x00);
            estado = 2;
            break;

        case 2:
            GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
            estado = 0;
            break;
    }
}

int main(void)
{
    uint32_t systemClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                               SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03); 
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x10); 

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, systemClock, 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, FS); 

    IntMasterEnable();
    IntEnable(INT_TIMER0A);  
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
        if(UARTCharsAvail(UART0_BASE))
        {
            char dato = UARTCharGet(UART0_BASE);

            if(dato >= '1' && dato <= '9')
            {
                uint32_t segundos = dato - '0';
                FS = systemClock * segundos;
                
                TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
            }
        }
    }
}