// IMPORTACIONES

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"


// VARIABLES GLOBALES

uint32_t g_ui32SysClock;
volatile uint32_t contador = 0;


// FUNCIONES

#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


// Mostrar el contador en los cuatro LED.
//
// D1 = PN1: representa 8.
// D2 = PN0: representa 4.
// D3 = PF4: representa 2.
// D4 = PF0: representa 1.

void mostrar_numero(uint32_t numero)
{
    uint8_t valor_n = 0x00;
    uint8_t valor_f = 0x00;

    if (numero & 0x08)
    {
        valor_n = valor_n | 0x02;
    }

    if (numero & 0x04)
    {
        valor_n = valor_n | 0x01;
    }

    if (numero & 0x02)
    {
        valor_f = valor_f | 0x10;
    }

    if (numero & 0x01)
    {
        valor_f = valor_f | 0x01;
    }

    GPIOPinWrite(GPIO_PORTN_BASE, 0x03, valor_n);
    GPIOPinWrite(GPIO_PORTF_BASE, 0x11, valor_f);
}


// Se conserva porque aparece en startup_gcc.c.
// Timer0 no se utiliza en este ejercicio.

void Timer0IntHandler(void)
{
}


// Interrupcion del temporizador 1.

void Timer1IntHandler(void)
{
    // Limpiar la interrupcion atendida.
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Avanzar al siguiente numero.
    contador = contador + 1;

    // Despues del 9, volver al 0.
    if (contador > 9)
    {
        contador = 0;
    }

    // Actualizar los LED.
    mostrar_numero(contador);
}


// PROGRAMA PRINCIPAL

int main(void)
{
    // CONFIGURACION INICIAL

    IntMasterDisable();

    // Configurar el reloj a 120 MHz.
    g_ui32SysClock = SysCtlClockFreqSet(
        SYSCTL_XTAL_25MHZ |
        SYSCTL_OSC_MAIN |
        SYSCTL_USE_PLL |
        SYSCTL_CFG_VCO_240,
        120000000
    );

    // Habilitar los puertos N y F.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Esperar hasta que los puertos esten listos.
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    // Configurar PN1 y PN0 como salidas.
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);

    // Configurar PF4 y PF0 como salidas.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);

    // Comenzar en cero, con los cuatro LED apagados.
    contador = 0;
    mostrar_numero(contador);

    // Habilitar el temporizador 1.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
    {
    }

    // Configurar el temporizador en modo periodico.
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    // Generar una interrupcion cada 2 segundos.
    TimerLoadSet(
        TIMER1_BASE,
        TIMER_A,
        (g_ui32SysClock * 2) - 1
    );

    // Limpiar y habilitar la interrupcion.
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER1A);

    // Habilitar las interrupciones del procesador.
    IntMasterEnable();

    // Iniciar el temporizador.
    TimerEnable(TIMER1_BASE, TIMER_A);


    // BUCLE PRINCIPAL

    while (1)
    {
        // El contador se actualiza en la interrupcion.
    }
}