// Ecole supérieure SL229_MINF TP
// Manipulation Tp3_MenuGen
// Créé le 9 Mai 2006 CHR
// Version 2016  du 03.02.2016
// Modif 2015 : utilisation de stdint.h
// Modif 2016 : ajout Duty pour PWM
// Modif 2018 SCA : suppression PWM et duty
// Definition pour le menuGen

#ifndef DEFMENUGEN_H
#define DEFMENUGEN_H

#include <stdint.h>
#include "app.h"

#define MAGIC 0x123455AA
#define STEPOFFSET 100
#define STEPAMPL 100
#define STEFREQ 20

#define LIMOFFSET 5000
#define LIMAMPL 10000
#define LIMFREQ 2000
#define _5V 5000
#define _20Hz 20
#define L1 1
#define L2 2
#define L3 3
#define L4 4
#define C1 1

typedef enum  { SignalSinus, SignalTriangle, SignalDentDeScie, SignalCarre } E_FormesSignal;


typedef enum  { AmplSel,AmplSet, FormeSel,FormeSet,FreqSel, FreqSet,OffsetSel,Offset7,Save } E_Menu;


// Structure des paramètres du générateur
typedef struct {
      E_FormesSignal Forme;
      int16_t Frequence;
      int16_t Amplitude;
      int16_t Offset;
      uint32_t Magic;
} S_ParamGen;

typedef struct{
    int8_t menu;
    int16_t reglageForme;
    int16_t reglageFreq;
    int16_t reglageOffset;
    int16_t reglageAmpl;
    
}S_Menu;
void UpdateScreen(S_ParamGen *pParam);
void Pec12SelSave(void);
#endif
