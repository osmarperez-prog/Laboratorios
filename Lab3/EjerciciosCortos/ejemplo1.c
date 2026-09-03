#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void) {
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    
    // Hab puerto N
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03); 

    while(1) {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01); 
        SysCtlDelay(120000000 / 3);               
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03); 
        SysCtlDelay(120000000 / 3);                
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00); 
        SysCtlDelay(120000000 / 3);                
    }
}