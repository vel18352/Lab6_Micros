/*
Archivo:        Lab6_Vel18352
Dispositivo:    PIC16F887
Autor:		    Emilio Velasquez 18352
Compilador:	    XC8, MPLABX 5.40
Programa:       Contador binario y display de voltaje por ADC
Hardware:	    2 potenciometros, 8 leds, 4 displays 7 segmentos
Creado:         27/03/2023
Ultima modificacion: 27/03/2023
 */
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
#include <stdbool.h>    //Se incluyen las librerias necesarias

//******************************************************************************
// Definiciones
//******************************************************************************
#define _XTAL_FREQ 4000000  //Se establece el oscilador a 4.0Mhz

//******************************************************************************
// Variables
//******************************************************************************
uint8_t  TMR0_Pre = 255;    //Precarga TMR0
uint16_t ADC1 = 0;  
uint16_t ADC2 = 0;          //Variables para almacenar valores ADC
bool  Bandera_ADC = false;  //Bandera para rotacion de ADC
uint8_t  Bandera_Display;   //Bandera para multiplexado de display
uint8_t  Miles = 0;
uint8_t  Centenas = 0;
uint8_t  Decenas = 0;
uint8_t  Unidades = 0;      //Variables para almacenar valores de los display
uint16_t ValorDC = 0;       //Almacenar valor DC para el display
uint8_t  Tabla_Display[10] =    //Tabla para mostrar numeros en display 
{
    0xC0, //0
    0xF9, //1
    0x24, //2
    0x30, //3
    0x19, //4
    0x12, //5
    0x02, //6
    0xF8, //7
    0x00, //8
    0x10  //9   
};

//******************************************************************************
// Interrupciones
//******************************************************************************
void __interrupt() isr(void)
{
    if (T0IF)                                   //Se chequea interrupcion TMR0
    {
    PORTC = 0x00;                               //Se limpia valor PORTC
    if(Bandera_Display == 0)                    //Mostrar valor Miles en display
    {        
        PORTD = Tabla_Display[0];               //Llama a la tabla y muestra valor en PORTD
        PORTC = 0x01;                           //Enciende primer bit de PORTC
        RD7 = 1;                                //Apaga punto decimal
        Bandera_Display = 1;                    //Rota Valor de multiplexado
    }
    else if(Bandera_Display == 1)               //Mostrar valor de Centenas en display
    {        
        PORTD = Tabla_Display[Centenas];        //Llama a tabla y muestra valor en PORTD
        PORTC = 0x02;                           //Enciende segundo bit de PORTC
        RD7 = 0;                                //Enciende punto decimal
        Bandera_Display = 2;                    //Rota valor de multiplexado
    }
    else if(Bandera_Display == 2)               //Mostrar valor de Decenas en display
    {        
        PORTD = Tabla_Display[Decenas];         //Llama a tabla y muestra valor en PORTD
        PORTC = 0x04;                           //Enciende el tercer bit de PORTC
        RD7 = 1;                                //Apaga punto decimal
        Bandera_Display = 3;                    //Rota valor de multiplexado
    }
    else if(Bandera_Display == 3)               //Mostrar valor de Unidades en display
    {        
        PORTD = Tabla_Display[Unidades];        //Llama a tabla y muestra valor en PORTD
        PORTC = 0x08;                           //Enciende el cuarto bit de PORTC
        RD7 = 1;                                //Apaga punto decimal
        Bandera_Display = 0;                    //Rota valor de multiplexado
    }
       T0IF = 0;                                //Reinicia bandera TMR0
       TMR0 = TMR0_Pre;                         //Vuelve a cargar TMR0
    }
    
    if (ADIF)                                   //Chequea bandera de interrupcion de ADC
    {       
        if (Bandera_ADC)                        //En caso Bandera_ADC 
        {
            ADC1 = ADRESH;                      //Lee ADC y almacena en ADC1 la lectura del primer potenciometro
        }
        else
        {
            ADC2 = ADRESH;                      //Lee ADC y almacena en ADC2 la lectura del segundo potenciometro
        }
        ADIF = 0;                               //Reinicia bandera ADC
        Bandera_ADC = !Bandera_ADC;             //Rota la lectura de potenciometro 
    }
}

//******************************************************************************
// Configuraciones
//******************************************************************************
void Setup(void)
{
    ANSEL  = 0x03;  //Enciende primeros 2 canales del ADC
    ANSELH = 0x00;   
    TRISA = 0x03;   //Establece primeros dos bits de PORTA como entrada
    PORTA = 0;      //Limpia PORTA
    TRISB = 0;      //Establece PORTB como salida
    PORTB = 0;      //Limpia PORTB
    TRISC = 0;      //Establece PORTC como salida
    PORTC = 0;      //Limpia PORTC
    TRISD = 0;      //Establece PORTD como salida
    PORTD = 0;      //Limpia PORTD
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
    T0CS = 0;   //Reloj interno para TMR0
    PSA = 0;    //TM0 Module
    PS2 = 1; 
    PS1 = 1; 
    PS0 = 1;    //1:256 Prescaler
    TMR0 = TMR0_Pre;    //Precarga
    T0IF = 0;   //Limpiar bandera de interrupcion
}

void ADC_Int()
{   
    ADCON0 = 0b10000001;    //Enciende ADC y FOSC/32 como ADCS
    ADCON1bits.ADFM  = 0;   //Justificado a la izquierda
    ADCON1bits.VCFG1 = 0;   
    ADCON1bits.VCFG0 = 0;   //Voltajes de referencia como VDD y VSS
}

void ADC_Select(int channel)
{
    ADCON0bits.CHS0 = (channel >> 0) & 0x01;
    ADCON0bits.CHS1 = (channel >> 1) & 0x01;
    ADCON0bits.CHS2 = (channel >> 2) & 0x01;
    ADCON0bits.CHS3 = (channel >> 3) & 0x01;    //Funcion para seleccionar el canal el cual se hara lectura de ADC
}

//******************************************************************************
// Subrutinas
//******************************************************************************
void ADC_Change(void)                           //Conmutar canal el cual se le hara lectura de ADC para ambos potenciometros
{
    if (ADCON0bits.GO)                          //Se chequea que no este ocurriendo una lectura de ADC
    {           
        return;
    }
    if (Bandera_ADC)                            //En caso Bandera_ADC sea verdadero 
    {
        ADC_Select(0);                          //Se selecciona el Canal 0 para la lectura de ADC
    }
    else
    {
        ADC_Select(1);                          //Se selecciona el Canal 1 para la lectura de ADC
    }
    while(ADCON0bits.GO);                       //Espera a terminar la conversion ADC
    ADCON0bits.GO = 1;                          //Inicia la conversion ADC
}

void Separar_Valores(int Valor) 
{
    Centenas=(Valor%1000-Valor%100)/100;        //Separa las centenas eliminando Miles y obteniendo el MOD de 100
    Decenas=(Valor%100-Valor%10)/10;            //Separa las decenas eliminando Centenas y obteniendo MOD de 10
    Unidades=Valor%10;                          //Obtiene MOD de 10 para tener el DIV 
    return;
}

//******************************************************************************
// Main Loop
//******************************************************************************
void main(void) 
{
    Setup();        
    Int_Enable();
    TMR0_Config();  
    ADC_Int();                                  //Llama a las rutinas de configuracion de funciones
    while(1)                                    //Loop principal
    {
        ADC_Change();                           //Llama a la funcion de conmutar el canal de ADC
        PORTB = ADC1;                           //Muestra en PORTB la lectura del canal 0 de ADC
        ValorDC = (uint16_t)((uint32_t)ADC2 * 500 / 255);   //Obtiene el valor en un rango de 0 a 5 del valor de ADC2
        Separar_Valores(ValorDC);               //Separa los valores del valor DC 
    }
    return;
}