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
#include "utils/uartstdio.c"

//*****************************************************************************
// VARIABLES GLOBALES (Memoria del Vehículo)
//*****************************************************************************
uint32_t ui32SysClock;
char data[100];

// Memoria de Motores y Velocidad
bool estado_m1 = false;
bool estado_m2 = false;
uint32_t ancho_pulso_actual = 45000; // 75% de 60000

// Memoria de Luces y Buzzer (Para restaurar estado)
bool estado_g1 = false;
bool estado_g2 = false;
bool sonando = false;

// Bandera de Bloqueo
volatile bool emergencia = false;

//*****************************************************************************
// FUNCION CASERA PARA COMPARAR TEXTO
//*****************************************************************************
bool comparar_cadenas(const char *cadena1, const char *cadena2)
{
    while (*cadena1 != '\0' && *cadena2 != '\0')
    {
        if (*cadena1 != *cadena2) return false;
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
void apagar_buzzer(void)
{
    sonando = false;
    if(!emergencia)
    {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, 1); // 1 tick = Silencio
    }
}

void encender_buzzer(void)
{
    sonando = true;
    if(!emergencia)
    {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, 30000); // 30000 ticks = 50% volumen
    }
}

//*****************************************************************************
// FUNCION PRINCIPAL
//*****************************************************************************
int main(void)
{
    IntMasterDisable();

//*****************************************************************************
// HABILITAR CLOCK (120 MHz)
//*****************************************************************************
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |
                                       SYSCTL_CFG_VCO_480), 120000000);

//*****************************************************************************
// HABILITAR PERIFERICOS (Eliminado TIMER0)
//*****************************************************************************
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);


//*****************************************************************************
// CONFIGURAR PINES Y COMUNICACIONES
//*****************************************************************************
// Configuración UART0

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03);
    UARTStdioConfig(0, 9600, 120000000);

// Configuración PWM0 (Motores y Buzzer)
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, 0x02); // PF1 (Motor 1)

    GPIOPinConfigure(GPIO_PG0_M0PWM4);
    GPIOPinConfigure(GPIO_PG1_M0PWM5);     // PG1 (Buzzer - Rescatado del Datasheet)
    GPIOPinTypePWM(GPIO_PORTG_BASE, 0x03); // 0x03 habilita PG0 y PG1

// Configuración GPIO Salidas
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x1D); // PF0, PF2, PF3, PF4
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x0F); // PN0, PN1, PN2, PN3
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, 0x30); // PL4, PL5 (PL0 eliminado)
    GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, 0x04); // PP2
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0x80); // PA7


//*****************************************************************************
// CONFIGURAR PWM (Motores y Buzzer unificados a 2 kHz)
//*****************************************************************************
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 60000);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, 60000);

    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMGenEnable(PWM0_BASE, PWM_GEN_2);
// Habilitar salidas 1 (M1), 4 (M2) y 5 (Buzzer)
    PWMOutputState(PWM0_BASE, (PWM_OUT_1_BIT | PWM_OUT_4_BIT | PWM_OUT_5_BIT), true);


//*****************************************************************************
// INICIALIZAR ESTADOS FÍSICOS
//*****************************************************************************
// Direccion Motores Adelante
    GPIOPinWrite(GPIO_PORTF_BASE, 0x0C, 0x04); //0000 1100 C
                                               //0000 0100 4
    GPIOPinWrite(GPIO_PORTL_BASE, 0x30, 0x20);

// PWMs al 0% mecanico / silencio
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, 1);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, 1);

// Luces Apagadas
    GPIOPinWrite(GPIO_PORTN_BASE, 0x0F, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
    GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x00);
    GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x00);

    IntMasterEnable();


//*****************************************************************************
// BUCLE INFINITO
//*****************************************************************************
    while(1)
    {
        UARTgets(data, 100);

// ================= PARADA DE EMERGENCIA =================
        if(comparar_cadenas(data, "E"))
        {
            emergencia = !emergencia;

            if(emergencia)
            {
            // APAGAR TODO FÍSICAMENTE (SIN BORRAR LA MEMORIA)
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1);
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, 1);
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, 1); // Silencia buzzer

                GPIOPinWrite(GPIO_PORTN_BASE, 0x0F, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
                GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x00);
                GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x00);

                UARTprintf("E\n");
            }
            else
            {
            // RESTAURAR ESTADOS DESDE LA MEMORIA
                if(estado_m1)
                {
                    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ancho_pulso_actual);
                    GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03);
                }
                if(estado_m2)
                {
                    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, ancho_pulso_actual);
                    GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x11);
                }
                if(estado_g1)
                {
                    GPIOPinWrite(GPIO_PORTN_BASE, 0x0C, 0x0C);
                }
                if(estado_g2)
                {
                    GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x04);
                    GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x80);
                }
                if(sonando)
                {
                    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, 30000);
                }
                UARTprintf("R\n");
            }
            continue;
        }


// ================= BLOQUEO DE SEGURIDAD =================
        if(emergencia)
        {
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

            ancho_pulso_actual = (pct * 60000) / 100;
            if(ancho_pulso_actual == 0) ancho_pulso_actual = 1;

            if(estado_m1) PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ancho_pulso_actual);
            if(estado_m2) PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, ancho_pulso_actual);
        
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
            UARTprintf("Motor 1 Alternado\n");
        }
        else if(comparar_cadenas(data, "MOTOR2"))
        {
            estado_m2 = !estado_m2;
            if(estado_m2)
            {
                PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, ancho_pulso_actual);
                GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x11); //0001 0001 11
        }
        else
        {
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, 1);
            GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
        }
        UARTprintf("Motor 2 Alternado\n");
    }

// ================= BLOQUE DE LUCES =================
        else if (comparar_cadenas(data, "G1:ON"))
        {
            estado_g1 = true;
            GPIOPinWrite(GPIO_PORTN_BASE, 0x0C, 0x0C);
            UARTprintf("Comando Ejecutado (G1:ON)\n");
        }
        else if (comparar_cadenas(data, "G1:OFF"))
        {
            estado_g1 = false;
            GPIOPinWrite(GPIO_PORTN_BASE, 0x0C, 0x00);
            UARTprintf("Comando Ejecutado (G1:OFF)\n");
        }
        else if (comparar_cadenas(data, "G2:ON"))
        {
            estado_g2 = true;
            GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x04);
            GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x80);
            UARTprintf("Comando Ejecutado (G2:ON)\n");
        }
        else if (comparar_cadenas(data, "G2:OFF"))
        {
            estado_g2 = false;
            GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x00);
            GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x00);
            UARTprintf("Comando Ejecutado (G2:OFF)\n");
        }
        else if (comparar_cadenas(data, "TRASERO:ON"))
        {
            estado_g1 = true;
            estado_g2 = true;
            GPIOPinWrite(GPIO_PORTN_BASE, 0x0C, 0x0C);
            GPIOPinWrite(GPIO_PORTP_BASE, 0x04, 0x04);
            GPIOPinWrite(GPIO_PORTA_BASE, 0x80, 0x80);
            UARTprintf("Comando Ejecutado (TRASERO:ON)\n");
        }
        else if (comparar_cadenas(data, "TRASERO:OFF"))
        {
            estado_g1 = false;
            estado_g2 = false;
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