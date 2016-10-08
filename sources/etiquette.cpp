#include "etiquette.h"

Etiquette::Etiquette()
{
    CallNumber = -1;
    for(int iField=0;iField<NBFIELDS;iField++) DataFields[iField] = "";
    SpecNumber = -1;
}

Etiquette::Etiquette(int callNumber)
{
    CallNumber = callNumber;
    for(int iField=0;iField<NBFIELDS;iField++) DataFields[iField] = "";
    SpecNumber = -1;
}

void Etiquette::EtiquetteClear()
{
    for(int iField=0;iField<NBFIELDS;iField++) DataFields[iField] = "";
    SpecNumber = -1;
}

void Etiquette::EtiquetteClone(Etiquette *labelSource)
{
    for(int iField=0;iField<NBFIELDS;iField++)
        DataFields[iField] = labelSource->DataFields[iField];
}


