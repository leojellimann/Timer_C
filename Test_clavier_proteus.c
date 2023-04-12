/*
 * File:   Prog_1.c
 * Author: sittler
 *
 * Created on 17 mai 2019, 16:21
 */
// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable (All VCAP pin functionality is disabled)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)


#include <xc.h>
#include "config.h"
#include "hd44780.h"
#define _XTAL_FREQ 4000000

unsigned char Touche=0;
unsigned char Old_Touche=0;
unsigned char i=0;







void lecture_clav_init()
{
#define col1 PORTEbits.RE0
#define col2 PORTEbits.RE1 
#define col3 PORTEbits.RE2
    
     //***init portB****
    ANSELB=ANSELB&0b11110000;
    TRISB=TRISB&0b11110000;
    LATB=0;
    //*** init port E***
    ANSELE=0;// port B en digital
    TRISE=TRISE|0b00000111;
}
unsigned char lecture_clav(void)
{ 
     const unsigned char clavier[]=
{
    '1','2','3',
    '4','5','6',
    '7','8','9',
    '*','0','#',
    0xFF
}; 
    unsigned char index=0;
    for(unsigned char ligne=1;ligne<16;ligne<<=1)
    {
        LATB=~ligne;
       __delay_ms(5);
        
        if(!col1)break;index++;
        if(!col2)break;index++;
        if(!col3)break;index++;      
    }

    return clavier[index];                                       //retourne le code ASCII de la touche appuyée. 
} 

void configuration()
{
    //***init oscillateur***
    OSCCON=0b01101010; // Oscillateur interne à 4MHz
    //***initialisation portA pour carte rouge***
    ANSELA=0; //port A en digital
    TRISA=0xF1; //RA1,2 et 3 en sortie
    LATAbits.LATA2=0; //0 sur R/!W pour carte rouge
    //***init portD carte rouge et verte***
    ANSELD=0; //PortD en digital
    //TRISD=0x00; // PortD en sortie pour carte verte
    TRISD=0xF0; // RD<3;0> en sortie pour carte rouge
    //***init LCD***
    __delay_ms(50);
    LCDinit(1); // Clignotement du curseur
   __delay_ms(2);  


  
    
    lecture_clav_init();
   
    
    
}

void main(void) 
{
    configuration();
    while(1)
    {
       Touche=lecture_clav();
       
       if(Touche!=Old_Touche)
       {
        
            LCDpos(0,i);
            if(Touche<0xFF)
            {
             LCDwrite(1,Touche);
             i++;
             if(i==21) { i=0; LCDclear();} 
            }
      //__delay_ms(500);   
       }
       Old_Touche=Touche;
    
    }
    
}
