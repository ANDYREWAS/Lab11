/*
 * File:   master.c
 * Author: Josea
 *
 * Created on 11 de mayo de 2022, 08:52 PM
 */



// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 * CONSTANTES 
 ------------------------------------------------------------------------------*/
#define _XTAL_FREQ 1000000

/*------------------------------------------------------------------------------
 * VARIABLES 
 ------------------------------------------------------------------------------*/
uint8_t contador = 0, pot = 0;


/*------------------------------------------------------------------------------
 * PROTOTIPO DE FUNCIONES 
 ------------------------------------------------------------------------------*/
void setup(void);

/*------------------------------------------------------------------------------
 * INTERRUPCIONES 
 ------------------------------------------------------------------------------*/
void __interrupt() isr (void){
    //ADC
    if(PIR1bits.ADIF){              // Fue interrupci?n del ADC?
        if(ADCON0bits.CHS == 0){    // Verificamos sea AN0 el canal seleccionado
            pot = ADRESH;         // guardamos los bits superiores en el registro para enviarlo 
           
        }
        
        PIR1bits.ADIF = 0;          // Limpiamos bandera de interrupci?n
    }
    
    
    
}


/*------------------------------------------------------------------------------
 * CICLO PRINCIPAL
 ------------------------------------------------------------------------------*/
void main(void) {
    setup();
    while(1){
        //ADC 
        if(ADCON0bits.GO == 0){             // No hay proceso de conversion
            ADCON0bits.GO = 1;              // Iniciamos proceso de conversi?n
         }
        PORTAbits.RA7 = 1;
        __delay_ms(10);
        PORTAbits.RA7 = 0;
        
  
       
        SSPBUF = pot;
        while(SSPSTATbits.BF){}
        PORTB = SSPBUF;
        __delay_ms(1000);
    }
    return;
}
    
/*------------------------------------------------------------------------------
 * CONFIGURACION 
 ------------------------------------------------------------------------------*/
void setup(void){
    ANSEL = 0;
    ANSELH = 0;                 // I/O digitales
    
    OSCCONbits.IRCF = 0b100;    // 1MHz
    OSCCONbits.SCS = 1;         // Reloj interno
    
    TRISA = 0b00100011;         // SS, RA0 y RA1 como entradas
    PORTA = 0;
    
    TRISE = 0b00000001;         //RE0 como selector master/slave
    PORTE = 0; 
    
    TRISC = 0b00010000;         // -> SDI entrada, SCK y SD0 como salida
    PORTC = 0;

    TRISB = 0;
    PORTB = 0;

    // SSPCON <5:0>
    SSPCONbits.SSPM = 0b0000;   // -> SPI Maestro, Reloj -> Fosc/4 (250kbits/s)
    SSPCONbits.CKP = 0;         // -> Reloj inactivo en 0
    SSPCONbits.SSPEN = 1;       // -> Habilitamos pines de SPI
    // SSPSTAT<7:6>
    SSPSTATbits.CKE = 1;        // -> Dato enviado cada flanco de subida
    SSPSTATbits.SMP = 1;        // -> Dato al final del pulso de reloj


    //ADC
     ANSEL = 0b00000001; // AN0 como entrada anal?gica
     ANSELH = 0;         // I/O digitales)



     ADCON0bits.CHS = 0b0000;    // Seleccionamos AN0
     ADCON1bits.ADFM = 0;        // Justificado a la izquierda
     ADCON0bits.ADON = 1;        // Habilitamos modulo ADC
     __delay_us(40);

     PIR1bits.ADIF = 0;          // Limpiamos bandera de int. ADC
     PIE1bits.ADIE = 1;          // Habilitamos int. de ADC
     INTCONbits.PEIE = 1;        // Habilitamos int. de perifericos
     INTCONbits.GIE = 1;         // Habilitamos int. globales
}