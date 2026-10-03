//*****************************************************************************
// LIBRERIAS
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"

//*****************************************************************************
// VARIABLES GLOBALES
//*****************************************************************************
uint32_t ui32SysClock;
int state = 1;

//*****************************************************************************
// RUTINA DE INTERRUPCION DEL TIMER 0A
//*****************************************************************************
void timer0A_handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);

    if(state == 1)
    {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);
    }
    else if(state == 2)
    {
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);
    }
    else if(state == 3)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
    }
    else if(state == 4)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x01);
    }

    state++;
    if(state > 4)
    {
        state = 1;
    }


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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}


//*****************************************************************************
// CONFIGURAR PINES Y ADC
//*****************************************************************************
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);

    GPIOPinTypeADC(GPIO_PORTK_BASE, 0x08);

    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH19);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);


//*****************************************************************************
// CONFIGURAR TIMER 0A
//*****************************************************************************
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32SysClock);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);


//*****************************************************************************
// BUCLE INFINITO
//*****************************************************************************
    uint32_t adcValue;

    while(1)
    {
        ADCProcessorTrigger(ADC0_BASE, 3);
        while(!ADCIntStatus(ADC0_BASE, 3, false)) {}
        ADCIntClear(ADC0_BASE, 3);
        ADCSequenceDataGet(ADC0_BASE, 3, &adcValue);

        if (adcValue < 100)
        {
            adcValue = 100;
        }

        uint32_t timer_load = (ui32SysClock / 4095) * adcValue;
        TimerLoadSet(TIMER0_BASE, TIMER_A, timer_load);

        SysCtlDelay(ui32SysClock / 30);
    }


}