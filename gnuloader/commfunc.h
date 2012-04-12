/*
*/

#ifndef commfuncH
#define commfuncH 

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

QString FileFromPath(QString apath);

QString FileWoExtFromPath(QString apath);

QString ExtFromPath(QString apath);   // include .

QString DirFromPath(QString apath);   // include "\\" at the end !

QString DirWoDriveFromPath(QString apath);  // include "\\" in front & at the end !

QString DriveFromPath(QString apath);  // include : & without "\\"

//---------------- Float Number Manipulation -------------------------------

QString WTDDCCommaToDot(QString CommaStr); // in a flaoting point it changes ',' into '.'

QString WTDDCDotToComma(QString DotStr); // in a floating point it changes '.' into ','

int WTDDRoundSLong(double In); // rounds the double to the nearest int (or signed long)

unsigned long WTDDRoundULong(double In); // rounds the double to the nearest unsigned long

#endif
