// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Fichier MenuGen.c
// Gestion du menu  du générateur
// Traitement cyclique à 10 ms



#include <stdint.h>                   
#include <stdbool.h>
#include "app.h"
#include "MenuGen.h"
#include "DefMenuGen.h"
#include "GesPec12.h"
#include "Mc32DriverLcd.h"
#include "Generateur.h"
#include "Mc32NVMUtil.h"

static S_Menu s_menu;
static const char FormeSignaux[4][20] = {"Sinus", "Triangle", "DentDeScie", "Carre"};
// Initialisation du menu et des paramètres
void MENU_Initialize(S_ParamGen *pParam)
{
  pParam->Forme =   SignalCarre;
  pParam->Frequence =  _20Hz ;
  pParam->Amplitude =  _5V ;
  pParam->Offset=0;
  s_menu.menu=FormeSel;
  s_menu.reglageForme= SignalSinus;
  s_menu.reglageFreq =_20Hz; 
  s_menu.reglageAmpl =_5V; 
  s_menu.reglageOffset =0; 
}
  
// Execution du menu, appel cyclique depuis l'application
void MENU_Execute(S_ParamGen *pParam)
{
    UpdateScreen();
    GENSIG_UpdateSignal(pParam);
    static uint8_t pushcnt=0;
    switch(s_menu.menu)
    {
        case AmplSel:
            if(Pec12IsPlus())
            {
                Pec12ClearPlus();
                
                s_menu.menu=OffsetSel;
            }
             if(Pec12IsMinus())
            {
                Pec12ClearMinus();
                s_menu.menu=FreqSel;
            }
            Pec12SelSave();
            break;
        case FormeSel:
            if(Pec12IsPlus())
            {
                Pec12ClearPlus();
                
                s_menu.menu=FreqSel;
            }
             if(Pec12IsMinus())
            {
                Pec12ClearMinus();
                s_menu.menu=OffsetSel;
            }
            Pec12SelSave();
            break;
        case FreqSel:
            if(Pec12IsPlus())
            {
                Pec12ClearPlus();
                
                s_menu.menu=AmplSel;
            }
             if(Pec12IsMinus())
            {
                Pec12ClearMinus();
                s_menu.menu=FormeSel;
            }
            Pec12SelSave();
            break;
        case OffsetSel:
            if(Pec12IsPlus())
            {
                Pec12ClearPlus();
                
                s_menu.menu=FormeSel;
            }
             if(Pec12IsMinus())
            {
                Pec12ClearMinus();
                s_menu.menu=AmplSel;
            }
            Pec12SelSave();
            break;
           
        case FormeSet:
            if (Pec12IsPlus()) {
                Pec12ClearPlus();
                if(s_menu.reglageForme >= 3)
                {
                    s_menu.reglageForme=0;    
                }
                else
                {
                    s_menu.reglageForme++;
                }

            }
            if (Pec12IsMinus()) {
                Pec12ClearMinus();
                if(s_menu.reglageForme <= 0)
                {
                    s_menu.reglageForme=3;    
                }
                else
                {
                    s_menu.reglageForme--;
                }
            }
            if (Pec12IsOK()) {
                Pec12ClearOK();
                pParam->Forme = s_menu.reglageForme;
                s_menu.menu = FormeSel;
                GENSIG_UpdateSignal(pParam);
                
            }
            if (Pec12IsESC()) {
                Pec12ClearESC();  
                s_menu.menu = FormeSel;
                s_menu.reglageForme = pParam->Forme;
            }
            break;
            
            
            
  
        case AmplSet:
          
             if (Pec12IsPlus()) {
                Pec12ClearPlus();
                s_menu.reglageAmpl +=STEPAMPL;
                if(s_menu.reglageAmpl>=LIMAMPL)
                {
                    s_menu.reglageAmpl = MINAMPL;
                }
                else
                {
                    s_menu.reglageAmpl +=STEPAMPL;
                }                          
            }
             
            if (Pec12IsMinus()) {
                Pec12ClearMinus();
                if(s_menu.reglageAmpl<=MINAMPL)
                {
                    s_menu.reglageAmpl = LIMAMPL;
                }
                else
                {
                    s_menu.reglageAmpl -=STEPAMPL;
                }    
            }
             
            if (Pec12IsOK()) {
                Pec12ClearOK();
                pParam->Amplitude = s_menu.reglageAmpl;
                s_menu.menu = AmplSel;
                GENSIG_UpdateSignal(pParam);
            }
            if (Pec12IsESC()) {
                Pec12ClearESC();
                
                s_menu.menu = AmplSel;
                s_menu.reglageAmpl = pParam->Amplitude;
            }
            
            break;
            
            
            
   
        case FreqSet: 
            
             if (Pec12IsPlus()) {
                Pec12ClearPlus();
                if(s_menu.reglageFreq>=LIMFREQ)
                {
                    s_menu.reglageFreq = MINFREQ;
                }
                else
                {
                    s_menu.reglageFreq +=STEFREQ;
                }      
            }
            if (Pec12IsMinus()) {
                Pec12ClearMinus();
                
                if(s_menu.reglageFreq<=MINFREQ)
                {
                    s_menu.reglageFreq = LIMFREQ;
                }
                else
                {
                    s_menu.reglageFreq -=STEFREQ;
                }                 
            }
 
            if (Pec12IsOK()) {
                Pec12ClearOK();
                pParam->Frequence = s_menu.reglageFreq;
                s_menu.menu = FreqSel;
                GENSIG_UpdateSignal(pParam);
            }
            if (Pec12IsESC()) {
                Pec12ClearESC();
                s_menu.menu = FreqSel;
                s_menu.reglageFreq= pParam->Frequence;
            }
            
            break;
        case Offset7: 
              if (Pec12IsPlus()) {
                Pec12ClearPlus();
                if(s_menu.reglageOffset>=LIMOFFSET)
                {
                    s_menu.reglageOffset = LIMOFFSET;
                }
                else
                {
                    s_menu.reglageOffset +=STEPOFFSET;
                }  
               
            }
            if (Pec12IsMinus()) {
                Pec12ClearMinus();
                
                if(s_menu.reglageOffset<=MINOFF)
                {
                    s_menu.reglageOffset = MINOFF;
                }
                else
                {
                    s_menu.reglageOffset -=STEPOFFSET;
                }    
                
            }
            if (Pec12IsOK()) {
                Pec12ClearOK();
                pParam->Offset= s_menu.reglageOffset;
                s_menu.menu = OffsetSel;
                GENSIG_UpdateSignal(pParam);
                
            }
            if (Pec12IsESC()) {
                Pec12ClearESC();
                s_menu.menu = OffsetSel;
                s_menu.reglageOffset = pParam->Offset;
            }
            break;
        case Save: 
            if (S9IsESC())
            {
                pushcnt ++;  
            }
            else
            {
                if (pushcnt > 0)
                {
                    if (pushcnt >= 200)
                    {
                        // Sauvegarde dans la flash
                        NVM_WriteBlock((uint32_t*)pParam, sizeof(S_ParamGen));
                        //NVM_WriteBlock((uint32_t*) pParam, sizeof(S_ParamGen));
                        pushcnt = 0;
                        lcd_gotoxy(1, 4);
                        printf_lcd("Data sauvegardee");
                         
                    }
                    else
                    {
                        // Retour au menu forme
                        s_menu.menu = FormeSel;
                        pushcnt = 0;
                    }
                }
            }
            
            
            break;
        default:
            s_menu.menu = FormeSel;
            break;

    }
    switch(s_menu.menu)
    {
        case AmplSel:   
        case FormeSel:  
        case FreqSel:   
        case OffsetSel:
            if (PLIB_PORTS_PinGet(S_OK, PORT_CHANNEL_G, PORTS_BIT_POS_12) == 0)
            {
                s_menu.menu = Save;  
                
            } 
            break;
        }
    //GENSIG_UpdateSignal(pParam);
    
}

void Pec12SelSave(void)
{
    if (Pec12IsOK())
    {
        Pec12ClearOK();
        switch(s_menu.menu)
        {
            case AmplSel:
                s_menu.menu = AmplSet;
                break;
            case FormeSel:
                s_menu.menu = FormeSet;
                break;
            case FreqSel:
                s_menu.menu = FreqSet;
                break;
            case OffsetSel:
                s_menu.menu = Offset7;
                break;
            
        }
    }
            

}
void UpdateScreen(void)
{
    //clear lcd
    lcd_putc('\f');

    switch(s_menu.menu)
    {
        case AmplSel:
            lcd_gotoxy(C1,L1);
            printf_lcd(" Forme = %s",FormeSignaux[s_menu.reglageForme]); //affiche sur le LCD
            lcd_gotoxy(C1,L2);
            printf_lcd(" Freq = %d",s_menu.reglageFreq); //affiche sur le LCD
            lcd_gotoxy(C1,L3);
            printf_lcd("*Ampl = %d",s_menu.reglageAmpl); //affiche sur le LCD
            lcd_gotoxy(C1,L4);
            printf_lcd(" Offset = %d",s_menu.reglageOffset); //affiche sur le LCD
            
            break;
            
        case AmplSet:
            lcd_gotoxy(C1,L1);
            printf_lcd(" Forme = %s",FormeSignaux[s_menu.reglageForme]); //affiche sur le LCD
            lcd_gotoxy(C1,L2);
            printf_lcd(" Freq = %d",s_menu.reglageFreq); //affiche sur le LCD
            lcd_gotoxy(C1,L3);
            printf_lcd("?Ampl = %d",s_menu.reglageAmpl); //affiche sur le LCD
            lcd_gotoxy(C1,L4);
            printf_lcd(" Offset = %d",s_menu.reglageOffset); //affiche sur le LCD
            
            break;
      
           
        case FormeSel:
            lcd_gotoxy(C1,L1);
            printf_lcd("*Forme = %s",FormeSignaux[s_menu.reglageForme]); //affiche sur le LCD
            lcd_gotoxy(C1,L2);
            printf_lcd(" Freq = %d",s_menu.reglageFreq); //affiche sur le LCD
            lcd_gotoxy(C1,L3);
            printf_lcd(" Ampl = %d",s_menu.reglageAmpl); //affiche sur le LCD
            lcd_gotoxy(C1,L4);
            printf_lcd(" Offset = %d",s_menu.reglageOffset); //affiche sur le LCD
            break;
        case FormeSet:
            lcd_gotoxy(C1,L1);
            printf_lcd("?Forme = %s",FormeSignaux[s_menu.reglageForme]); //affiche sur le LCD
            lcd_gotoxy(C1,L2);
            printf_lcd(" Freq = %d",s_menu.reglageFreq); //affiche sur le LCD
            lcd_gotoxy(C1,L3);
            printf_lcd(" Ampl = %d",s_menu.reglageAmpl); //affiche sur le LCD
            lcd_gotoxy(C1,L4);
            printf_lcd(" Offset = %d",s_menu.reglageOffset); //affiche sur le LCD
            break;
            
        case FreqSel:
            lcd_gotoxy(C1,L1);
            printf_lcd(" Forme = %s",FormeSignaux[s_menu.reglageForme]); //affiche sur le LCD
            lcd_gotoxy(C1,L2);
            printf_lcd("*Freq = %d",s_menu.reglageFreq); //affiche sur le LCD
            lcd_gotoxy(C1,L3);
            printf_lcd(" Ampl = %d",s_menu.reglageAmpl); //affiche sur le LCD
            lcd_gotoxy(C1,L4);
            printf_lcd(" Offset = %d",s_menu.reglageOffset); //affiche sur le LCD
            break;
            
        case FreqSet:
            lcd_gotoxy(C1,L1);
            printf_lcd(" Forme = %s",FormeSignaux[s_menu.reglageForme]); //affiche sur le LCD
            lcd_gotoxy(C1,L2);
            printf_lcd("?Freq = %d",s_menu.reglageFreq); //affiche sur le LCD
            lcd_gotoxy(C1,L3);
            printf_lcd(" Ampl = %d",s_menu.reglageAmpl); //affiche sur le LCD
            lcd_gotoxy(C1,L4);
            printf_lcd(" Offset = %d",s_menu.reglageOffset); //affiche sur le LCD
            break;
            
        case OffsetSel:
            lcd_gotoxy(C1,L1);
            printf_lcd(" Forme = %s",FormeSignaux[s_menu.reglageForme]); //affiche sur le LCD
            lcd_gotoxy(C1,L2);
            printf_lcd(" Freq = %d",s_menu.reglageFreq); //affiche sur le LCD
            lcd_gotoxy(C1,L3);
            printf_lcd(" Ampl = %d",s_menu.reglageAmpl); //affiche sur le LCD
            lcd_gotoxy(C1,L4);
            printf_lcd("*Offset = %d",s_menu.reglageOffset); //affiche sur le LCD
            break;
        case Offset7:
            lcd_gotoxy(C1,L1);
            printf_lcd(" Forme = %s",FormeSignaux[s_menu.reglageForme]); //affiche sur le LCD
            lcd_gotoxy(C1,2);
            printf_lcd(" Freq = %d",s_menu.reglageFreq); //affiche sur le LCD
            lcd_gotoxy(C1,L3);
            printf_lcd(" Ampl = %d",s_menu.reglageAmpl); //affiche sur le LCD
            lcd_gotoxy(C1,L4);
            printf_lcd("?Offset = %d",s_menu.reglageOffset); //affiche sur le LCD
            break;
        case Save:
            lcd_putc('\f');
            lcd_gotoxy(1, 2);
            printf_lcd("   Sauvegarde  ?");
            lcd_gotoxy(1, 3);
            printf_lcd("   (appuis long)");
            
            break;
   
  
    }
}



