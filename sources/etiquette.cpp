#include "etiquette.h"

Etiquette::Etiquette()
{
    CallNumber = -1;
    for(int iField=0;iField<NBFIELDS;iField++) DataFields[iField] = "";
    SpecNumber = -1;
}

Etiquette::Etiquette(int numcri)
{
    CallNumber = numcri;
    for(int iField=0;iField<NBFIELDS;iField++) DataFields[iField] = "";
    SpecNumber = -1;
}

void Etiquette::EtiquetteClear()
{
    for(int iField=0;iField<NBFIELDS;iField++) DataFields[iField] = "";
    SpecNumber = -1;
}

void Etiquette::EtiquetteClone(Etiquette *etisource)
{
    for(int iField=0;iField<NBFIELDS;iField++)
        DataFields[iField] = etisource->DataFields[iField];
}


