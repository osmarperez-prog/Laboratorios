//*****************************************************************************
// LIBRERIAS
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
// MANEJO DE ERRORES
//*****************************************************************************
#ifdef DEBUG
void
error(char *pcFilename, uint32_t ui32Line)
{
while(1);
}
#endif

//*****************************************************************************
// FUNCION PRINCIPAL
//*****************************************************************************
int main(void)
{
    uint32_t ui32SysClock;
    int counter = 0;

//*****************************************************************************
// HABILITAR CLOCK
//*****************************************************************************
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |
                                       SYSCTL_CFG_VCO_240), 120000000);

//*****************************************************************************
// HABILITAR PERIFERICOS
//*****************************************************************************
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }


//*****************************************************************************
// CONFIGURAR PINES
//*****************************************************************************
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);


//*****************************************************************************
// EJECUTAR FUNCIONES
//*****************************************************************************
    while(1)
    {
    //Boton 1
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0)
        {
            if(counter < 15)
            {
                counter++;
            }
            SysCtlDelay(ui32SysClock / 5);
        }

    // Boton 2
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0)
        {
            if(counter > 0)
            {
                counter--;
            }
            SysCtlDelay(ui32SysClock / 5);
        }

        if(counter & 1)
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x0);
        }

        if(counter & 2)
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x0);
        }

        if(counter & 4)
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
        }

        if(counter & 8)
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0);
        }
    }


}