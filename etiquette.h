#ifndef ETIQUETTE_H
#define ETIQUETTE_H

#include <QString>

enum FIELDCODE {ESPECE,TYPE,INDICE,ZONE,SITE,COMMENTAIRE,MATERIEL,CONFIDENTIEL,DATENREG,AUTEUR,ETIQUETEUR,NBFIELDS};

class Etiquette
{
public:
    //explicit Etiquette(int,QString,QString,int,QString,QString,QString,QString,QString,QString,QString,QString);
    explicit Etiquette(int num_cri);
    Etiquette();
    void vide();
    int e_numCri;
    QString DataFields[NBFIELDS];
	int SpecNumber;
    void recopie(Etiquette *etisource);

private:
};

#endif // ETIQUETTE_H


