// IMPORTACIONES

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"


// VARIABLES GLOBALES

uint32_t g_ui32SysClock;

volatile uint32_t estado = 1;
volatile uint32_t segundos = 2;


// FUNCIONES

#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


// Secuencia de tres estados.

void mostrar_estado(void)
{
    if (estado == 1)
    {
        // Encender PN1.
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
    }
    else if (estado == 2)
    {
        // Encender PN0.
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
    }
    else
    {
        // Encender PF4 y PF0.
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x11);
    }
}


// Se conserva por su referencia en startup_gcc.c.
// Timer0 no se habilita.

void Timer0IntHandler(void)
{
}


// Timer1 cambia el estado de la secuencia.

void Timer1IntHandler(void)
{
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    estado = estado + 1;

    if (estado > 3)
    {
        estado = 1;
    }

    mostrar_estado();
}


// Interrupcion UART: recibir el tiempo desde la Raspberry.

void recibir_tiempo(void)
{
    uint32_t interrupciones;
    int32_t dato;

    interrupciones = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, interrupciones);

    while (UARTCharsAvail(UART0_BASE))
    {
        dato = UARTCharGetNonBlocking(UART0_BASE);

        // Aceptar solamente los caracteres del '1' al '9'.
        if (dato >= '1' && dato <= '9')
        {
            // Convertir el caracter recibido a numero.
            segundos = dato - '0';

            TimerDisable(TIMER1_BASE, TIMER_A);

            TimerLoadSet(
                TIMER1_BASE,
                TIMER_A,
                (g_ui32SysClock * segundos) - 1
            );

            TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
            IntPendClear(INT_TIMER1A);

            // Mantener el estado actual y reiniciar su intervalo.
            TimerEnable(TIMER1_BASE, TIMER_A);

            // Devolver el dato para confirmar la recepcion.
            UARTCharPutNonBlocking(UART0_BASE, dato);
        }
    }
}


// PROGRAMA PRINCIPAL

int main(void)
{
    // CONFIGURACION INICIAL

    IntMasterDisable();

    g_ui32SysClock = SysCtlClockFreqSet(
        SYSCTL_XTAL_25MHZ |
        SYSCTL_OSC_MAIN |
        SYSCTL_USE_PLL |
        SYSCTL_CFG_VCO_240,
        120000000
    );

    // Habilitar los puertos.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }

    // Configurar los LED.
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);

    mostrar_estado();

    // Configurar UART0, conectada al USB serial del ICDI.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03);

    UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);

    UARTConfigSetExpClk(
        UART0_BASE,
        g_ui32SysClock,
        115200,
        UART_CONFIG_WLEN_8 |
        UART_CONFIG_STOP_ONE |
        UART_CONFIG_PAR_NONE
    );

    // Registrar la funcion de interrupcion desde el programa.
    UARTIntRegister(UART0_BASE, recibir_tiempo);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    // Configurar Timer1.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
    {
    }

    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    TimerLoadSet(
        TIMER1_BASE,
        TIMER_A,
        (g_ui32SysClock * segundos) - 1
    );

    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER1A);

    // Evitar que ambas interrupciones se interrumpan entre si.
    IntPrioritySet(INT_TIMER1A, 0x80);
    IntPrioritySet(INT_UART0, 0x80);

    TimerEnable(TIMER1_BASE, TIMER_A);
    IntMasterEnable();


    // BUCLE PRINCIPAL VACIO

    while (1)
    {
    }
}