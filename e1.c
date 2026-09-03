#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// Función para controlar los 4 LEDs integrados según el estado (0 a 3)
void setLEDs(int state) {
    // Apagar todos los LEDs de los puertos N y F primero
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0x00);

    // Encender el LED correspondiente al estado actual
    switch(state) {
        case 0:
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1); // LED 1 (PN1)
            break;
        case 1:
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0); // LED 2 (PN0)
            break;
        case 2:
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4); // LED 3 (PF4)
            break;
        case 3:
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0); // LED 4 (PF0)
            break;
    }
}

int main(void) {
    // Configuración del reloj del sistema a 120 MHz
    uint32_t g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                                 SYSCTL_OSC_MAIN |
                                                 SYSCTL_USE_PLL |
                                                 SYSCTL_CFG_VCO_480), 120000000);

    // Habilitar periféricos de LEDs (N, F) y Switches (J)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));

    // Configurar pines de LEDs como salida
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    // Configurar pines de Switches como entrada con resistencia Pull-Up interna
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    int state = 0;
    setLEDs(state);

    while(1) {
        // Switch 1 (PJ0): Secuencia Ascendente
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0) {
            SysCtlDelay(g_ui32SysClock / 30); // Antirrebote (~100ms)
            if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0) {
                state = (state + 1) % 4; // Avanzar estado (0 -> 1 -> 2 -> 3 -> 0)
                setLEDs(state);
                while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0); // Esperar liberación
            }
        }

        // Switch 2 (PJ1): Secuencia Descendente
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0) {
            SysCtlDelay(g_ui32SysClock / 30); // Antirrebote (~100ms)
            if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0) {
                state = (state - 1 + 4) % 4; // Retroceder estado (3 -> 2 -> 1 -> 0 -> 3)
                setLEDs(state);
                while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0); // Esperar liberación
            }
        }
    }
}
