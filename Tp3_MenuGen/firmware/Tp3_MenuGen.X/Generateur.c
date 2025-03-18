// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion  du g�n�rateur

// Pr�vu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber


#include <proc/p32mx795f512l.h>
#include "Mc32NVMUtil.h"
#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "driver/tmr/drv_tmr.h"
#include "math.h"
#include "stdint.h"

// T.P. 2016 100 echantillons
#define MAX_ECH 100
#define VALUE 3/65536 
#define FCLK 80000
static uint16_t tablEch[100] = {0};

// Initialisation du  g�n�rateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{
    NVM_ReadBlock((uint32_t*)pParam, sizeof(S_ParamGen));
    
    //test de la sauvegarde
    if (pParam->Magic != 0x12345678)
    {
        pParam->Forme = SignalTriangle;
        pParam->Frequence = 100;
        pParam->Amplitude= 5000;
        pParam->Offset = 0;
        pParam->Magic =MAGIC;
    }
}
  

// Mise � jour de la periode d'�chantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    //modifie la valeur de periode du registe timer3 
    PLIB_TMR_Period16BitSet(TMR_ID_3, (double)((FCLK/pParam->Frequence)+0.5));  
}

// Mise � jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam)
{
   
    static uint32_t val;
    uint8_t i=0;
    float step= (((pParam->Amplitude)/MAX_ECH) *COEF);
    switch(pParam->Forme)
{
        case SignalSinus:
            for (i = 0; i < MAX_ECH; i++) 
            {
                tablEch[i] = (MIDPOINT -(((pParam->Amplitude/2) * sin(2 * M_PI * (i / (float)MAX_ECH)) + pParam->Offset)*COEF));
            }
            break;
        case SignalTriangle:
            step= (((pParam->Amplitude)/MAX_ECH) *COEF);

            for (i = 0; i <= MAX_ECH; i++) 
            {

                if (i < MAX_ECH / 2) 
                {

                    tablEch[i] = (MIDPOINT - ((pParam->Offset*COEF) + (step * i)));
                    

                } else 
                {

                    tablEch[i] = (MIDPOINT - ((pParam->Offset * COEF) + (step * (MAX_ECH/2 - i))));


                }
            }
            break;
        case SignalCarre:
            for (i=0;i<(MAX_ECH/2);i++)
            {
                tablEch[i] =(MIDPOINT+((((pParam->Amplitude *COEF)/2))-(pParam->Offset *COEF)));
            }
            for (i=(MAX_ECH/2);i<MAX_ECH;i++)
            {
                tablEch[i] = (MIDPOINT-((((pParam->Amplitude *COEF)/2))+(pParam->Offset *COEF)));
            }
            break;
            
        case SignalDentDeScie:
            for (i = 0; i < (MAX_ECH); i++)
            {
                tablEch[i] =(MIDPOINT-(float)(step * i)-(pParam->Offset*COEF)+(MAX_ECH * step )/ 2);
            }
            break;
    }     
}
// Fonction appel�e dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC � modifier
void  GENSIG_Execute(void)
{
    static uint16_t EchNb = 0;

    //pour dent de scie
    SPI_WriteToDac(0,tablEch[EchNb]);      // sur canal 0
//    SPI_WriteToDac(0, Step * EchNb );      // sur canal 0
    EchNb++;
    EchNb = EchNb % MAX_ECH;
}
