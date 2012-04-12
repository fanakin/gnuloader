//---------------------------------------------------------------------------
#ifndef WTDDLibreriaH
#define WTDDLibreriaH

#include <qstring.h>

//---------------------------------------------------------------------------
// estrae dalla fine tutta una stringa fino alla prima occorrenza del carattere Point
QString wtddEstraiDallaFine(QString& Str,char Point);
// estrae dalla fine i primi NumChar caratteri
QString wtddEstraiDallaFineNum(QString& Str,int NumChar);
// converte str in un unsigned long; in caso di errore restituisce 0
unsigned long wtddahextol(const char *str);
// verifica la coerenza dei nomi dei device ed i codici relativi
bool wtddCoerenzaNome(const QString Nome,QString& Device);
// cerca il file FNam nella directory corrente
bool wtddTrovaFile(const QString& FNam);

#endif
