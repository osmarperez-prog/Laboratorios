//============================== LIBRERIAS ==============================

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_ints.h"

//============================== VARIABLES ==============================
uint32_t g_ui32SysClock;
uint8_t contador = 0;
uint8_t numero = 0;

//============================== FUNCIONES ==============================

void Timer0IntHandler(void);

//============================== MAIN ==============================

int main(void)
{
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                         SYSCTL_OSC_MAIN |
                                         SYSCTL_USE_PLL |
                                         SYSCTL_CFG_VCO_480),
                                        120000000);

    //============================== CONFIGURACION GPIO ==============================

    // Habilitar puertos N y F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); // N1,N0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // F4,F0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // J0,J1
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
        ;

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
        ;
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
        ;
    // Configuracion de pineS LED
    // PN0 = 0x01
    // PN1 = 0x02
    // PN0 + PN1 = 0x03
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);

    // PF0 = 0x01
    // PF4 = 0x10
    // PF0 + PF4 = 0x11

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);

    // CONFIGURACION DE PINE DE BOTON
    //  PJ0 = 0x01
    //  PJ1 = 0x02
    //  PJ0 + PJ1 = 0x03
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x03);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, 0x03,
                     GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);

    //============================== CONFIGURACION TIMER ==============================

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
        ;
    // Configurar Timer 0A como periódico
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // Configurar el tiempo del Timer
    TimerLoadSet(TIMER0_BASE, TIMER_A,
                 g_ui32SysClock / 1); // 1 segundo
    // Habilitar interrupción del Timer
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Habilitar interrupciones globales
    IntMasterEnable();
    // Iniciar Timer
    TimerEnable(TIMER0_BASE, TIMER_A);

    //============================== EJECUCION PRINCIPAL ==============================
    while (1)
    {
    }
}

//============================== INTERRUPCION POR TIMER ==============================

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //============================== CAMBIO DE INTERVALO ==============================

    if (GPIOPinRead(GPIO_PORTJ_BASE, 0x01) == 0)
    {
        // Botón presionado = 3 segundos
        TimerLoadSet(TIMER0_BASE,
                     TIMER_A,
                     g_ui32SysClock * 3);
    }
    else
    {
        // Botón no presionado = 1.5 segundos
        TimerLoadSet(TIMER0_BASE,
                     TIMER_A,
                     (g_ui32SysClock * 3) / 2);
    }

    //================================================
    // CONTADOR BINARIO
    //================================================

    numero++;

    if (numero > 15)
    {
        numero = 0;
    }

    //================================================
    // MOSTRAR NUMERO EN BINARIO
    //================================================

    // PN0 = bit 0
    GPIOPinWrite(GPIO_PORTN_BASE,
                 0x01,
                 (numero & 0x01) ? 0x01 : 0x00);

    // PN1 = bit 1
    GPIOPinWrite(GPIO_PORTN_BASE,
                 0x02,
                 (numero & 0x02) ? 0x02 : 0x00);

    // PF0 = bit 2
    GPIOPinWrite(GPIO_PORTF_BASE,
                 0x01,
                 (numero & 0x04) ? 0x01 : 0x00);

    // PF4 = bit 3
    GPIOPinWrite(GPIO_PORTF_BASE,
                 0x10,
                 (numero & 0x08) ? 0x10 : 0x00);
}

//============================== FIN INTERRUPCION POR TIMER ==============================

//============================== CONFIGURACION ADC ==============================

// No existe ADC en el código original.

//============================== CONFIGURACION PWM ==============================

// No existe PWM en el código original.

//============================== CONFIGURACION I2C ==============================

// No existe I2C en el código original.

//============================== CONFIGURACION BOTON ==============================

// No existe configuración de botones en el código original.

//============================== RUTINA DE ERROR ==============================

#ifdef DEBUG

void __error__(char *pcFilename, uint32_t ui32Line)
{
    while (1)
        ;
}

#endif

//============================== FIN DEL PROGRAMA ==============================
