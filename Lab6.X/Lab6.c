//******************************************************************************
// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
//******************************************************************************

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT
#pragma config WDTE = OFF       
#pragma config PWRTE = OFF      
#pragma config MCLRE = OFF      
#pragma config CP = OFF         
#pragma config CPD = OFF        
#pragma config BOREN = OFF      
#pragma config IESO = OFF       
#pragma config FCMEN = OFF      
#pragma config LVP = OFF        

// CONFIG2
#pragma config BOR4V = BOR40V   
#pragma config WRT = OFF        

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//******************************************************************************
// Includes
//******************************************************************************
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

//******************************************************************************
// Defines
//******************************************************************************
#define _XTAL_FREQ 4000000

void Setup(void);
void Int_Enable(void);
void TMR0_Config(void);

//******************************************************************************
// Variables
//******************************************************************************
uint8_t  TM0_Pre = 61;
uint16_t ADC1 = 0;
uint16_t ADC2 = 0;
uint8_t  Numero = 0;
bool     adc_flag = false;
//******************************************************************************
// Interrupciones
//******************************************************************************
void __interrupt() isr(void)
{
    T0IF = 0;
    if (ADIF)
    {       
        if (adc_flag)
        {
            ADC1 = ADRESH;
        }
        else
        {
            ADC2 = ADRESH;
        }
        ADIF = 0;
        adc_flag = !adc_flag;
    }
}

//******************************************************************************
// Setups
//******************************************************************************
void Setup(void)
{
    ANSEL  = 0x03;
    ANSELH = 0x00;   
    TRISA = 0x03;
    TRISB = 0;
    PORTB = 0;
    TRISC = 0;
    PORTC = 0;
    TRISD = 0;
    PORTD = 0;
}

void Int_Enable(void)
{
    T0IF = 0;
    T0IE = 1;   //Habilitar interrupciones TMR0
    ADIF = 0;
    ADIE = 1;   //Habilitar interrupciones ADC
    PEIE = 1;   //Habilitar interrupciones Perifericas
    GIE = 1;    //Habilitar interrupciones Globales
}

void TMR0_Config(void)
{
    T0CS = 0;
    PSA = 0;
    PS2 = 1; 
    PS1 = 1; 
    PS0 = 1;
    TMR0 = 61;
    T0IF = 0;
}

void ADC_Int()
{
    ADCON0 = 0b10000001;
    ADCON1bits.ADFM  = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.VCFG0 = 0;
}

void ADC_Select(int channel)
{
    ADCON0bits.CHS0 = (channel >> 0) & 0x01;
    ADCON0bits.CHS1 = (channel >> 1) & 0x01;
    ADCON0bits.CHS2 = (channel >> 2) & 0x01;
    ADCON0bits.CHS3 = (channel >> 3) & 0x01;
}

void ADC_Change(void)
{
    if (ADCON0bits.GO)
    {
        return;
    }
    if (adc_flag)
    {
        ADC_Select(0);
    }
    else
    {
        ADC_Select(1);
    }
    while(ADCON0bits.GO);
    ADCON0bits.GO = 1;
}


//******************************************************************************
// Loop
//******************************************************************************
void main(void) 
{
    Setup();
    Int_Enable();
    TMR0_Config();
    
    ADC_Int();
    
    while(1)
    {
        ADC_Change();
        PORTB = ADC1;
        PORTD = ADC2;
    }
    return;
}
