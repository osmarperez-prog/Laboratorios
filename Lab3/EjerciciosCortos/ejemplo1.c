#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void) {
    // Configuración de reloj del sistema
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    
    // Habilitar puerto N
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {
    }

    // Configurar PN0 y PN1 como salidas
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03); 

    // Bucle infinito de ejecucion
    while(1) {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01); // Encender PN0
        SysCtlDelay(120000000 / 3);                // Retardo 1 seg
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03); // Encender PN1 (mantiene PN0)
        SysCtlDelay(120000000 / 3);                // Retardo 1 seg
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00); // Apagar ambos
        SysCtlDelay(120000000 / 3);                // Retardo 1 seg
    }
}
