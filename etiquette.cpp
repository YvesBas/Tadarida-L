#include "etiquette.h"

Etiquette::Etiquette()
{
    e_numCri = -1;
    for(int iField=0;iField<NBFIELDS;iField++) DataFields[iField] = "";
    SpecNumber = -1;
}

Etiquette::Etiquette(int numcri)
{
    e_numCri = numcri;
    for(int iField=0;iField<NBFIELDS;iField++) DataFields[iField] = "";
    SpecNumber = -1;
}

void Etiquette::vide()
{
    for(int iField=0;iField<NBFIELDS;iField++) DataFields[iField] = "";
    SpecNumber = -1;
}

void Etiquette::recopie(Etiquette *etisource)
{
    for(int iField=0;iField<NBFIELDS;iField++)
        DataFields[iField] = etisource->DataFields[iField];
}


