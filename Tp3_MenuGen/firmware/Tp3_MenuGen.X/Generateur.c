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

static uint16_t tablEch[100] = {0};
// Initialisation du  générateur

void GENSIG_Initialize(S_ParamGen *pParam) {
    static S_ParamGen ValInit;
    NVM_ReadBlock((uint32_t*) & ValInit, sizeof (S_ParamGen));

    //test de la sauvegarde
    if (ValInit.Magic != MAGIC) {
        //assignation des valeurs
        pParam->Forme = SignalSinus;
        pParam->Frequence = 1000;
        pParam->Amplitude = 5000;
        pParam->Offset = 0;
        pParam->Magic = MAGIC;
    } else {
        *pParam = ValInit;

    }
}


// Mise à jour de la periode d'échantillonage

void GENSIG_UpdatePeriode(S_ParamGen *pParam) {
    //modifie la valeur de periode du registe timer3 
    PLIB_TMR_Period16BitSet(TMR_ID_3, (uint16_t) ((uint64_t) (FCLK / pParam->Frequence)));
}

// Mise à jour du signal (forme, amplitude, offset)

void GENSIG_UpdateSignal(S_ParamGen *pParam) {
    //variable pour test > 65535(ValMaxADC)
    static uint32_t val;

    uint8_t i = 0;
    float step = (((pParam->Amplitude) / MAX_ECH) * COEF);
    switch (pParam->Forme) {
            // Calcul des echantillons pour le sinus 
        case SignalSinus:
            for (i = 0; i < MAX_ECH; i++) {
                val = (MIDPOINT - ((((pParam->Amplitude / 2) * sin(2 * M_PI * (i / (float) MAX_ECH)) + pParam->Offset) * COEF)));
                // test pour écretage
                if (val >= ADC_MAX) {
                    val = (MINAMPL + 1);
                }
                if (val <= MINAMPL) {
                    val = (ADC_MAX - 1);
                }
                tablEch[i] = (uint16_t) val;
            }
            break;
            // Calcul des echantillons pour le signal triangle        
        case SignalTriangle:
            //step= (((pParam->Amplitude*2)/MAX_ECH/4) *COEF);
            for (i = 0; i <= MAX_ECH; i++) {
                if (i < MAX_ECH / 2) {
                    if (i < (MAX_ECH / 4)) {
                        val = (uint32_t) ((MIDPOINT - (((((pParam->Amplitude / 2 * COEF)) / (MAX_ECH / 4)) * i) - (MIDPOINT / 4))));

                    } else {
                        val = (uint32_t) ((MIDPOINT - (((((pParam->Amplitude / 2 * COEF)) / (MAX_ECH / 4)) * i) -(MIDPOINT / 4))));

                    }


                } else {
                    if (i < (3 * (MAX_ECH / 4))) {
                        val = (uint32_t) ((MIDPOINT - (((((pParam->Amplitude / 2 * COEF)) / (MAX_ECH / 4)) * (MAX_ECH - i))) + (MIDPOINT / 4)));
                    } else {
                        val = (uint32_t) ((MIDPOINT - (((((pParam->Amplitude / 2 * COEF)) / (MAX_ECH / 4)) * (MAX_ECH - i)) - (MIDPOINT / 4))));
                    }

                }
                // appliquer l'offset au résultat 
                val -= (pParam->Offset * COEF);

                // Test pour écretage
                if (val >= ADC_MAX) {
                    val = (MINAMPL + 1);
                }
                if (val <= MINAMPL) {
                    val = (ADC_MAX - 1);
                }

                tablEch[i] = (uint16_t) val;
            }

            break;
        case SignalCarre:
            for (i = 0; i < (MAX_ECH / 2); i++) {
                val = (MIDPOINT + ((((pParam->Amplitude * COEF) / 2))-(pParam->Offset * COEF)));
                // test pour écretage
                if (val >= ADC_MAX) {
                    val = (MINAMPL + 1);
                }
                if (val <= MINAMPL) {
                    val = (ADC_MAX - 1);
                }

                tablEch[i] = (uint16_t) val;
            }
            for (i = (MAX_ECH / 2); i < MAX_ECH; i++) {
                val = (MIDPOINT - ((((pParam->Amplitude * COEF) / 2))+(pParam->Offset * COEF)));
                // test pour écretage
                if (val >= ADC_MAX) {
                    val = (MINAMPL + 1);
                }
                if (val <= MINAMPL) {
                    val = (ADC_MAX - 1);
                }

                tablEch[i] = (uint16_t) val;
            }
            break;
        case SignalDentDeScie:
            for (i = 0; i < (MAX_ECH); i++) {
                val = (MIDPOINT - (float) (step * i)-(pParam->Offset * COEF)+(MAX_ECH * step) / 2);
                // test pour écretage
                if (val >= ADC_MAX) {
                    val = (MINAMPL + 1);
                }
                if (val <= MINAMPL) {
                    val = (ADC_MAX - 1);
                }
                tablEch[i] = (uint16_t) val;
            }
            break;
    }
}
// Fonction appelée dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC à modifier

void GENSIG_Execute(void) {
    static uint16_t EchNb = 0;


    SPI_WriteToDac(0, tablEch[EchNb]); // sur canal 0
    //    SPI_WriteToDac(0, Step * EchNb );      // sur canal 0
    EchNb++;
    EchNb = EchNb % MAX_ECH;
}
