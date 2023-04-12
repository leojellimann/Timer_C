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
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

#include <xc.h>

#define _XTAL_FREQ 4000000
#define digit LATA
#define clavhoriz LATB
#define BUZZER LATCbits.LATC2
#define RED_LED LATCbits.LATC0 

unsigned char Min_diz=0;//variable minute_dizaine assignée au digit 1 de l'afficheur
unsigned char Min_uni=0;//variable minute_unité assignée au digit 2 de l'afficheur
unsigned char Sec_diz=0;//variable seconde_dizaine assignée au digit 3 de l'afficheur
unsigned char Sec_uni=0;//variable minute_unité assignée au digit 1 de l'afficheur

unsigned char Touche=0;// variable qui prend la valeur retournée par la fonction clavier 
unsigned char Old_Touche=0xFF;
unsigned char saisie = 0;// variable du switch pour afficher les valeurs du clavier sur l'afficheur 
unsigned char var_TMR1=0;//Variable timer 1 pour compter 1 sec
unsigned char var_TMR4=0;//Variable timer 4 pour compter 1 sec
unsigned char active_clavier=1;



void afficheur(unsigned char digit_1, unsigned char digit_2, unsigned char digit_3, unsigned char digit_4);

void afficheur_init()
{
    //LT, RBI, BI toujours égale à 1
    //DP = 1 si on veut afficher un point 

    //*******description du cablage*********
    /*RD0=A     RD1=B       RD2=C       RD3=D       RD4=DP      RD5=BI/RB0      RD6=RBI     RD7=LT*/
    
    //La fonction afficheur(digit, LATD); doit se trouver dans
    //void interrupt minuteur()
    
    //*********Config digit 7seg**********
    
    //PORT A en sorti numérique
    ANSELA=0;//Mise en digital de la sélection des digits
    TRISA=0;//Mise en sortie des digits
    LATA=0;//Aucuns digits du port A ne sont sélectionnés
    
     //*********Config 7 seg 7447*********
    
    //PORT D en sorti numérique 
    ANSELD=0;// port D en numérique
    TRISD=0;// port D en sortie 
    LATD=0;// les sorties du port D sont à 0000 0000
    
     //**Config Timer 0 pour l'affichage alternatif des chiffres sur l'afficheur
    INTCON=0b11100000;
    OPTION_REG=0b00000111;//prescaler 256
    TMR0=246;//frequence 200Hz
}

void clavier_init()
{
    //***********Init des lignes horizontales du clavier**************
    TRISB=0;//Mise en sortie des pins 0 à 3 du port B
    ANSELB=0;//Mise en numérique des pins du port B pour le clavier
    
    
    //***********Init des lignes verticales du clavier****************
    TRISE=0x7;//Mise des pates RE0 à RE2 en entrées
    ANSELE=0;// Mise en numérique des pins du port E pour le clavier
 
    //********Config Timer 0 pour affichage alternatif des chiffres afficheur
    INTCON=0b11100000;
    OPTION_REG=0b00000111;//prescaler 256
    TMR0=246;//frequence 200Hz
}

void buzzer_init ()
{
    T2CON=0b00000110;//Configuration du timer 2 avec prescaler = 16, (pas de postscaler)
    PR2=70;//Précharge du timer2
    TRISCbits.TRISC2=0;  // Broche 2 du portC en sortie
    BUZZER=0;  //Broche 2 du portC à 0
    PIE1bits.TMR2IE=0; // autorisation des interruptions du Timer 2 
    PIR1bits.TMR2IF=0; //Mise à zero du drapeau de debordement du Timer1
}

void led_init()
{
    TRISCbits.TRISC0=0;//port C en sortie 
    RED_LED=0;
  
}

void decompteur_init()
{
    INTCON=0b11100000;//Prescaler à 8 on compte 500ms
    T1CON=0b00111101;
    TMR1L=0xDC;//Initialisation a 3036
    TMR1H=0x0B;//Initialisation a 3036
    PIE1bits.TMR1IE=0; // autorisation des interruptions du Timer 1
    PIR1bits.TMR1IF=0; //Mise à zero du drapeau de debordement du Timer1
}

void afficheur(unsigned char digit_1, unsigned char digit_2, unsigned char digit_3, unsigned char digit_4)
{
    static unsigned char etat=0;
      
        switch (etat)
        {
            case 0:
                digit=0x01;//selection du digit 1 (à gauche)
                LATD=0xF0+digit_1;//mise à 0 du digit + la valeur voulue du chiffre à afficher 
               
                etat=1;// envoie au case d'après
                
                break;
            
            case 1 :
                digit=0x02;//Digit 2
                LATD=0xE0+digit_2;//mise à 0. du digit + la valeur voulue du chiffre à afficher 
              
                etat=2;//envoie au case d'après
               
                break;
             
             case 2 :
                digit=0x04;//Digit 3
                LATD=0xF0+digit_3;//mise à 0 du digit + la valeur voulue du chiffre à afficher 
                
                etat=3;//envoie au case d'après
                
                break;
             
              case 3 :
                digit=0x08;//Digit 4
                LATD=0xF0+digit_4;//mise à 0 du digit + la valeur voulue du chiffre à afficher 
               
                etat=0;// envoie au case de départ 
                
                break;   
        }

   
    
}
unsigned char clavier ()
{
    static unsigned char clavierstate=0;
    unsigned char valtouche;

   
        switch (clavierstate)
            {
                case 0:
                     if (active_clavier==1)//Sert à bloquer les appuis sur le clavier si le minuteur est lancé
                        {
                                clavhoriz=0xFE;//scrutage pour la première ligne horizontale du clavier

                                
                                    if (PORTE==0xE)
                                        {
                                            valtouche=0x31;//Valeur de 1 en ascii
                                               break;
                                        }

                                    if (PORTE==0xD)
                                        {
                                            valtouche=0x32;//Valeur de 2 en ascii
                                               break;
                                        }

                                    if (PORTE==0xB)
                                        {
                                            valtouche=0x33;//Valeur de 3 en ascii
                                               break;
                                        }
                                
                                    if (PORTE==0xF)
                                        {
                                            valtouche=0xFF;//Valeur de erreur en ascii

                                        }                         
                                
                        }               

                    clavierstate=1;//Envoi à l'état suivant

                    break;

                case 1 :
                     if (active_clavier==1)
                        {
                             clavhoriz=0xFD;//scrutage pour la deuxième ligne horizontale du clavier

                               
                                    if (PORTE==0xE)
                                        {
                                            valtouche=0x34;//Valeur de 4 en ascii
                                               break;
                                        }

                                    if (PORTE==0xD)
                                        {
                                            valtouche=0x35;//Valeur de 5 en ascii
                                               break;
                                        }
                             
                                    if (PORTE==0xB)
                                        {
                                            valtouche=0x36;//Valeur de 6 en ascii
                                               break;
                                        }
                             
                                    if (PORTE==0xF)
                                        {
                                            valtouche=0xFF;//Valeur de erreur en ascii

                                        }
                                
                        }
                    clavierstate=2;

                    break;
                    
                 case 2 :
                     
                     if (active_clavier==1)
                        {
                           clavhoriz=0xFB;//scrutage pour la troisième ligne horizontale du clavier


                           if (PORTE==0xE)
                                {
                                    valtouche=0x37;//Valeur de 7 en ascii
                                       break;
                                }

                           if (PORTE==0xD)
                                {
                                    valtouche=0x38;//Valeur de 8 en ascii
                                       break;
                                }

                           if (PORTE==0xB)
                                {
                                    valtouche=0x39;//Valeur de 9 en ascii
                                    break;
                                }
                           
                           if (PORTE==0xF)
                                {
                                    valtouche=0xFF;//Valeur de erreur en ascii

                                }

                        }
                    clavierstate=3;

                    break;

                  case 3 :
                     clavhoriz=0xF7;//Digit 4

                     
                        if (PORTE==0xE)
                            {
                                   TMR1IE=0;//Valeur de * en ascii, arreter le decomptage
                                   break;
                            }
                     
                        if (active_clavier==1)
                            {
                                if (PORTE==0xD)
                                {
                                    valtouche=0x30;//Valeur de 0 en ascii
                                       break;
                                }
                            }
                        if (PORTE==0xB)
                                {
                                    TMR1IE=1;//Valeur de # en ascii, demarrer le decomptage

                                       break;
                                }
                         if (PORTE==0xF)
                                {
                                    valtouche=0xFF;//Valeur de erreur en ascii
                                }

                   clavierstate=0;

                   break;

        }
        return valtouche;
    }

//*************************Programme**************************
void init()
{
    OSCCON=0b01101010;    
    
  
    afficheur_init();
    clavier_init();
    decompteur_init();
    buzzer_init();
    led_init();
    

}

void main (void)
{
    init();
    
    while(1)
    {
//*********************Affichage des valeurs saisies au clavier sur l'afficheur******************
         Touche=clavier();// valeur renvoyée par la fonction clavier 
          
        if(Touche!=Old_Touche)
        {
            switch (saisie)
            {
                case 0:

                    if(Touche!=0xFF && Touche <= 0x35)//N'affiche pas si >5
                        
                    {
                        Min_diz=Touche-0x30;
                        saisie=1;
                    }

                break;
                case 1:

                    if(Touche!=0xFF)
                    {
                        Min_uni=Touche-0x30;
                        saisie=2;
                    }

                break;   
                
                case 2:

                    if(Touche!=0xFF && Touche <= 0x35)//N'affiche pas si >5
                    {
                        Sec_diz=Touche-0x30;
                        saisie=3;
                    }

                break;   
                case 3:

                    if(Touche!=0xFF )
                    {
                        Sec_uni=Touche-0x30;
                        saisie=0;
                    }

                break;   
            }
        }    
            Old_Touche=Touche;//dès le relachement de la touche pressée old_touche vaudra 0xFF
           
            
//************* Décrémentation des variables correspondantes aux secondes et minutes*************
            
            if(Sec_uni==0xFF && TMR1IE==1)
                {
                    Sec_diz--;
                    Sec_uni=9;
                
                if(Sec_diz==0xFF)
                    {
                        Min_uni--;
                        Sec_diz=5;
                        Sec_uni=9;

                    if(Min_uni==0xFF)
                        {
                            Min_diz--;
                            Min_uni=9;
                            Sec_diz=5;
                                Sec_uni=9;
                                
                                
                                
                                

                        if(Min_diz==0xFF)
                                { 
                                    TMR1IE=0;
                                    Min_diz=0;
                                    Min_uni=0;
                                    Sec_diz=0;
                                    Sec_uni=0;
                                    TMR2IE=1;
                                    RED_LED=!RED_LED;
                                   
                                }
                                
                        }   
                    }
                }    
        }

}

    

void interrupt minuteur()
{
 
    if (TMR0IF==1)// quand flag du timer 0 est a 1 
        {
         //Appelle de la fonction qui permet de faire fonctionner l'afficheur
        afficheur(Min_diz,Min_uni,Sec_diz,Sec_uni);
        
        TMR0IF=0;
        TMR0=246;//Réinitialisation du timer pour le comptage à 200Hz
        }
    
    
    //**************permet de ne plus pouvoir saisir de valeur quand le décomptage est lancé******
    
    if (TMR1IE==1)
        {
            active_clavier=0;
        }
    
    
    
    //**********************Programme décompteur interruption***************
    
     
    if(TMR1IF==1 && TMR1IE==1)//Si le timer 1 est en interruption et que le bouton de lancement a été appuyé
        { 
            var_TMR1++;

            if (var_TMR1==2)//Comptage de 2 interruptions (2x500ms) pour avoir 1s
                {    
                Sec_uni--;

                var_TMR1=0;
                }  

            TMR1IF=0;
            TMR1L=0xDC;//Initialisation a 3036
            TMR1H=0x0B;//Initialisation a 3036
        }
 
    
    //*********** Allumer le buzzer à la fin du temps défini ************
    
    if(TMR2IF==1 && TMR2IE==1)// timer 2 en interruption et autorisation des flag
        {  
       
            BUZZER=!BUZZER;// invrertion de la valeur de BUZZER 
            PR2=70;// repréchargment du TMR2
            TMR2IF=0;// mise a 0 du flag
        }  
     
    
    
}

