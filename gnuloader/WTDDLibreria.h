//---------------------------------------------------------------------------
#ifndef WTDDLibreriaH
#define WTDDLibreriaH

#include <q3cstring.h>

//---------------------------------------------------------------------------
// estrae dalla fine tutta una stringa fino alla prima occorrenza del carattere Point
Q3CString wtddEstraiDallaFine(Q3CString& Str,char Point);
// estrae dalla fine i primi NumChar caratteri
Q3CString wtddEstraiDallaFineNum(Q3CString& Str,int NumChar);
// converte str in un unsigned long; in caso di errore restituisce 0
unsigned long wtddahextol(const char *str);
// verifica la coerenza dei nomi dei device ed i codici relativi
bool wtddCoerenzaNome(const Q3CString Nome,Q3CString& Device);
// cerca il file FNam nella directory corrente
bool wtddTrovaFile(const Q3CString& FNam);

#endif
