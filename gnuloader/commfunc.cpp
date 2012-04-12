/*
*/


#define MAXDRIVE  6
#define MAXDIR    1024
#define MAXFILE   1024
#define MAXEXT    32

#include <math.h>
#include <stdlib.h>

#include "commfunc.h"
//#include "appcfg.h"

//------------------------------------------------------------------------------
unsigned long lmirror(unsigned long value)
{
    unsigned long  r;
    unsigned char *v = (((unsigned char*) &value) + 3);
    unsigned char *x = (unsigned char*) &r;
    *(x++) = *(v--);
    *(x++) = *(v--);
    *(x++) = *(v--);
    *x = *v;
    return r;
}
//---------------------------------------------------------------------------
unsigned short smirror(unsigned short value)
{
    unsigned short  r;
    unsigned char *v = (((unsigned char*) &value) + 1);
    unsigned char *x = (unsigned char*) &r;
    *(x++) = *(v--);
    *x = *v;
    return r;
}

void strfld(FILE *in, char *str, int lim)
{
	int pt=0;
	int c;

	while((pt<lim)&&(c=fgetc(in))!='\n' && (c!=EOF)) str[pt++]= (char) c;
	str[pt]='\0';
        if (pt == 0) return;
        if (str[pt-1] == '\r') str[pt-1] = 0;
}
//------------------------------------------------------------------------------

void copyfile(char *destination, char *source)
{
    FILE *in, *out;
    int c;

    in = fopen(source,"rb");
    if (in == NULL) {
	  Q3CString t(source);
      printf("Error: Open %s\n",t.data());
      return;
      }
    out = fopen(destination,"wb");
    if (out == NULL) {
	  Q3CString t(destination);
      printf("Error: Open %s\n",t.data());
      return;
      }

    for(;;) {
       if ((c = fgetc(in)) == EOF) break;
       fputc(c,out);
    }

    fclose(in);
    fclose(out);
}
//---------------------------------------------------------------------------

Q3CString FileFromPath(Q3CString apath)
{
//   char drive[MAXDRIVE];
//   char dir[MAXDIR];
   char name[MAXFILE];
   char ext[MAXEXT];

//   fnsplit(apath.c_str(), drive, dir, name, ext);
   return Q3CString(name) + Q3CString(ext);
}
//---------------------------------------------------------------------------

Q3CString FileWoExtFromPath(Q3CString apath)
{
//   char drive[MAXDRIVE];
//   char dir[MAXDIR];
   char name[MAXFILE];
//   char ext[MAXEXT];

//   fnsplit(apath.c_str(), drive, dir, name, ext);
   return Q3CString(name);
}
//---------------------------------------------------------------------------

Q3CString ExtFromPath(Q3CString apath)
{
//   char drive[MAXDRIVE];
//   char dir[MAXDIR];
//   char name[MAXFILE];
   char ext[MAXEXT];

//   fnsplit(apath.c_str(), drive, dir, name, ext);
   return Q3CString(ext);
}
//---------------------------------------------------------------------------

Q3CString DirFromPath(Q3CString apath)
{
   char drive[MAXDRIVE];
   char dir[MAXDIR];
//   char name[MAXFILE];
//   char ext[MAXEXT];

//   fnsplit(apath.c_str(), drive, dir, name, ext);
   return Q3CString(drive) + Q3CString(dir);
}
//---------------------------------------------------------------------------

Q3CString DirWoDriveFromPath(Q3CString apath)
{
//   char drive[MAXDRIVE];
   char dir[MAXDIR];
//   char name[MAXFILE];
//   char ext[MAXEXT];

//   fnsplit(apath.c_str(), drive, dir, name, ext);
   return Q3CString(dir);
}

//---------------------------------------------------------------------------

Q3CString DriveFromPath(Q3CString apath)
{
   char drive[MAXDRIVE];
//   char dir[MAXDIR];
//   char name[MAXFILE];
//   char ext[MAXEXT];

//   fnsplit(apath.c_str(), drive, dir, name, ext);
   return Q3CString(drive);
}

//---------------------------------------------------------------------------

Q3CString WTDDCCommaToDot(Q3CString CommaStr)
{
    double TempFlt = CommaStr.toFloat();
    char TmpBff[256];
	if (sprintf(TmpBff,"%12.10g",TempFlt) == EOF) return Q3CString("");
	return Q3CString(TmpBff);
}

//---------------------------------------------------------------------------
Q3CString WTDDCDotToComma(Q3CString DotStr)
{
  Q3CString r;
  /* I search a possible , to change it into a .*/
    for (unsigned int i = 0; i < DotStr.length(); i++)
       if (DotStr[i] == ',') {DotStr[i] = '.'; break;}
    double TmpFlt = atof(DotStr);
    r.setNum(TmpFlt);
    return r;
}
//---------------------------------------------------------------------------
int WTDDRoundSLong(double In)
{
	int i = (int)floor(In);
	if (In == 0) return 0;
    else if (In > 0) {
	    if ((In - ((double) i)) < 0.5) return i;
    	else {
        	if (((double) (i + 1)) >= 0x7FFFFFFF)
            	return 0x7FFFFFFF;
            else return ++i;
            }
        }
    else {
	    if ((In - ((double) i)) < 0.5) return i;
    	else return (++i);
	    }
}
//---------------------------------------------------------------------------
unsigned long WTDDRoundULong(double In)
{
	unsigned long i = (unsigned long)floor(In);
	if (In == 0) return 0;
	if (In > 0) {
	    if ((In - ((double) i)) < 0.5) return i;
    	else {
        	if (((double) (i + 1)) >= 0xFFFFFFFF)
            	return 0xFFFFFFFF;
            else return ++i;
            }
        }
    else return 0;
}
