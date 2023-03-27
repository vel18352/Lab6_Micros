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
uint8_t TM0_Pre = 61;

//******************************************************************************
// Interrupciones
//******************************************************************************
void __interrupt() isr(void)
{
    if (T0IF)
    {
        RB0 = !RB0;
        T0IF = 0;
    }
}

//******************************************************************************
// Setups
//******************************************************************************
void Setup(void)
{
    ANSEL = 0;
    ANSELH = 0; 
    
    TRISB = 0;
    PORTB = 0;
    TRISC = 0;
    PORTC = 0;
   
    return;
}

void Int_Enable(void)
{
    T0IF = 0;
    T0IE = 1;
    GIE = 1;
    return;
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
    return;
}

//******************************************************************************
// Loop
//******************************************************************************
void main(void) 
{
    Setup();
    Int_Enable();
    TMR0_Config();
    
    while(1)
    {

    }
    return;
}

/*
void ADC_Init(void) 
  {
    ADCON0bits.ADON = 1; 
    ADCON1bits.ADFM = 1; 
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0; 
    ADCON0bits.CHS = 0; 
}

int ADC_Read(int channel) 
 {
    ADCON0bits.CHS = channel; 
    ADCON0bits.GO = 1; 
    while (ADCON0bits.GO);
    return ((ADRESH << 8) + ADRESL); 
}

void main(void) 
   {
    ADC_Init(); 
    int valor_adc; 
    while(1) 
   { 
        valor_adc = ADC_Read(0);   
   }
}
 */
