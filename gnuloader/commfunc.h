/*
*/

#ifndef commfuncH
#define commfuncH 

#include <q3cstring.h>
#include <qstring.h>
#include <qstringlist.h>

#include <stdio.h>
//#include <dir.h>
#include "Containers.h"

#define UNDEF	0
#define NAME	1
#define NUMBER	2
#define SYMBOL	3

unsigned long lmirror(unsigned long value);

unsigned short smirror(unsigned short value);

void strfld(FILE *in, char *str, int lim);

void copyfile(char *destination, char *source);

//---------------- File Names Manipulation ----------------------------------

Q3CString FileFromPath(Q3CString apath);

Q3CString FileWoExtFromPath(Q3CString apath);

Q3CString ExtFromPath(Q3CString apath);   // include .

Q3CString DirFromPath(Q3CString apath);   // include "\\" at the end !

Q3CString DirWoDriveFromPath(Q3CString apath);  // include "\\" in front & at the end !

Q3CString DriveFromPath(Q3CString apath);  // include : & without "\\"

//---------------- Float Number Manipulation -------------------------------

Q3CString WTDDCCommaToDot(Q3CString CommaStr); // in a flaoting point it changes ',' into '.'

Q3CString WTDDCDotToComma(Q3CString DotStr); // in a floating point it changes '.' into ','

int WTDDRoundSLong(double In); // rounds the double to the nearest int (or signed long)

unsigned long WTDDRoundULong(double In); // rounds the double to the nearest unsigned long

#endif
