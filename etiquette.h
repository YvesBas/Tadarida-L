#ifndef ETIQUETTE_H
#define ETIQUETTE_H

#include <QString>

enum FIELDCODE {ESPECE,TYPE,INDICE,ZONE,SITE,COMMENTAIRE,MATERIEL,CONFIDENTIEL,DATENREG,AUTEUR,ETIQUETEUR,NBFIELDS};

class Etiquette
{
public:
    explicit Etiquette(int);
    Etiquette();

    void                       EtiquetteClear();
    void                       EtiquetteClone();
    void                       EtiquetteClone(Etiquette *);

    int                        CallNumber;
    QString                    DataFields[NBFIELDS];
    int                        SpecNumber;

private:
};

#endif // ETIQUETTE_H


