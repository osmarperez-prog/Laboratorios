//*****************************************************************************
// LIBRERIAS
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.c"

//*****************************************************************************
// VARIABLES GLOBALES
//*****************************************************************************
uint32_t ui32SysClock;
char data[100];

// Estados Motores y Velocidad PWM (Periodo = 60000 ticks)
bool estado_m1 = false;
bool estado_m2 = false;
uint32_t ancho_pulso_actual = 45000; // Valor inicial al 75% (45000 / 60000)

// Estados Buzzer y Emergencia
const uint32_t frecuencia = 2000;
volatile bool emergencia = false;
volatile bool sonando = false;
volatile uint8_t nivel = 0x00;

//*****************************************************************************
// FUNCION CASERA PARA COMPARAR TEXTO (REEMPLAZO DE STRCMP)
//*****************************************************************************
bool comparar_cadenas(const char *cadena1, const char *cadena2)
{
    while (*cadena1 != '\0' && *cadena2 != '\0')
    {
        if (*cadena1 != *cadena2)
        {
            return false;
        }
        cadena1++;
        cadena2++;
    }
    return (*cadena1 == *cadena2);
}

//*****************************************************************************
// MANEJO DE ERRORES
//*****************************************************************************
#ifdef DEBUG
void error(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
// FUNCIONES DEL BUZZER
//*****************************************************************************
void timer0A_handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if (sonando && !emergencia)
    {
        nivel = nivel ^ 0x01; // Alterna entre 0 y 1
        GPIOPinWrite(GPIO_PORTL_BASE, 0x01, nivel); // Pin PL0
    }
    else
    {
        nivel = 0x00;
        GPIOPinWrite(GPIO_PORTL_BASE, 0x01, 0x00);
    }
}

void apagar_buzzer(void)
{
    sonando = false;
    TimerDisable(TIMER0_BASE, TIMER_A);
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntPendClear(INT_TIMER0A);
    nivel = 0x00;
    GPIOPinWrite(GPIO_PORTL_BASE, 0x01, 0x00);
}

void encender_buzzer(void)
{
    if (emergencia)
    {
        return;
    }

    apagar_buzzer();

    // Cargar nuevo periodo para el tono
    TimerLoadSet(TIMER0_BASE, TIMER_A, (ui32SysClock / (2 * frecuencia)) - 1);

    sonando = true;
    TimerEnable(TIMER0_BASE, TIMER_A);
}

//*****************************************************************************
// FUNCION PRINCIPAL
//*****************************************************************************
int main(void)
{
    // Deshabilitar interrupciones
    IntMasterDisable();

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
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP)) {}


//*****************************************************************************
// CONFIGURAR PINES Y COMUNICACIONES
//*****************************************************************************
    // Configuración UART0  
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03);
    UARTStdioConfig(0, 9600, 120000000);

    // Configuración PWM0
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, 0x02); // PF1

    GPIOPinConfigure(GPIO_PG0_M0PWM4);
    GPIOPinTypePWM(GPIO_PORTG_BASE, 0x01); // PG0


    // Configuración GPIO Salidas
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x1D); // PF0, PF2, PF3, PF4
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x0F); // PN0, PN1, PN2, PN3
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, 0x31); // PL4, PL5 (M2 Dir) + PL0 (Buzzer)
    GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, 0x04); // PP2 (Luces G2)
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0x80); // PA7 (Luces G2)

//*****************************************************************************
// CONFIGURAR PWM (Motores) Y TIMER (Buzzer)
//*****************************************************************************
    // Generadores PWM a 2 kHz (60,000 ticks)
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 60000);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, 60000);

    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMGenEnable(PWM0_BASE, PWM_GEN_2);
    PWMOutputState(PWM0_BASE, (PWM_OUT_1_BIT | PWM_OUT_4_BIT), true);


    // Timer0 para el Buzzer
    TimerDisable(TIMER0_BASE, TIMER_A);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (ui32SysClock / (2 * frecuencia)) - 1);
    TimerIntRegister(TIMER0_BASE, TIMER_A, timer0A_handler);
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntPrioritySet(INT_TIMER0A, 0x80);

//*****************************************************************************
// INICIALIZAR ESTADOS FÍSICOS
//*****************************************************************************
    // Direccion Motores Adelante y Buzzer Apagado
    GPIOPinWrite(GPIO_PORTF_BASE, 0x0C, 0x04); // PF3=0, PF2=1
    GPIOPinWrite(GPIO_PORTL_BASE, 0x31, 0x20); // PL4=0, PL5=1, PL0=0

    // Motores al 0% mecanico
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, 1);

    // Luces Apagadas
    GPIOPinWrite(GPIO_PORTN_BASE, 0x0F, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
    GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x00);
    GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x00);

    // Habilitar Interrupciones Generales
    apagar_buzzer();
    IntMasterEnable();

//*****************************************************************************
// BUCLE INFINITO
//*****************************************************************************
    while(1)
    {
        // Se pausa aquí esperando comando desde Raspberry
        UARTgets(data, 100);

// ================= PARADA DE EMERGENCIA =================
        if(comparar_cadenas(data, "E"))
        {
            emergencia = !emergencia;
            apagar_buzzer();

            if(emergencia)
            {
                // Cortar potencia a motores
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1);
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, 1);
                estado_m1 = false;
                estado_m2 = false;

                // Apagar todas las luces y LEDs indicadores
                GPIOPinWrite(GPIO_PORTN_BASE, 0x0F, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
                GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x00);
                GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x00);

                UARTprintf("E\n");
            }
            else
            {
                UARTprintf("R\n");
            }
            continue; // Saltar validaciones y reiniciar bucle
        }

// ================= BLOQUEO DE SEGURIDAD =================
        if(emergencia)
        {
            // Si intenta mandar cualquier comando en estado de emergencia
            UARTprintf("!\n");
            continue; 
        }

// ================= BLOQUE DE VELOCIDAD =================
        if(data[0] == 'V' && data[1] == 'E' && data[2] == 'L' && data[3] == ':')
        {
            uint32_t pct = 0;
            int i = 4;
            while(data[i] >= '0' && data[i] <= '9')
            {
                pct = pct * 10 + (data[i] - '0');
                i++;
            }
            if(pct > 100) pct = 100;

            // Mapear porcentaje (0 a 100) al rango de ticks del PWM (1 a 60000)
            ancho_pulso_actual = (pct * 60000) / 100;
            if(ancho_pulso_actual == 0) ancho_pulso_actual = 1;

            // Aplicar la nueva velocidad a los motores que estén encendidos
            if(estado_m1)
            {
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ancho_pulso_actual);
            }
            if(estado_m2)
            {
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, ancho_pulso_actual);
            }
            UARTprintf("Velocidad actualizada a %d%%\n", pct);
        }

// ================= BLOQUE DE BUZZER =================
        else if(comparar_cadenas(data, "B"))
        {
            encender_buzzer();
            UARTprintf("B\n");
        }
        else if(comparar_cadenas(data, "b"))
        {
            apagar_buzzer();
            UARTprintf("b\n");
        }

// ================= BLOQUE DE MOTORES =================
        else if(comparar_cadenas(data, "MOTOR1"))
        {
            estado_m1 = !estado_m1;
            if(estado_m1)
            {
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ancho_pulso_actual);
                GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03);
            }
            else
            {
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1);
                GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00);
            }
            UARTprintf("Motor 1 (PWM PF1, LEDs PN0 PN1) Alternado\n");
        }
        else if(comparar_cadenas(data, "MOTOR2"))
        {
            estado_m2 = !estado_m2;
            if(estado_m2)
            {
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, ancho_pulso_actual);
                GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x11);
            }
            else
            {
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, 1);
                GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
            }
            UARTprintf("Motor 2 (PWM PG0, LEDs PF0 PF4) Alternado\n");
        }

// ================= BLOQUE DE LUCES =================
        else if (comparar_cadenas(data, "G1:ON"))
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x0C, 0x0C); 
            UARTprintf("Comando Ejecutado (G1:ON)\n");
        }
        else if (comparar_cadenas(data, "G1:OFF"))
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x0C, 0x00); 
            UARTprintf("Comando Ejecutado (G1:OFF)\n");
        }
        else if (comparar_cadenas(data, "G2:ON"))
        {
            GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x04); 
            GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x80); 
            UARTprintf("Comando Ejecutado (G2:ON)\n");
        }
        else if (comparar_cadenas(data, "G2:OFF"))
        {
            GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x00); 
            GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x80); 
            UARTprintf("Comando Ejecutado (G2:OFF)\n");
        }
        else if (comparar_cadenas(data, "TRASERO:ON"))
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x0C, 0x0C); 
            GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x04); 
            GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x80); 
            UARTprintf("Comando Ejecutado (TRASERO:ON)\n");
        }
        else if (comparar_cadenas(data, "TRASERO:OFF"))
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x0C, 0x00); 
            GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x00); 
            GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x00); 
            UARTprintf("Comando Ejecutado (TRASERO:OFF)\n");
        }
        else
        {
            UARTprintf("Comando no reconocido.\n");
        }
    }
}