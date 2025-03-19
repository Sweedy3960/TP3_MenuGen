// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion  du générateur

// Prévu pour signal de 40 echantillons

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
S_ParamGen ValInit;
#define FCLK 800000
static uint16_t tablEch[100] = {0};
// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{
    NVM_ReadBlock((uint32_t*)&ValInit, sizeof(S_ParamGen));
    
    //test de la sauvegarde
    if (pParam->Magic != 0x12345678)
    {
        pParam->Forme = SignalSinus;
        pParam->Frequence = 100;
        pParam->Amplitude= 10000;
        pParam->Offset = 5000;
        pParam->Magic =MAGIC;
    }
    else
    {
        *pParam = ValInit;
    }
}
  

// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    //modifie la valeur de periode du registe timer3 
    PLIB_TMR_Period16BitSet(TMR_ID_3, ((uint16_t)((FCLK/pParam->Frequence))));  
}

// Mise à jour du signal (forme, amplitude, offset)
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
                  val = (MIDPOINT -((((pParam->Amplitude/2) * sin(2 * M_PI * (i / (float)MAX_ECH)) + pParam->Offset)*COEF)));
                  
                  
                      if (val >= ADC_MAX )
                      {
                          val = (MINAMPL+1);
                      }
                      if (val <= MINAMPL )
                      {
                          val = (ADC_MAX-1);
                      }
                  
                 tablEch[i] =(uint16_t)val;
                  
                  

                 
            }  
            break;
       
        case SignalTriangle:

            //step= (((pParam->Amplitude*2)/MAX_ECH/4) *COEF);

            for (i = 0; i <= MAX_ECH; i++) 
            {

                if (i < MAX_ECH / 2) 
               {
                    if(i<(MAX_ECH/4))
                    {
                        val = (uint32_t) ((MIDPOINT + (pParam->Offset*COEF))-((((((pParam->Amplitude/2 *COEF) -1)))/(MAX_ECH/4))*i)-MIDPOINT/4);
                        if (val >= ADC_MAX )
                      {
                          val = (MINAMPL+1);
                      }
                      if (val <= MINAMPL )
                      {
                          val = (ADC_MAX-1);
                      }
                  
                     tablEch[i] =(uint16_t)val;
                        
                        //tablEch[i] =
                  //tablEch[i] = MIDPOINT + (pParam->Offset * COEF) + (step * i)-MIDPOINT/4;
                    }
                    else
                    {
                        tablEch[i] = (((MIDPOINT + pParam->Offset*COEF))-((((((pParam->Amplitude/2 *COEF)-1)))/(MAX_ECH/4))*i)-MIDPOINT/4);
                    }
               }
                else 
                {
                   if (i<(3*(MAX_ECH/4)))
                   {
                        tablEch[i] = (((MIDPOINT+ pParam->Offset*COEF) )-((((((pParam->Amplitude/2 *COEF))-1)/(MAX_ECH/4))*(MAX_ECH-i)))-MIDPOINT/4);
                   }
                   else
                    {
                       tablEch[i] = (((MIDPOINT+ pParam->Offset*COEF) )-((((((pParam->Amplitude/2 *COEF))-1)/(MAX_ECH/4))*(MAX_ECH-i))+MIDPOINT/4));
                    }
                }
            }
            break;
        case SignalCarre:
            for (i=0;i<(MAX_ECH/2);i++)
            {
                val =(MIDPOINT+((((pParam->Amplitude *COEF)/2))-(pParam->Offset *COEF)));
                if (val >= ADC_MAX )
                      {
                          val = (MINAMPL+1);
                      }
                      if (val <= MINAMPL )
                      {
                          val = (ADC_MAX-1);
                      }
                  
                 tablEch[i] =(uint16_t)val;
                  
            }
            for (i=(MAX_ECH/2);i<MAX_ECH;i++)
            {
                val = (MIDPOINT-((((pParam->Amplitude *COEF)/2))+(pParam->Offset *COEF)));
                if (val >= ADC_MAX )
                      {
                          val = (MINAMPL+1);
                      }
                      if (val <= MINAMPL )
                      {
                          val = (ADC_MAX-1);
                      }
                  
                tablEch[i] =(uint16_t)val;
            }
            break;
            
        case SignalDentDeScie:
            for (i = 0; i < (MAX_ECH); i++)
            {
                val =(MIDPOINT-(float)(step * i)-(pParam->Offset*COEF)+(MAX_ECH * step )/ 2);
                 if (val >= ADC_MAX )
                      {
                          val = (MINAMPL+1);
                      }
                      if (val <= MINAMPL )
                      {
                          val = (ADC_MAX-1);
                      }
                  
                tablEch[i] =(uint16_t)val;
                
            }
            break;
         
}
     for (i = 0; i < MAX_ECH; i++) 
     {
        val = (uint32_t)tablEch[i];
        
            
     }
}
// Fonction appelée dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC à modifier
void  GENSIG_Execute(void)
{
    static uint16_t EchNb = 0;

    //pour dent de scie
    SPI_WriteToDac(0,tablEch[EchNb]);      // sur canal 0
//    SPI_WriteToDac(0, Step * EchNb );      // sur canal 0
    EchNb++;
    EchNb = EchNb % MAX_ECH;
}
