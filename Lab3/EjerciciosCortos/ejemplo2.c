#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void) {
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03); 
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11); 

    while(1) {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x01);
        SysCtlDelay(120000000 / 3);

        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
        SysCtlDelay(120000000 / 3);

        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x00);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x00);
        SysCtlDelay(120000000 / 3);

        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x00);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x00);
        SysCtlDelay(120000000 / 3);
    }
}