// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion  du générateur

// Prévu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber


#include <proc/p32mx795f512l.h>

#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "driver/tmr/drv_tmr.h"
#include "math.h"
// T.P. 2016 100 echantillons
#define MAX_ECH 100
#define VALUE 3/65536 

char tablEch[100] = {0};

// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{
   
    pParam->Forme = SignalCarre;
    pParam->Frequence = 1000;
    pParam->Amplitude= 500;
    pParam->Offset = 0;
    pParam->Magic =MAGIC;

}
  

// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    //modifie la valeur de periode du registe timer3 
    PLIB_TMR_Period16BitSet(TMR_ID_3, (double)(80000*pParam->Frequence));  
}

// Mise à jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam)
{
    const uint16_t Step = 65535 / MAX_ECH;
    static char i=0;
    static SaveSinVal=0;
    switch(pParam->Forme)
    {
    case SignalSinus:
        for(i=0;i<MAX_ECH;i++)
        {
            //Calcul d chaque échantillon test des valeurs limites à faire
            //tablEch[i] = sin( i * 2 *M_PI )*VALUE;
        }
        break;
    case SignalTriangle:
        
        break;
    case SignalDentDeScie:
        break;
    case SignalCarre :
        break;
    }
   //recalcul de echantillions
    
    
   
}


// Execution du générateur
// Fonction appelée dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC à modifier
void  GENSIG_Execute(void)
{
   static uint16_t EchNb = 0;
   const uint16_t Step = 65535 / MAX_ECH;
  
   //pour dent de scie
   SPI_WriteToDac(0, Step * EchNb );      // sur canal 0
   EchNb++;
   EchNb = EchNb % MAX_ECH;
}
