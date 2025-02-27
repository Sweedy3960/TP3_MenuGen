// GesPec12.c  Canevas pour réalisation  
// C. HUBER    09/02/2015

// Fonctions pour la gestion du Pec12
//
//
// Principe : Il est nécessaire d'appeler cycliquement la fonction ScanPec12
//            avec un cycle de 1 ms
//
//  Pour la gestion du Pec12, il y a 9 fonctions à disposition :
//       Pec12IsPlus       true indique un nouveau incrément
//       Pec12IsMinus      true indique un nouveau décrément
//       Pec12IsOK         true indique action OK
//       Pec12IsESC        true indique action ESC
//       Pec12NoActivity   true indique abscence d'activité sur PEC12
//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'incrément
//       Pec12ClearMinus   annule indication de décrément
//       Pec12ClearOK      annule indication action OK
//       Pec12ClearESC     annule indication action ESC
//
//
//---------------------------------------------------------------------------


// définitions des types qui seront utilisés dans cette application

#include "GesPec12.h"
#include "Mc32Debounce.h"
#include "Mc32DriverLcd.h"

// Descripteur des sinaux
S_SwitchDescriptor DescrA;
S_SwitchDescriptor DescrB;
S_SwitchDescriptor DescrPB;

// Structure pour les traitement du Pec12
S_Pec12_Descriptor Pec12;


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Principe utilisation des fonctions
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//
// ScanPec12 (bool ValA, bool ValB, bool ValPB)
//              Routine effectuant la gestion du Pec12
//              recoit la valeur des signaux et du boutons
//
// s'appuie sur le descripteur global.
// Après l'appel le descripteur est mis à jour

// Comportement du PEC12
// =====================

// Attention 1 cran génère une pulse complète (les 4 combinaisons)
// D'ou traitement uniquement au flanc descendand de B

// Dans le sens horaire CW:
//     __________                      ________________
// B:            |____________________|
//     ___________________                       _________
// A:                     |_____________________|                    

// Dans le sens anti-horaire CCW:
//     ____________________                      _________
// B:                      |____________________|
//     __________                       __________________
// A:            |_____________________|        

void ScanPec12(bool ValA, bool ValB, bool ValPB)
{
    // Traitement antirebond sur A, B et PB
    DoDebounce(&DescrA, ValA);
    DoDebounce(&DescrB, ValB);
    DoDebounce(&DescrPB, ValPB);

    // Détection incrément / décrément
    if ((DescrB.bits.KeyPrevInputValue == 1) && (DescrB.bits.KeyValue == 0))
    {
      
        if (DescrA.bits.KeyValue == 1)
        {
            
            Pec12ClearPlus();
            Pec12.Dec = 1;
        }
        else
        {
            
            Pec12ClearMinus();
            Pec12.Inc = 1;
        }
        Pec12ClearInactivity();
       
    }
    else
    {
         // Traitement du PushButton
    if (DescrPB.bits.KeyValue == 0)
    {
        Pec12.PressDuration++;
        Pec12.NoActivity = 0;
        Pec12.InactivityDuration = 0;
    }
    else
    {

        if ((DescrPB.bits.KeyPrevInputValue == 0) && (DescrPB.bits.KeyValue == 1))
        {
            Pec12ClearInactivity();
            if (Pec12.PressDuration < 500)
            {
                Pec12.OK = 1;
                Pec12ClearESC();
                
            }
            else
            {
                Pec12ClearOK();
                Pec12.ESC = 1;
            }
        }
        else
        {
            
                
            Pec12.NoActivity = 1;
        }

    }

        
    }


   
    // Gestion inactivité
    if(Pec12NoActivity()!=0)
    {
        Pec12.InactivityDuration ++;
        if(Pec12.InactivityDuration >= 500)
        {
            Pec12.InactivityDuration=500;
            lcd_bl_off();
        }
    }
    else
    {
         lcd_bl_on();
    }

} // ScanPec12

void Pec12Init(void)
{
    // Initialisation des descripteurs de touches Pec12
    DebounceInit(&DescrA);
    DebounceInit(&DescrB);
    DebounceInit(&DescrPB);

    // Init de la structure PEc12
    Pec12.Inc = 0; // événement incrément  
    Pec12.Dec = 0; // événement décrément 
    Pec12.OK = 0; // événement action OK
    Pec12.ESC = 0; // événement action ESC
    Pec12.NoActivity = 0; // Indication d'activité
    Pec12.PressDuration = 0; // Pour durée pression du P.B.
    Pec12.InactivityDuration = 0; // Durée inactivité

} // Pec12Init





//       Pec12IsPlus       true indique un nouveau incrément

bool Pec12IsPlus(void)
{
    return (Pec12.Inc);
}

//       Pec12IsMinus      true indique un nouveau décrément

bool Pec12IsMinus(void)
{
    return (Pec12.Dec);
}

//       Pec12IsOK         true indique action OK

bool Pec12IsOK(void)
{
    return (Pec12.OK);
}

//       Pec12IsESC        true indique action ESC

bool Pec12IsESC(void)
{
    return (Pec12.ESC);
}

//       Pec12NoActivity   true indique abscence d'activité sur PEC12

bool Pec12NoActivity(void)
{
    return (Pec12.NoActivity);
}

//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'incrément

void Pec12ClearPlus(void)
{
    Pec12.Inc = 0;
}

//       Pec12ClearMinus   annule indication de décrément

void Pec12ClearMinus(void)
{
    Pec12.Dec = 0;
}

//       Pec12ClearOK      annule indication action OK

void Pec12ClearOK(void)
{
    Pec12.OK = 0;
}

//       Pec12ClearESC     annule indication action ESC

void Pec12ClearESC(void)
{
    Pec12.ESC = 0;
}

void Pec12ClearInactivity(void)
{
    Pec12.NoActivity = 0;
    Pec12.InactivityDuration = 0;
}


