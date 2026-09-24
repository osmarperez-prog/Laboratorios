```c
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

    int
    main(void)
{
    uint32_t ui32SysClock;

    char cComando;

    //=========================================================
    // CONFIGURACION DEL RELOJ
    //=========================================================

    ui32SysClock = SysCtlClockFreqSet(
        (SYSCTL_XTAL_25MHZ |
         SYSCTL_OSC_MAIN |
         SYSCTL_USE_PLL |
         SYSCTL_CFG_VCO_480),
        120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }

    //=========================================================
    // HABILITAR UART0
    //=========================================================

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }

    //=========================================================
    // CONFIGURAR PINES PA0 Y PA1 PARA UART
    //=========================================================

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPinTypeUART(
        GPIO_PORTA_BASE,
        GPIO_PIN_0 | GPIO_PIN_1);

    //=========================================================
    // CONFIGURAR UART0
    //
    // 115200 baudios
    // 8 bits
    // 1 bit de parada
    // Sin paridad
    //=========================================================

    UARTConfigSetExpClk(
        UART0_BASE,
        ui32SysClock,
        115200,
        (UART_CONFIG_WLEN_8 |
         UART_CONFIG_STOP_ONE |
         UART_CONFIG_PAR_NONE));

    //=========================================================
    // INICIALIZAR UARTSTDIO
    //=========================================================

    UARTStdioConfig(
        0,
        115200,
        ui32SysClock);

    //=========================================================
    // MENSAJE INICIAL
    //=========================================================

    UARTprintf("\r\n");
    UARTprintf("================================\r\n");
    UARTprintf("   TIVA TM4C1294XL - UART\r\n");
    UARTprintf("================================\r\n");
    UARTprintf("UART funcionando correctamente.\r\n");
    UARTprintf("Baudrate: 115200\r\n");
    UARTprintf("Envie '1' para encender PN0.\r\n");
    UARTprintf("Envie '0' para apagar PN0.\r\n");
    UARTprintf("\r\n");

    //=========================================================
    // LOOP PRINCIPAL
    //=========================================================

    while (1)
    {
        // Esperar un caracter recibido por UART
        cComando = UARTgetc();

        // Mostrar el comando recibido
        UARTprintf("Comando recibido: %c\r\n", cComando);

        //=====================================================
        // COMANDO 1 -> ENCENDER LED PN0
        //=====================================================

        if (cComando == '1')
        {
            GPIOPinWrite(
                GPIO_PORTN_BASE,
                GPIO_PIN_0,
                GPIO_PIN_0);

            UARTprintf("LED PN0: ENCENDIDO\r\n");
        }

        //=====================================================
        // COMANDO 0 -> APAGAR LED PN0
        //=====================================================

        else if (cComando == '0')
        {
            GPIOPinWrite(
                GPIO_PORTN_BASE,
                GPIO_PIN_0,
                0);

            UARTprintf("LED PN0: APAGADO\r\n");
        }

        //=====================================================
        // OTRO CARACTER
        //=====================================================

        else
        {
            UARTprintf("Comando no valido.\r\n");
            UARTprintf("Use '1' para ON o '0' para OFF.\r\n");
        }
    }
}
```

    //============================== LEER ADC ==============================

    valorADC = LeerADC();

//============================== CALCULAR TIEMPO ==============================

// ADC 0    -> 1.5 segundos
// ADC 4095 -> 3 segundos

tiempo = (g_ui32SysClock * 3) / 2;

tiempo = tiempo +
         ((g_ui32SysClock * 3) / 2 * valorADC) / 4095;

TimerLoadSet(TIMER0_BASE,
             TIMER_A,
             tiempo);

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

uint32_t valorADC;
uint32_t tiempo;