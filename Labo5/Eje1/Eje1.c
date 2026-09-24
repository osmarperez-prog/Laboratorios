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
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.c"
#include <string.h>
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
//============================== VARIABLES ==============================
uint32_t g_ui32SysClock;
uint8_t contador = 0;
uint8_t numero = 0;
char data[100];
char msg[] = "Ingrese un dato: ";
uint32_t LeerADC(void);
uint32_t valorADC;
uint32_t tiempo;
//============================== FUNCIONES ==============================

void Timer0IntHandler(void);
void ConfigurarADC(void);
void ConfigurarPWM(void);
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
    //============================== CONFIGURACION UART ==============================

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
        ;
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
        ;

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03);

    UARTStdioConfig(0, 9600, g_ui32SysClock);
    //============================== CONFIGURACION ADC ==============================
    ConfigurarADC();
    //============================== CONFIGURACION PWM ==============================
    ConfigurarPWM();
    //============================== EJECUCION PRINCIPAL ==============================
    while (1)
    {
        UARTgets(data, 100);
        if (strcmp(data, "PN1:ON") == 0)
            GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);
        else if (strcmp(data, "PN1:OFF") == 0)
            GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0);
        else if (strcmp(data, "PN0:ON") == 0)
            GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);
        else if (strcmp(data, "PN0:OFF") == 0)
            GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0);
        else if (strcmp(data, "PF4:ON") == 0)
            GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
        else if (strcmp(data, "PF4:OFF") == 0)
            GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0);
        else if (strcmp(data, "PF0:ON") == 0)
            GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x01);
        else if (strcmp(data, "PF0:OFF") == 0)
            GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0);

        UARTprintf("Comando Ejecutado\n");
    }
}

//============================== INTERRUPCION POR TIMER ==============================
void Timer0IntHandler(void)
{

    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

//============================== FIN INTERRUPCION POR TIMER ==============================

//============================== CONFIGURACION ADC ==============================
void ConfigurarADC(void)
{
    // Habilitar puerto K
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK))
        ;

    // Habilitar ADC0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
        ;

    // PK3 = 0x08
    // PK3 corresponde al canal AIN19

    GPIOPinTypeADC(GPIO_PORTK_BASE,
                   0x08);

    // Configurar secuenciador 3
    ADCSequenceConfigure(ADC0_BASE,
                         3,
                         ADC_TRIGGER_PROCESSOR,
                         0);

    // Seleccionar canal 19
    ADCSequenceStepConfigure(ADC0_BASE,
                             3,
                             0,
                             ADC_CTL_CH19 |
                                 ADC_CTL_IE |
                                 ADC_CTL_END);

    // Habilitar secuenciador
    ADCSequenceEnable(ADC0_BASE,
                      3);

    // Limpiar bandera de interrupción
    ADCIntClear(ADC0_BASE,
                3);
}
uint32_t LeerADC(void)
{
    uint32_t valor;

    // Iniciar conversión
    ADCProcessorTrigger(ADC0_BASE,
                        3);

    // Esperar hasta que termine
    while (!ADCIntStatus(ADC0_BASE,
                         3,
                         false))
        ;

    // Limpiar bandera
    ADCIntClear(ADC0_BASE,
                3);

    // Obtener valor convertido
    ADCSequenceDataGet(ADC0_BASE,
                       3,
                       &valor);

    return valor;
}
//============================== CONFIGURACION PWM ==============================

void ConfigurarPWM(void)
{
    uint32_t periodo;
    uint32_t pulso;

    //============================== HABILITAR PWM ==============================

    // Habilitar módulo PWM0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
        ;

    // Habilitar puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
        ;

    //============================== CONFIGURACION PIN PWM ==============================

    // PF1 = 0x02
    // PF1 corresponde a M0PWM1

    GPIOPinConfigure(GPIO_PF1_M0PWM1);

    GPIOPinTypePWM(GPIO_PORTF_BASE,
                   0x02);

    //============================== CALCULO DEL PERIODO ==============================

    // Periodo de 20 us
    // periodo = tiempo * frecuencia del reloj

    periodo = g_ui32SysClock / 50000;

    //============================== CONFIGURACION GENERADOR PWM ==============================

    PWMGenConfigure(PWM0_BASE,
                    PWM_GEN_0,
                    PWM_GEN_MODE_DOWN |
                        PWM_GEN_MODE_NO_SYNC);

    // Configurar periodo
    PWMGenPeriodSet(PWM0_BASE,
                    PWM_GEN_0,
                    periodo);

    //============================== DUTY CYCLE 75% ==============================

    pulso = (periodo * 75) / 100;

    PWMPulseWidthSet(PWM0_BASE,
                     PWM_OUT_1,
                     pulso);

    //============================== HABILITAR PWM ==============================

    PWMGenEnable(PWM0_BASE,
                 PWM_GEN_0);

    PWMOutputState(PWM0_BASE,
                   PWM_OUT_1_BIT,
                   true);
}

//============================== FIN CONFIGURACION PWM ==============================
//============================== CONFIGURACION I2C ==============================

// No existe I2C en el código original.

//============================== RUTINA DE ERROR ==============================

#ifdef DEBUG

void __error__(char *pcFilename, uint32_t ui32Line)
{
    while (1)
        ;
}

#endif

//============================== FIN DEL PROGRAMA ==============================