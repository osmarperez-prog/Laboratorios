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
volatile uint32_t velocidad = 0;

// Variables para confirmar el estado del boton.
volatile uint32_t boton_estable = 0x01;
volatile uint32_t boton_candidato = 0x01;
volatile uint32_t muestras = 0;


// FUNCIONES

#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


// Mostrar el numero en los cuatro LED.
// PN1 = 8, PN0 = 4, PF4 = 2, PF0 = 1.

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


// Cambiar velocidad: 2 segundos, 1 segundo y 0,5 segundos.

void cambiar_velocidad(void)
{
    uint32_t carga;

    velocidad = velocidad + 1;

    if (velocidad > 2)
    {
        velocidad = 0;
    }

    if (velocidad == 0)
    {
        carga = (g_ui32SysClock * 2) - 1;
    }
    else if (velocidad == 1)
    {
        carga = g_ui32SysClock - 1;
    }
    else
    {
        carga = (g_ui32SysClock / 2) - 1;
    }

    TimerDisable(TIMER1_BASE, TIMER_A);

    TimerLoadSet(TIMER1_BASE, TIMER_A, carga);

    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntPendClear(INT_TIMER1A);

    TimerEnable(TIMER1_BASE, TIMER_A);
}


// Timer0: revisar el boton cada 10 milisegundos.

void Timer0IntHandler(void)
{
    uint32_t lectura;

    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    lectura = GPIOPinRead(GPIO_PORTJ_BASE, 0x01);

    // Si cambia la lectura, comenzar una nueva confirmacion.
    if (lectura != boton_candidato)
    {
        boton_candidato = lectura;
        muestras = 0;
    }
    else
    {
        if (muestras < 3)
        {
            muestras = muestras + 1;
        }

        // Confirmar el cambio despues de 30 ms sin variaciones.
        if (muestras == 3 && boton_estable != boton_candidato)
        {
            boton_estable = boton_candidato;

            // Cambiar velocidad solo al presionar.
            if (boton_estable == 0)
            {
                cambiar_velocidad();
            }
        }
    }
}


// Timer1: avanzar la cuenta de 0 a 9.

void Timer1IntHandler(void)
{
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    contador = contador + 1;

    if (contador > 9)
    {
        contador = 0;
    }

    mostrar_numero(contador);
}


// PROGRAMA PRINCIPAL

int main(void)
{
    // CONFIGURACION INICIAL

    IntMasterDisable();

    // Reloj a 120 MHz.
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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }

    // Configurar los cuatro LED como salidas.
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);

    // Configurar SW1, PJ0, como entrada.
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x01);

    GPIOPadConfigSet(
        GPIO_PORTJ_BASE,
        0x01,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );

    // Comenzar en cero y con intervalo de 2 segundos.
    contador = 0;
    velocidad = 0;
    mostrar_numero(contador);

    // Habilitar los temporizadores.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1))
    {
    }

    // Timer0: interrupcion cada 10 ms para revisar SW1.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    TimerLoadSet(
        TIMER0_BASE,
        TIMER_A,
        (g_ui32SysClock / 100) - 1
    );

    // Timer1: interrupcion inicial cada 2 segundos.
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    TimerLoadSet(
        TIMER1_BASE,
        TIMER_A,
        (g_ui32SysClock * 2) - 1
    );

    // Limpiar las interrupciones.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Habilitar las interrupciones de los temporizadores.
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    IntEnable(INT_TIMER0A);
    IntEnable(INT_TIMER1A);

    // Misma prioridad para evitar que se interrumpan entre si.
    IntPrioritySet(INT_TIMER0A, 0x80);
    IntPrioritySet(INT_TIMER1A, 0x80);

    // Iniciar los temporizadores.
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_A);

    IntMasterEnable();


    // BUCLE PRINCIPAL VACIO

    while (1)
    {
    }
}