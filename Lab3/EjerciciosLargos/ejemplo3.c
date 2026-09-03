#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void) {
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF) || 
          !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));

    //Desbloqueo de PJ0
    HWREG(GPIO_PORTJ_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTJ_BASE + GPIO_O_CR) |= GPIO_PIN_0;
    HWREG(GPIO_PORTJ_BASE + GPIO_O_LOCK) = 0;

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    int contador = 0;

    while(1) {
        if (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0) {
            contador++;
            if (contador > 15) contador = 15;
            SysCtlDelay(10000000); 
        }

        if (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0) {
            if (contador == 0) contador = 0;
            else contador--;
            SysCtlDelay(10000000); 
        }

        // Mapear los bits 
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, (contador & 0x01) ? GPIO_PIN_1 : 0); 
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, (contador & 0x02) ? GPIO_PIN_0 : 0); 
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, (contador & 0x04) ? GPIO_PIN_4 : 0); 
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, (contador & 0x08) ? GPIO_PIN_0 : 0); 

        SysCtlDelay(2000000); 
    }
}