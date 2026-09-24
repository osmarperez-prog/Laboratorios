//*****************************************************************************
// LIBRERIAS
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include <string.h>
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.c"

//*****************************************************************************
// VARIABLES GLOBALES
//*****************************************************************************
uint32_t ui32SysClock;
float distance = 100.0;

//*****************************************************************************
// FUNCION CONVERTIDORA DE TEXTO A FLOAT (REEMPLAZO DE ATOF)
//*****************************************************************************
float string_to_float(const char *str)
{
    float result = 0.0;
    float factor = 1.0;
    bool fractional = false;
    bool negative = false;

    if (*str == '-') 
    {
        negative = true;
        str++;
    }

    while (*str) 
    {
        if (*str == '.') 
        {
            fractional = true;
        } 
        else if (*str >= '0' && *str <= '9') 
        {
            if (fractional) 
            {
                factor /= 10.0;
                result += (*str - '0') * factor;
            } 
            else 
            {
                result = result * 10.0 + (*str - '0');
            }
        } 
        else 
        {
            break; 
        }
        str++;
    }

    if (negative)
    {
        return -result;
    }
    return result;


}

//*****************************************************************************
// FUNCION PRINCIPAL
//*****************************************************************************
int main(void)
{
//*****************************************************************************
// HABILITAR CLOCK
//*****************************************************************************
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |
                                       SYSCTL_CFG_VCO_480), 120000000);

//*****************************************************************************
// HABILITAR PERIFERICOS
//*****************************************************************************
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}


//*****************************************************************************
// CONFIGURAR PINES Y UART
//*****************************************************************************
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03); 

    UARTConfigSetExpClk(UART0_BASE, ui32SysClock, 115200, 
               (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


//*****************************************************************************
// BUCLE INFINITO
//*****************************************************************************
    char buffer[15];
    int idx = 0;

    while(1)
    {
        while(UARTCharsAvail(UART0_BASE))
        {
            char c = UARTCharGetNonBlocking(UART0_BASE);

            if(c == '\n')
            {
                buffer[idx] = '\0';
                distance = string_to_float(buffer); 
                idx = 0; 
            }
            else
            {
                if(idx < 14)
                {
                    buffer[idx++] = c;
                }
            }
        }

        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);

        if (distance > 10.0) 
        {
        } 
        else if (distance >= 8.0 && distance <= 10.0) 
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02); 
        }
        else if (distance >= 6.0 && distance < 8.0) 
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03); 
        }
        else if (distance >= 4.0 && distance < 6.0) 
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03); 
            GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10); 
        }
        else if (distance < 4.0) 
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03); 
            GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x11); 
        }

        SysCtlDelay(ui32SysClock / 30); 
    }


}