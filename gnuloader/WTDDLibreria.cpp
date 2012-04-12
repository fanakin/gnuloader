//---------------------------------------------------------------------------
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "WTDDLibreria.h"
//---------------------------------------------------------------------------
QString wtddEstraiDallaFine(QString& Str,char Point)
{
QString res;
int Len;
int count;

Len = Str.length();
if (Len == 0) return "";
count = Len;
do {

   } while ((Str[count--] != Point) && (count > 0));

res = Str.mid(count + 2,Len - count + 1);

return res;
}

//---------------------------------------------------------------------------
unsigned long wtddahextol(const char *str)
{
unsigned long res;
unsigned short pointer;
unsigned char pow;
unsigned char n_cifre;
unsigned char index;
unsigned char car[100];
unsigned long base;

pointer = 0;
n_cifre = 0;

while ((str[pointer] == 0x0D) || (str[pointer] == 0x0A) || (str[pointer] == ' ') || (str[pointer] == 0x17))
      pointer++;
/*
while (!((str[pointer] < 0x30) || ((str[pointer] > 0x39) && (str[pointer] < 0x41)) || (str[pointer] > 0x46)))
      {
      if ((0x30 <= str[pointer]) && (str[pointer] <= 0x39))
         car[n_cifre] = (unsigned char) (((unsigned char) str[pointer]) - 0x30);
      else
         car[n_cifre] = (unsigned char) (((unsigned char) str[pointer]) - 0x37);

      n_cifre++;
      pointer++;
      }
*/
while (((str[pointer] >= 0x30) && (str[pointer] <= 0x39)) ||
        ((str[pointer] >= 0x41) && (str[pointer] <= 0x46)) ||
        ((str[pointer] >= 0x61) && (str[pointer] <= 0x66))
        )
      {
      if ((0x30 <= str[pointer]) && (str[pointer] <= 0x39))
         car[n_cifre] = (unsigned char) (((unsigned char) str[pointer]) - 0x30);
      else if ((str[pointer] >= 0x41) && (str[pointer] <= 0x46))
         car[n_cifre] = (unsigned char) (((unsigned char) str[pointer]) - 0x37);
      else
         car[n_cifre] = (unsigned char) (((unsigned char) str[pointer]) - 0x57);
      
      n_cifre++;
      pointer++;
      }
res = 0;
for (index = 0; index < n_cifre; index++)
    {
    base = 1;
    for (pow = 0; pow < (n_cifre - index - 1); pow++)
        base *= 16;
    res += (base * car[index]);
    }

return res;
}
//---------------------------------------------------------------------------
bool wtddCoerenzaNome(const QString Nome,QString& Device)
{
QString App(Nome);
App.remove(App.find('.'),App.length() - App.find('.') + 1);
char StringaNome[64];
int StringaNomeLen = App.length();
strcpy(StringaNome,App);
Device = "";

bool trovato = false;
int index = 0;
while ((index < StringaNomeLen) && (!trovato))
      {
      if (StringaNome[index] == '_')
         trovato = true;
      index++;
      }
if (!trovato)
   return false;
int IndexUnderscore = index - 1;

trovato = false;
while ((index < StringaNomeLen) && (!trovato))
      {
      if (StringaNome[index] == '$')
         trovato = true;
      index++;
      }

if (!trovato)
   return false;
int IndexDollar = index - 1;
if ((IndexDollar - IndexUnderscore - 1) != 8)
   return false;

for (int i = 1; i < (IndexDollar - IndexUnderscore); i++)
    Device += StringaNome[IndexUnderscore + i];

return true;
}

//---------------------------------------------------------------------------
bool wtddTrovaFile(const QString& FNam)
{
	int len;
	DIR* dirp;
	dirent* dp;

	len = strlen(FNam);
	dirp = opendir(".");
	if (dirp == NULL) {
		return false;
	}
	while ((dp = readdir(dirp)) != NULL) {
		if (!strcmp(dp->d_name, FNam)) {
			closedir(dirp);
			return true;
			}
		}
	closedir(dirp);
	return false;
}
//---------------------------------------------------------------------------
QString wtddEstraiDallaFineNum(QString& Str,int NumChar)
{
QString res;
int Len = Str.length();

res = Str.mid(Len - NumChar,NumChar + 1);
return res;
}
//---------------------------------------------------------------------------


