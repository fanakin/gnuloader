//---------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <math.h>
#include "commfunc.h"
#include "WTDDLibreria.h"
//#include "MainForm.h"
//#include "CPasswordForm.h"
//#include "TrasfMon.h"
#include "TabStrc.h"
//---------------------------------------------------------------------------

bool ChkBitN(unsigned int numb, int pos)
{
    unsigned int msk = 1;
    for (int i = 0; i < pos; i++) msk *= 2;
    if ((numb & msk) != 0) return true;
    return false;
}

Element::Element()
{
    id = undef;
    name = "UNDEFINED";
    memo = "This element is not defined";
    readonly = false;
    xname_on = "";
    xname_off = "";
    xmin_on = 0;
    xmin_off = 0;
    xmax_on = 1;
    xmax_off = 1;
    xbpos = -1;
    yname_on = "";
    yname_off = "";
    ymin_on = 0;
    ymin_off = 0;
    ymax_on = 1;
    ymax_off = 1;
    ybpos = -1;
    zname_on = "";
    zname_off = "";
    zmin_on = 0;
    zmin_off = 0;
    zmax_on = 1;
    zmax_off = 1;
    zbpos = -1;
    indxon = (unsigned int)-1;
    indxoff = (unsigned int)-1;
    indyon = (unsigned int)-1;
    indyoff = (unsigned int)-1;

    xvmaxon = (unsigned int)-1;
    xvminon = (unsigned int)-1;
    xvmaxoff = (unsigned int)-1;
    xvminoff = (unsigned int)-1;
    yvmaxon = (unsigned int)-1;
    yvminon = (unsigned int)-1;
    yvmaxoff = (unsigned int)-1;
    yvminoff = (unsigned int)-1;
    zvmaxon = (unsigned int)-1;
    zvminon = (unsigned int)-1;
    zvmaxoff = (unsigned int)-1;
    zvminoff = (unsigned int)-1;

}

Table::Table(int grp,
          int caddr,
		  Q3CString tname,
          int tsize,
          int tbytes,
		  Q3CString txname,
          double txmin,
          double txmax,
		  Q3CString tyname,
          double tymin,
          double tymax,
		  Q3CString cmemo) : Element()
{
    int i;

    id = table;
    gruppo = grp & 0x0000FFFF;
    if (grp & 0x40000000) segno = true;
    else segno = false;
    addr = caddr;
    name = tname;
    size = tsize;
    bytes = tbytes;
    xname = txname;
    xmin = txmin;
    xmax = txmax;
    yname = tyname;
    ymin = tymin;
    ymax = tymax;
    memo = cmemo;
    xpt = new double[size];
    for (i = 0; i< size; i++) {
    	xpt[i] = xmin + ((xmax - xmin) / (double)(size - 1)) * (double)i;
    }
    visymin = ymin;
    visymax = ymax;
}

Table::~Table()
{
    delete [] xpt;
}

double Table::GetXVal(int i)
{
	if ( i < 0 || i >= size) throw EElement("Table::GetXVal Out of range");
	return xpt[i];
}

double Table::GetYVal(Tabella *ctab, int i)
{
    if ( i < 0 || i >= size) throw EElement("Table::GetYVal Out of range");
    int displ = bytes * i;
    unsigned int cval;
    int sval;
    double aval;

    if (segno) {
      sval = ctab->GetSVal(bytes, addr + displ);
      if (bytes == 1) {
    	aval = ymin + (((double)(sval+128) / 255.0) * (ymax - ymin));
      }
      else if (bytes == 2) {
    	aval = ymin + (((double)(sval+32768) / 65535.0) * (ymax - ymin));
      }
      else if (bytes == 4) {
    	aval = ymin + (((((double)sval) + 2147483648.0) / 4294967295.0) * (ymax - ymin));
      }
      else throw EElement("Table word size");
    }
    else {
      cval = ctab->GetVal(bytes, addr + displ);
      if (bytes == 1) {
    	aval = ymin + (((double)cval / 255.0) * (ymax - ymin));
      }
      else if (bytes == 2) {
    	aval = ymin + (((double)cval / 65535.0) * (ymax - ymin));
      }
      else if (bytes == 4) {
    	aval = ymin + (((double)cval / 4294967295.0) * (ymax - ymin));
      }
      else throw EElement("Table word size");
    }
    return aval;
}

double Table::GetXZeroN(void)
{
    double v = 0;
    if ((xmax - xmin) <= 0) return -1.0;
    return (v - xmin) / (xmax - xmin);
}

double Table::GetYZeroN(void)
{
    double v = 0;
    if ((ymax - ymin) <= 0) return -1.0;
    return (v - ymin) / (ymax - ymin);
}

double Table::GetYZeroD(void)
{
    double v = 0;

    if ((visymax - visymin) <= 0) return -1.0;
    return (v - visymin) / (visymax - visymin);
}

double Table::GetXValN(int i)
{
    double v = GetXVal(i);
    if ((xmax - xmin) <= 0) return 0;
    return (v - xmin) / (xmax - xmin);
}

double Table::GetYValN(Tabella *ctab, int i)
{
    double v = GetYVal(ctab,i);
    if ((ymax - ymin) <= 0) return 0;
    return (v - ymin) / (ymax - ymin);
}

double Table::GetYValD(Tabella *ctab, int i)
{
    double v = GetYVal(ctab,i);
    if ((visymax - visymin) <= 0) return 0;
    return (v - visymin) / (visymax - visymin);
}

void Table::SetXVal(double val, int i)
{
  if (i > size - 1) throw EElement("Table::SetXVal Out of range");
  if (i < 0) throw EElement("Table::SetXVal Out of range");
  xpt[i] = val;
}

void Table::SetYVal(Tabella *ctab, double val, int i, bool undo)
{
    if (readonly) return;
    if ( i < 0 || i >= size) throw EElement("Table::GetYVal Out of range");
    int displ = bytes * i;
    unsigned int cval;
    int sval;

    if (val < ymin) val = ymin;
    if (val > ymax) val = ymax;

    if (segno) {
      if (bytes == 1) {
        double tmp = (val - ymin);
        tmp *= 255.0;
        tmp /= (ymax - ymin);
        tmp -= 128.0;
        //(255.0 * (val - ymin) / (ymax - ymin) - 128);
    	sval = WTDDRoundSLong(tmp);
      }
      else if (bytes == 2) {
        double tmp = val - ymin;
        tmp *= 65535.0;
        tmp /= (ymax - ymin);
        tmp -= 32768.0;
        //(65535.0 * (val - ymin) / (ymax - ymin) - 32768.0);



    	sval = WTDDRoundSLong(tmp);
      }
      else if (bytes == 4) {
        double tmp = (val - ymin);
        tmp *= 4294967295.0;
        tmp /= (ymax - ymin);
        tmp -= 2147483648.0;
        //(4294967295.0 * (val - ymin) / (ymax - ymin) - 2147483648);
        sval = WTDDRoundSLong(tmp);
      }
      else throw EElement("Table word size");
      ctab->SetSVal(bytes,addr + displ,sval, undo);
    }
    else {
      if (bytes == 1) {
        double tmp = (val - ymin);
        tmp *= 255.0;
        tmp /= (ymax - ymin);
        cval = WTDDRoundULong(tmp);
    	//cval = (unsigned int) (255.0 * (val - ymin) / (ymax - ymin));
      }
      else if (bytes == 2) {
        double tmp = (val - ymin);
        tmp *= 65535.0;
        tmp /= (ymax - ymin);
        cval = WTDDRoundULong(tmp);
    	//cval = (unsigned int) (65535.0 * (val - ymin) / (ymax - ymin));
      }
      else if (bytes == 4) {
        double tmp = (val - ymin);
        tmp *= 4294967295.0;
        tmp /= (ymax - ymin);
        cval = WTDDRoundULong(tmp);
   		//cval = (unsigned int) (4294967295.0 * (val - ymin) / (ymax - ymin));
      }
      else throw EElement("Table word size");
      ctab->SetVal(bytes,addr + displ,cval, undo);
    }

}

void Table::SetYValN(Tabella *ctab, double val, int i)
{
    if (readonly) return;
    val = ymin + val * (ymax - ymin);
    SetYVal(ctab,val,i);
}

void Table::SetYValD(Tabella *ctab, double val, int i)
{
    if (readonly) return;
    val = visymin + val * (visymax - visymin);
    SetYVal(ctab,val,i);
}

bool Table::IsDrawable()
{
    if ((xmax - xmin) <= 0) return false;
    if ((ymax - ymin) <= 0) return false;
    if ((visymax - visymin) <= 0) return false;
    return true;
}

void Table::SetOB(int xyz,int bitpos,Q3CString onname, double onmin, double onmax,
		Q3CString offname, double offmin, double offmax)
{
	if (xyz == R_ONBIT_X) {
    	xname_on = onname;
    	xname_off = offname;
    	xmin_on = onmin;
    	xmin_off = offmin;
    	xmax_on = onmax;
    	xmax_off = offmax;
    	xbpos = bitpos;
    }
    if (xyz == R_ONBIT_Y) {
    	yname_on = onname;
    	yname_off = offname;
    	ymin_on = onmin;
    	ymin_off = offmin;
    	ymax_on = onmax;
    	ymax_off = offmax;
    	ybpos = bitpos;
    }
    if (xyz == R_ONBIT_Z) throw EElement("Table::SetOB Z");
}

void Table::SetOBT(int xyz, int bitpos, unsigned int idx1, unsigned int idx2)
{
	if (xyz == R_ONBIT_X_A) {
        indxon = idx1;
        indxoff = idx2;
    	xbpos = bitpos;
    }
    if (xyz == R_ONBIT_Y_A) throw EElement("Table::SetOB Y");
}
void Table::SetOBV(int xyz,int bitpos,unsigned int idx1, unsigned int idx2,
		unsigned int idx3, unsigned int idx4,Q3CString onname,Q3CString offname)
{
	if (xyz == R_ONBIT_X_V) {
        xname_on = onname;
    	xname_off = offname;
    	xvmaxon = idx2;
    	xvminon = idx1;
    	xvmaxoff = idx4;
    	xvminoff = idx3;
    	xbpos = bitpos;
    }
	if (xyz == R_ONBIT_Y_V) {
    	yname_on = onname;
    	yname_off = offname;
    	yvmaxon = idx2;
    	yvminon = idx1;
    	yvmaxoff = idx4;
    	yvminoff = idx3;
    	ybpos = bitpos;
    }
    if (xyz == R_ONBIT_Z_V) throw EElement("Table::SetOBV Z");
}

void Table::RunOB(int xyz,unsigned int number)
{
	if (xyz == R_ONBIT_X) {
        if (xbpos < 0) throw EElement("Table::RunOB X");
    	if (ChkBitN(number, xbpos)) {
        	xname =	xname_on;
    		xmin = xmin_on;
    		xmax = xmax_on;
        }
        else {
        	xname =	xname_off;
    		xmin = xmin_off;
    		xmax = xmax_off;
        }
    }
    if (xyz == R_ONBIT_Y) {
    	if (ybpos < 0) throw EElement("Table::RunOB Y");
    	if (ChkBitN(number, ybpos)) {
        	yname =	yname_on;
    		ymin = ymin_on;
    		ymax = ymax_on;
        }
        else {
        	yname =	yname_off;
    		ymin = ymin_off;
    		ymax = ymax_off;
        }
    }
    if (xyz == R_ONBIT_Z) throw EElement("Table::RunOB Z");
    if (xyz == R_ONBIT_X) {
    	for (int i = 0; i < size; i++) {
    		xpt[i] = xmin + ((xmax - xmin) / (double)(size - 1)) * (double)i;
    	}
    }
}

void Table::RunOBT(int xyz, unsigned int number, Element **earr, Tabella *ctab)
{
	if (xyz == R_ONBIT_X_A) {
        if (xbpos < 0) throw EElement("Table::RunOBT X");
    	if (ChkBitN(number, xbpos)) {
			RunArray(R_ARRAY_X,(Table*)earr[indxon],ctab);
            xname = ((Table*)earr[indxon])->GetYName();
        }
        else {
			RunArray(R_ARRAY_X,(Table*)earr[indxoff],ctab);
            xname = ((Table*)earr[indxoff])->GetYName();
        }
    }
    if (xyz == R_ONBIT_Y_A) throw EElement("Table::RunOBT Y");
}

void Table::RunOBV(int xyz, unsigned int number, Element **earr, Tabella *ctab)
{
	if (xyz == R_ONBIT_X_V) {
        if (xbpos < 0) throw EElement("Table::RunOBV X");
    	if (ChkBitN(number, xbpos)) {
         	xname = xname_on;
    		xmin = ((Variable*)earr[xvminon])->GetValue(ctab);
    		xmax = ((Variable*)earr[xvmaxon])->GetValue(ctab);
        }
        else {
         	xname = xname_off;
    		xmin = ((Variable*)earr[xvminoff])->GetValue(ctab);
    		xmax = ((Variable*)earr[xvmaxoff])->GetValue(ctab);
        }
    }
	if (xyz == R_ONBIT_Y_V) {
        if (ybpos < 0) throw EElement("Table::RunOBV Y");
    	if (ChkBitN(number, ybpos)) {
         	yname = yname_on;
    		ymin = ((Variable*)earr[yvminon])->GetValue(ctab);
    		ymax = ((Variable*)earr[yvmaxon])->GetValue(ctab);
        }
        else {
         	yname = yname_off;
    		ymin = ((Variable*)earr[yvminoff])->GetValue(ctab);
    		ymax = ((Variable*)earr[yvmaxoff])->GetValue(ctab);
        }
    }
    if (xyz == R_ONBIT_Z_V) throw EElement("Table::RunOBV Z");
}

void Table::RunMinMax(int mode,double value)
{
	if (mode == R_MAX_X) xmax = value;
	if (mode == R_MIN_X) xmin = value;
	if (mode == R_MAX_Y) ymax = value;
	if (mode == R_MIN_Y) ymin = value;
	if (mode == R_MAX_Z) throw EElement("Table::RunMinMax");
	if (mode == R_MIN_Z) throw EElement("Table::RunMinMax");
    if (mode == R_MAX_X || mode == R_MIN_X) {
    	for (int i = 0; i < size; i++) {
    		xpt[i] = xmin + ((xmax - xmin) / (double)(size - 1)) * (double)i;
    	}
    }
}

void Table::RunArray(int mode,Table *el,Tabella *ctab)
{
	if (mode == R_ARRAY_X) {
    	int i, lim = size;
    	if (el->GetSize() < size) lim = size;
        xname = el->GetYName();
		for (i = 0; i < lim; i++) xpt[i] = el->GetYVal(ctab,i);
		for (i = 1; i < size; i++) if (xpt[i] < xpt[i-1]) xpt[i] = xpt[i-1];
        xmin = xpt[0];
        xmax = xpt[size-1];
    }
	if (mode == R_ARRAY_Y) throw EElement("Table::RunArray");
}

double Table::GetMinMax(Tabella *ctab)
{
	throw EElement("Table::GetMinMax");
}

unsigned int Table::GetBinImg(Tabella *ctab)
{
	throw EElement("Table::GetBinImg");
}

int Table::NumDecim()
{
  int ndec = 0;
  double rng = ymax - ymin;
        if (bytes == 1) rng = rng / 256.0;
        else if(bytes == 2) rng = rng / 65536.0;
        else if(bytes == 4) rng = rng / 4294967296.0;
        else throw EElement("Table word size");
        if (rng > 0.0) {
            rng = 1.0 / rng;
            ndec = 1 + (int)log10(rng);
            if (ndec < 0) ndec = 0;
        }
        return ndec;
}

void Table::RunLimits(int mode, Tabella *ctab)
{
   if (mode == R_FLOOR_Y) {
      for (int i = 0; i < size; i++)
         if (GetYVal(ctab,i) < infy) SetYVal(ctab,infy,i, false);
   }
   else if (mode == R_CEIL_Y) {
      for (int i = 0; i < size; i++)
         if (GetYVal(ctab,i) > supy) SetYVal(ctab,supy,i, false);
   }
   else throw EElement("Table RunLimits");
}

void Table::SetLimits(double val,int mode)
{
   if (mode == R_FLOOR_Y) infy = val;
   else if (mode == R_CEIL_Y) supy = val;
   else throw EElement("Table SetLimits");
}

Map::Map(int grp,
    	int caddr,
		Q3CString mname,
        int mxsize,
        int mysize,
        int mbytes,
		Q3CString mxname,
        double mxmin,
        double mxmax,
		Q3CString myname,
        double mymin,
        double mymax,
		Q3CString mzname,
        double mzmin,
        double mzmax,
		Q3CString cmemo) : Element()
{
    id = map;
    gruppo = grp & 0x0000FFFF;
    if (grp & 0x40000000) segno = true;
    else segno = false;
    addr = caddr;
    name = mname;
    xsize = mxsize;
    ysize = mysize;
    bytes = mbytes;
    if (bytes > 4) bytes = 1;
    xname = mxname;
    xmin = mxmin;
    xmax = mxmax;
    yname = myname;
    ymin = mymin;
    ymax = mymax;
    zname = mzname;
    zmin = mzmin;
    zmax = mzmax;
    memo = cmemo;
    xpt = new double[xsize];
    for (int i = 0; i < xsize; i++) {
    	xpt[i] = xmin + ((xmax - xmin) / (double)(xsize - 1)) * (double)i;
    }
    ypt = new double[ysize];
    for (int i = 0; i < ysize; i++) {
    	ypt[i] = ymin + ((ymax - ymin) / (double)(ysize - 1)) * (double)i;
    }
    viszmin = zmin;
    viszmax = zmax;
}

Map::~Map()
{
	delete [] xpt;
	delete [] ypt;
}

double Map::GetXVal(int x)
{
	if ( x < 0 || x >= xsize) throw EElement("Map::GetXVal Out of range");
	return xpt[x];
}
double Map::GetYVal(int y)
{
	if ( y < 0 || y >= ysize)
        throw EElement("Map::GetYVal Out of range");
	return ypt[y];
}

double Map::GetZVal(Tabella *ctab, int x, int y)
{
    if ( x < 0 || x >= xsize) throw EElement("Map::GetZVal Out of range");
    if ( y < 0 || y >= ysize) throw EElement("Map::GetZVal Out of range");
    int displ = bytes * (xsize * y + x);
    unsigned int cval;
    int sval;
    double aval;

    if (segno) {
      sval = ctab->GetSVal(bytes, addr + displ);
      if (bytes == 1) {
    	aval = zmin + (((double)(sval+128) / 255.0) * (zmax - zmin));
      }
      else if (bytes == 2) {
        aval = ((double)(sval+32768));
        aval /= 65535.0;
        aval *= (zmax - zmin);
        aval += zmin;
    	/*aval = zmin + (((double)(sval+32768) / 65535.0) * (zmax - zmin));*/
      }
      else if (bytes == 4) {
    	aval = zmin + (((double)(sval+2147483648.0) / 4294967295.0) * (zmax - zmin));
      }
      else throw EElement("Map word size");
    }
    else {
      cval = ctab->GetVal(bytes, addr + displ);
      if (bytes == 1) {
    	aval = zmin + (((double)cval / 255.0) * (zmax - zmin));
      }
      else if (bytes == 2) {
    	aval = zmin + (((double)cval / 65535.0) * (zmax - zmin));
      }
      else if (bytes == 4) {
    	aval = zmin + (((double)cval / 4294967295.0) * (zmax - zmin));
      }
      else throw EElement("Map word size");
    }
    return aval;
}

double Map::GetXValN(int x)
{
	double v = GetXVal(x);
    if ((xmax - xmin) <= 0) return 0;
    return (v - xmin) / (xmax - xmin);
}

double Map::GetYValN(int y)
{
	double v = GetYVal(y);
    if ((ymax - ymin) <= 0) return 0;
    return (v - ymin) / (ymax - ymin);
}

double Map::GetZValN(Tabella *ctab, int x, int y)
{
	double v = GetZVal(ctab, x, y);
    if ((zmax - zmin) <= 0) return 0;
    return (v - zmin) / (zmax - zmin);
}

double Map::GetZValD(Tabella *ctab, int x, int y)
{
    double v = GetZVal(ctab, x, y);
    if ((viszmax - viszmin) <= 0) return 0;
    return (v - viszmin) / (viszmax - viszmin);
}

void Map::SetZVal(Tabella *ctab, double val, int x, int y, bool undo)
{
	if (readonly) return;
	if ( x < 0 || x >= xsize) throw EElement("Map::GetZVal Out of range");
	if ( y < 0 || y >= ysize) throw EElement("Map::GetZVal Out of range");
    int displ = bytes * (xsize * y + x);
    unsigned int cval;
    int sval;

    if (val < zmin) val = zmin;
    if (val > zmax) val = zmax;
    if (zmax <= zmin) return;

    if (segno) {
      if (bytes == 1) {
        double tmp = (val - zmin);
        tmp *= 255.0;
        tmp /= (zmax - zmin);
        tmp -= 128.0;
        sval = WTDDRoundSLong(tmp);
    	//sval = (unsigned int) (255.0 * (val - zmin) / (zmax - zmin) - 128);
      }
      else if (bytes == 2) {
        double tmp = (val - zmin);
        tmp *= 65535;
        tmp /= (zmax - zmin);
        tmp -= 32768;
        sval = WTDDRoundSLong(tmp);
    	//sval = (unsigned int) (65535.0 * (val - zmin) / (zmax - zmin) - 32768);
      }
      else if (bytes == 4) {
        double tmp = (val - zmin);
        tmp *= 4294967295.0;
        tmp /= (zmax - zmin);
        tmp -= 2147483648.0;
        sval = WTDDRoundSLong(tmp);
        //sval = (unsigned int) (4294967295.0 * (val - zmin) / (zmax - zmin) - 2147483648);
      }
      else throw EElement("Map word size");
      ctab->SetSVal(bytes,addr + displ,sval, undo);
    }
    else {
      if (bytes == 1) {
        double tmp = (val - zmin);
        tmp *= 255.0;
        tmp /= (zmax - zmin);
        cval = WTDDRoundULong(tmp);

    	//cval = (unsigned int) (255.0 * (val - zmin) / (zmax - zmin));
      }
      else if (bytes == 2) {
        double tmp = (val - zmin);
        tmp *= 65535.0;
        tmp /= (zmax - zmin);
        cval = WTDDRoundULong(tmp);
    	//cval = (unsigned int) (65535.0 * (val - zmin) / (zmax - zmin));
      }
      else if (bytes == 4) {
        double tmp = (val - zmin);
        tmp *= 4294967295.0;
        tmp /= (zmax - zmin);
        cval = WTDDRoundULong(tmp);
        //cval = (unsigned int) (4294967295.0 * (val - zmin) / (zmax - zmin));
      }
      else throw EElement("Map word size");
      ctab->SetVal(bytes,addr + displ,cval, undo);
    }
}

void Map::SetZValN(Tabella *ctab, double val, int x, int y)
{
    if (readonly) return;
    val = zmin + val * (zmax - zmin);
    SetZVal(ctab, val, x, y);
}
void Map::SetZValD(Tabella *ctab, double val, int x, int y)
{
    if (readonly) return;
    val = viszmin + val * (viszmax - viszmin);
    SetZVal(ctab, val, x, y);
}

bool Map::IsDrawable()
{
    if ((xmax - xmin) <= 0) return false;
    if ((ymax - ymin) <= 0) return false;
    if ((zmax - zmin) <= 0) return false;
    if ((viszmax - viszmin) <= 0) return false;
    return true;
}


void Map::SetOB(int xyz,int bitpos,Q3CString onname, double onmin, double onmax,
		Q3CString offname, double offmin, double offmax)
{
	if (xyz == R_ONBIT_X) {
    	xname_on = onname;
    	xname_off = offname;
    	xmin_on = onmin;
    	xmin_off = offmin;
    	xmax_on = onmax;
    	xmax_off = offmax;
    	xbpos = bitpos;
    }
    if (xyz == R_ONBIT_Y) {
    	yname_on = onname;
    	yname_off = offname;
    	ymin_on = onmin;
    	ymin_off = offmin;
    	ymax_on = onmax;
    	ymax_off = offmax;
    	ybpos = bitpos;
    }
    if (xyz == R_ONBIT_Z) {
    	zname_on = onname;
    	zname_off = offname;
    	zmin_on = onmin;
    	zmin_off = offmin;
    	zmax_on = onmax;
    	zmax_off = offmax;
    	zbpos = bitpos;
    }
}

void Map::SetOBT(int xyz, int bitpos, unsigned int idx1, unsigned int idx2)
{
	if (xyz == R_ONBIT_X_A) {
    	indxon = idx1;
    	indxoff = idx2;
    	xbpos = bitpos;
    }
    if (xyz == R_ONBIT_Y_A) {
    	indyon = idx1;
    	indyoff = idx2;
    	ybpos = bitpos;
    }
    if (xyz == R_ONBIT_Z) throw EElement("Map::SetOBT Z");
}

void Map::SetOBV(int xyz,int bitpos,unsigned int idx1, unsigned int idx2,
		unsigned int idx3, unsigned int idx4,Q3CString onname,Q3CString offname)
{
	if (xyz == R_ONBIT_X_V) {
    	xname_on = onname;
    	xname_off = offname;
        xvmaxon = idx2;
    	xvminon = idx1;
    	xvmaxoff = idx4;
    	xvminoff = idx3;
    	xbpos = bitpos;
    }
	if (xyz == R_ONBIT_Y_V) {
    	yname_on = onname;
    	yname_off = offname;
    	yvmaxon = idx2;
    	yvminon = idx1;
    	yvmaxoff = idx4;
    	yvminoff = idx3;
    	ybpos = bitpos;
    }
	if (xyz == R_ONBIT_Z_V) {
    	zname_on = onname;

    	zname_off = offname;
    	zvmaxon = idx2;
    	zvminon = idx1;
    	zvmaxoff = idx4;
    	zvminoff = idx3;
    	zbpos = bitpos;
    }
}

void Map::RunOB(int xyz,unsigned int number)
{
	if (xyz == R_ONBIT_X) {
        if (xbpos < 0) throw EElement("Map::RunOB X");
    	if (ChkBitN(number, xbpos)) {
        	xname =	xname_on;
    		xmin = xmin_on;
    		xmax = xmax_on;
        }
        else {
        	xname =	xname_off;
    		xmin = xmin_off;
    		xmax = xmax_off;
        }
    }

    if (xyz == R_ONBIT_Y) {
    	if (ybpos < 0) throw EElement("Map::RunOB Y");
    	if (ChkBitN(number, ybpos)) {
        	yname =	yname_on;
    		ymin = ymin_on;
    		ymax = ymax_on;
        }
        else {
        	yname =	yname_off;
    		ymin = ymin_off;
    		ymax = ymax_off;
        }
    }
    if (xyz == R_ONBIT_Z) {
    	if (zbpos < 0) throw EElement("Map::RunOB Z");
    	if (ChkBitN(number, zbpos)) {
        	zname =	zname_on;
    		zmin = zmin_on;
    		zmax = zmax_on;
        }
        else {
        	zname =	zname_off;
    		zmin = zmin_off;
    		zmax = zmax_off;
        }
    }
    if (xyz == R_ONBIT_X) {
    	for (int i = 0; i < xsize; i++) {
    		xpt[i] = xmin + ((xmax - xmin) / (double)(xsize - 1)) * (double)i;
    	}
    }
    if (xyz == R_ONBIT_Y) {
    	for (int i = 0; i < ysize; i++) {
    		ypt[i] = ymin + ((ymax - ymin) / (double)(ysize - 1)) * (double)i;
    	}
    }
}

void Map::RunOBT(int xyz, unsigned int number, Element **earr, Tabella *ctab)
{
	if (xyz == R_ONBIT_X_A) {
        if (xbpos < 0) throw EElement("Table::RunOBT X");
    	if (ChkBitN(number, xbpos)) {
			RunArray(R_ARRAY_X,(Table*)earr[indxon],ctab);
            xname = ((Table*)earr[indxon])->GetYName();
        }
        else {
			RunArray(R_ARRAY_X,(Table*)earr[indxoff],ctab);
            xname = ((Table*)earr[indxoff])->GetYName();
        }
    }
	if (xyz == R_ONBIT_Y_A) {
        if (ybpos < 0) throw EElement("Table::RunOBT Y");
    	if (ChkBitN(number, ybpos)) {
			RunArray(R_ARRAY_Y,(Table*)earr[indyon],ctab);
            yname = ((Table*)earr[indyon])->GetYName();
        }
        else {
			RunArray(R_ARRAY_Y,(Table*)earr[indyoff],ctab);
            yname = ((Table*)earr[indyoff])->GetYName();
        }
    }
}

void Map::RunOBV(int xyz, unsigned int number, Element **earr, Tabella *ctab)
{
	if (xyz == R_ONBIT_X_V) {
        if (xbpos < 0) throw EElement("Map::RunOBV X");
    	if (ChkBitN(number, xbpos)) {

         	xname = xname_on;

    		xmin = ((Variable*)earr[xvminon])->GetValue(ctab);
    		xmax = ((Variable*)earr[xvmaxon])->GetValue(ctab);
        }
        else {
         	xname = xname_off;
    		xmin = ((Variable*)earr[xvminoff])->GetValue(ctab);
    		xmax = ((Variable*)earr[xvmaxoff])->GetValue(ctab);
        }
    }
	if (xyz == R_ONBIT_Y_V) {
        if (ybpos < 0) throw EElement("Map::RunOBV Y");
    	if (ChkBitN(number, ybpos)) {
         	yname = yname_on;
    		ymin = ((Variable*)earr[yvminon])->GetValue(ctab);
    		ymax = ((Variable*)earr[yvmaxon])->GetValue(ctab);
        }
        else {
         	yname = yname_off;
    		ymin = ((Variable*)earr[yvminoff])->GetValue(ctab);
    		ymax = ((Variable*)earr[yvmaxoff])->GetValue(ctab);
        }
    }
	if (xyz == R_ONBIT_Z_V) {
        if (zbpos < 0) throw EElement("Map::RunOBV Z");
    	if (ChkBitN(number, zbpos)) {
         	zname = zname_on;
    		zmin = ((Variable*)earr[zvminon])->GetValue(ctab);
    		zmax = ((Variable*)earr[zvmaxon])->GetValue(ctab);
        }
        else {
         	zname = zname_off;
    		zmin = ((Variable*)earr[zvminoff])->GetValue(ctab);
    		zmax = ((Variable*)earr[zvmaxoff])->GetValue(ctab);
        }
    }
}

void Map::RunMinMax(int mode,double value)
{
	if (mode == R_MAX_X) xmax = value;
	if (mode == R_MIN_X) xmin = value;
	if (mode == R_MAX_Y) ymax = value;
	if (mode == R_MIN_Y) ymin = value;
	if (mode == R_MAX_Z) zmax = value;
	if (mode == R_MIN_Z) zmin = value;
    if (mode == R_MAX_X || mode == R_MIN_X) {
    	for (int i = 0; i < xsize; i++) {
    		xpt[i] = xmin + ((xmax - xmin) / (double)(xsize - 1)) * (double)i;
    	}
    }
    if (mode == R_MAX_Y || mode == R_MIN_Y) {
    	for (int i = 0; i < ysize; i++) {
    		ypt[i] = ymin + ((ymax - ymin) / (double)(ysize - 1)) * (double)i;
    	}
    }
}

void Map::RunArray(int mode,Table *el,Tabella *ctab)
{
     if (mode == R_ARRAY_X) {
    	int i, lim = xsize;
    	if (el->GetSize() < xsize) lim = xsize;
        xname = el->GetYName();
        for (i = 0; i < lim; i++) xpt[i] = el->GetYVal(ctab,i);
        for (i = 1; i < xsize; i++) if (xpt[i] < xpt[i-1]) xpt[i] = xpt[i-1];
        xmin = xpt[0];
        xmax = xpt[xsize-1];
     }
     if (mode == R_ARRAY_Y) {
    	int i, lim = ysize;
    	if (el->GetSize() < ysize) lim = ysize;
        yname = el->GetYName();
        for (i = 0; i < lim; i++) ypt[i] = el->GetYVal(ctab,i);
        for (i = 1; i < ysize; i++) if (ypt[i] < ypt[i-1]) ypt[i] = ypt[i-1];
        ymin = ypt[0];
        ymax = ypt[ysize-1];
     }
}

double Map::GetMinMax(Tabella *ctab)
{
	throw EElement("Map::GetMinMax");
}

unsigned int Map::GetBinImg(Tabella *ctab)
{
    throw EElement("Map::GetBinImg");
}

int Map::NumDecim()
{
  int ndec = 0;
  double rng = zmax - zmin;
        if (bytes == 1) rng = rng / 256.0;
        else if(bytes == 2) rng = rng / 65536.0;
        else if(bytes == 4) rng = rng / 4294967296.0;
        else throw EElement("Map word size");
        if (rng > 0.0) {
            rng = 1.0 / rng;
            ndec = 1 + (int)log10(rng);
            if (ndec < 0) ndec = 0;
        }
        return ndec;
}

void Map::RunLimits(int mode, Tabella *ctab)
{
   if (mode == R_FLOOR_Z) {
      for (int i = 0; i < xsize; i++) {
         for (int j = 0; j < ysize; j++) {
            if (GetZVal(ctab,i,j) < infz) SetZVal(ctab,infz,i,j, false);
         }
      }
   }
   else if (mode == R_CEIL_Z) {
      for (int i = 0; i < xsize; i++) {

         for (int j = 0; j < ysize; j++) {
            if (GetZVal(ctab,i,j) > supz) SetZVal(ctab,supz,i,j, false);
         }
      }
   }
   else throw EElement("Map RunLimit");
}

void Map::SetLimits(double val,int mode)
{
   if (mode == R_FLOOR_Z) infz = val;
   else if (mode == R_CEIL_Z) supz = val;
   else throw EElement("Map SetLimits");
}

Variable::Variable(int grp,
          int caddr,
		  Q3CString vname,
          int vbytes,
		  Q3CString vxname,
          double vxmin,
          double vxmax,
		  Q3CString cmemo) : Element()
{
    id = variable;
    gruppo = grp & 0x0000FFFF;
    if (grp & 0x40000000) segno = true;
    else segno = false;
    addr = caddr;
    name = vname;
    bytes = vbytes;
    xname = vxname;
    xmin = vxmin;
    xmax = vxmax;
    memo = cmemo;
}

Variable::~Variable()
{

}

double Variable::GetValue(Tabella *ctab)
{
    unsigned int cval;
    int sval;
    double aval;

    if (segno) {
      sval = ctab->GetSVal(bytes,addr);
      if (bytes == 1) {
    	aval = xmin + (((double)(sval+128) / 255.0) * (xmax - xmin));
      }
      else if (bytes == 2) {
    	aval = xmin + (((double)(sval+32768) / 65535.0) * (xmax - xmin));
      }
      else if (bytes == 4) {
    	aval = xmin + (((double)(sval+2147483648.0) / 4294967295.0) * (xmax - xmin));
      }
      else throw EElement("Var word size");
    }
    else {
      cval = ctab->GetVal(bytes,addr);
      if (bytes == 1) {
    	aval = xmin + ((((double)cval) / 255.0) * (xmax - xmin));

      }
      else if (bytes == 2) {
    	aval = xmin + ((((double)cval) / 65535.0) * (xmax - xmin));
      }
      else if (bytes == 4) {
    	aval = xmin + ((((double)cval) / 4294967295.0) * (xmax - xmin));
      }
      else throw EElement("Var word size");
    }
    return aval;
}

void Variable::SetValue(Tabella *ctab, double val, bool undo)
{
    if (readonly) return;
    unsigned int cval;
    int sval;

    if (val < xmin) val = xmin;
    if (val > xmax) val = xmax;

    if (segno) {
      if (bytes == 1) {
        double tmp = (val - xmin);
        tmp *= 255.0;
        tmp /= (xmax - xmin);
        tmp -= 128.0;
        sval = WTDDRoundSLong(tmp);
    	//sval = (unsigned int) (255.0 * (val - xmin) / (xmax - xmin) - 128);
      }
      else if (bytes == 2) {
        double tmp = (val - xmin);
        tmp *= 65535.0;
        tmp /= (xmax - xmin);
        tmp -= 32768.0;
        sval = WTDDRoundSLong(tmp);
    	//sval = (unsigned int) (65535.0 * (val - xmin) / (xmax - xmin) - 32768);
      }
      else if (bytes == 4) {
        double tmp = (val - xmin);
        tmp *= 4294967295.0;
        tmp /= (xmax - xmin);
        tmp -= 2147483648.0;
        sval = WTDDRoundSLong(tmp);
        //sval = (unsigned int) (4294967295.0 * (val - xmin) / (xmax - xmin) - 2147483648);
      }
      else throw EElement("Var word size");
      ctab->SetSVal(bytes,addr,sval, undo);
    }
    else {
      if (bytes == 1) {
        double tmp = (val - xmin);
        tmp *= 255.0;
        tmp /= (xmax - xmin);
        cval = WTDDRoundULong(tmp);
    	//cval = (unsigned int) (255.0 * (val - xmin) / (xmax - xmin));
      }
      else if (bytes == 2) {
        double tmp = (val - xmin);
        tmp *= 65535.0;
        tmp /= (xmax - xmin);
        cval = WTDDRoundULong(tmp);
    	//cval = (unsigned int) (65535.0 * (val - xmin) / (xmax - xmin));
      }
      else if (bytes == 4) {
        double tmp = (val - xmin);
        tmp *= 4294967295.0;
        tmp /= (xmax - xmin);
        cval = WTDDRoundULong(tmp);
        //cval = (unsigned int) (4294967295.0 * (val - xmin) / (xmax - xmin));
      }
      else throw EElement("Var word size");
      ctab->SetVal(bytes,addr,cval, undo);
    }
}

void Variable::Increase(Tabella *ctab)
{
    if (readonly) return;

    if (segno) {
       int sval = ctab->GetSVal(bytes,addr);
       if (bytes == 1 && sval < 127) sval++;
       else if (bytes == 2 && sval < 32767) sval++;
       else if (bytes == 4 && sval < 2147483647) sval++;
       ctab->SetSVal(bytes,addr,sval);
    }
    else {
       unsigned int cval = ctab->GetVal(bytes,addr);
       if (bytes == 1 && cval < 255) cval++;
       else if (bytes == 2 && cval < 65535) cval++;
       else if (bytes == 4 && cval < 4294967295.0) cval++;
       ctab->SetVal(bytes,addr,cval);
    }
}

void Variable::Decrease(Tabella *ctab)
{
    if (readonly) return;

    if (segno) {
        int sval = ctab->GetSVal(bytes,addr);

        if (bytes == 1 && sval > -128) sval--;

        else if (bytes == 2 && sval > -32768) sval--;
        else if (bytes == 4 && (((double)sval) > -2147483648.0)) sval--;
        ctab->SetSVal(bytes,addr,sval);
    }
    else {
        unsigned int cval = ctab->GetVal(bytes,addr);
        if (cval > 0) cval--;
        ctab->SetVal(bytes,addr,cval);
    }
}

void Variable::SetOB(int xyz,int bitpos,Q3CString onname, double onmin, double onmax,
		Q3CString offname, double offmin, double offmax)
{
	if (xyz == R_ONBIT_X) {
    	xname_on = onname;
    	xname_off = offname;
    	xmin_on = onmin;
    	xmin_off = offmin;
    	xmax_on = onmax;
    	xmax_off = offmax;
    	xbpos = bitpos;
    }
    if (xyz == R_ONBIT_Y) throw EElement("Variable::SetOB Y");
    if (xyz == R_ONBIT_Z) throw EElement("Variable::SetOB Z");
}

void Variable::SetOBT(int xyz, int bitpos, unsigned int idx1, unsigned int idx2)
{
	if (xyz == R_ONBIT_X_A) throw EElement("Variable::SetOBT X");
    if (xyz == R_ONBIT_Y_A) throw EElement("Variable::SetOBT Y");
}

void Variable::SetOBV(int xyz,int bitpos,unsigned int idx1, unsigned int idx2,
		unsigned int idx3, unsigned int idx4,Q3CString onname,Q3CString offname)
{
	if (xyz == R_ONBIT_X_V) {
     	xname_on = onname;
    	xname_off = offname;
   		xvmaxon = idx2;
    	xvminon = idx1;
    	xvmaxoff = idx4;
    	xvminoff = idx3;
    	xbpos = bitpos;
    }
	if (xyz == R_ONBIT_Y_V) throw EElement("Variable::SetOBV X");
    if (xyz == R_ONBIT_Z_V) throw EElement("Variable::SetOBV Y");

}

void Variable::RunOB(int xyz,unsigned int number)
{
	if (xyz == R_ONBIT_X) {
        if (xbpos < 0) throw EElement("Variable::RunOB X");
    	if (ChkBitN(number, xbpos)) {
        	xname =	xname_on;
    		xmin = xmin_on;
    		xmax = xmax_on;
        }
        else {
        	xname =	xname_off;
    		xmin = xmin_off;
    		xmax = xmax_off;
        }
    }
    if (xyz == R_ONBIT_Y) throw EElement("Variable::RunOB Y");
    if (xyz == R_ONBIT_Z) throw EElement("Variable::RunOB Z");
}

void Variable::RunOBT(int xyz, unsigned int number, Element **earr, Tabella *ctab)
{
    if (xyz == R_ONBIT_X_A) throw EElement("Variable::RunOBT X");
    if (xyz == R_ONBIT_Y_A) throw EElement("Variable::RunOBT Y");
}

void Variable::RunOBV(int xyz, unsigned int number, Element **earr, Tabella *ctab)
{
	if (xyz == R_ONBIT_X_V) {
        if (xbpos < 0) throw EElement("Variable::RunOBV X");
    	if (ChkBitN(number, xbpos)) {
         	xname = xname_on;
    		xmin = ((Variable*)earr[xvminon])->GetValue(ctab);
    		xmax = ((Variable*)earr[xvmaxon])->GetValue(ctab);
        }
        else {
         	xname = xname_off;
    		xmin = ((Variable*)earr[xvminoff])->GetValue(ctab);
    		xmax = ((Variable*)earr[xvmaxoff])->GetValue(ctab);
        }
    }
    if (xyz == R_ONBIT_Y_V) throw EElement("Variable::RunOBV Y");
    if (xyz == R_ONBIT_Z_V) throw EElement("Variable::RunOBV Z");
}

void Variable::RunMinMax(int mode,double value)
{
	if (mode == R_MAX_X) xmax = value;
	else if (mode == R_MIN_X) xmin = value;
    else throw EElement("Variable::RunMinMax");
}

void Variable::RunArray(int mode,Table *el,Tabella *ctab)
{
 	if (mode == R_ARRAY_X) throw EElement("Variable::RunArray");
	if (mode == R_ARRAY_Y) throw EElement("Variable::RunArray");
}

double Variable::GetMinMax(Tabella *ctab)
{
	return GetValue(ctab);
}

unsigned int Variable::GetBinImg(Tabella *ctab)
{
	if (segno) return (unsigned int) ctab->GetSVal(bytes,addr);
        else return ctab->GetVal(bytes,addr);
}


bool Variable::IsDrawable()
{
    if ((xmax - xmin) <= 0) return false;
    return true;
}

int Variable::NumDecim()
{
  int ndec = 0;
  double rng = xmax - xmin;
  if (bytes == 1) rng = rng / 256.0;
  else if(bytes == 2) rng = rng / 65536.0;
  else if(bytes == 4) rng = rng / 4294967296.0;
  else throw EElement("Variable word size");
  if (rng > 0.0) {
     rng = 1.0 / rng;
     ndec = 1 + (int)log10(rng);
     if (ndec < 0) ndec = 0;
  }
  return ndec;
}

void Variable::RunLimits(int mode, Tabella *ctab)
{
   if (mode == R_FLOOR_X) {
      if (GetValue(ctab) < infx) SetValue(ctab,infx, false);
   }
   else if (mode == R_CEIL_X) {
      if (GetValue(ctab) > supx) SetValue(ctab,supx, false);
   }
   else throw EElement("Variable RunLimits");
}

void Variable::SetLimits(double val,int mode)
{
   if (mode == R_FLOOR_X) infx = val;
   else if (mode == R_CEIL_X) supx = val;
   else throw EElement("Variable SetLimits");
}

/////////////////////////////////// tabella //////////////////////////////////


Tabella::Tabella(unsigned int tsize, KaArray<unsigned int>& displ, unsigned int nb, char *name)
{
    size = tsize;
    numblocks = nb;
    if (displ.Count() != (int)nb) throw ETabella("num blocks");
    displarray = new unsigned int[numblocks];
    blocks = new unsigned char*[numblocks];
	notes = new Q3CString[numblocks];
    for (unsigned int i = 0; i < numblocks; i++ ) {
       notes[i] = "";
       blocks[i] = new unsigned char[size];
       displarray[i] = displ[i];

    }
//    undolist = new TList();
//    undolist->Clear();
//    undostate = 0;
    ClearAll();
    if (nb == 0) return;   // no device
    currblock = 0;
    displacement = displarray[0];
    Array = blocks[0];
    note = notes[0];
    strcpy(dbname,name);
}

Tabella::Tabella(const Tabella& atable)
{
//    UndoAtom *ua;
    unsigned int i, k;
//    int j;
//    int nb;
//    int ad;
//    unsigned int vl;

    size = atable.size;
    numblocks = atable.numblocks;
    displarray = new unsigned int[numblocks];
    blocks = new unsigned char*[numblocks];
	notes = new Q3CString[numblocks];
    for (i = 0; i < numblocks; i++ ) {
       notes[i] = atable.notes[i];
       blocks[i] = new unsigned char[size];
       for (k = 0; k < size; k++ ) blocks[i][k] =  atable.blocks[i][k];
       displarray[i] = atable.displarray[i];
    }
    currblock = atable.currblock;
    displacement = atable.displacement;
    Array = blocks[currblock];
    note = notes[currblock];
    rflag = atable.rflag;
    strcpy(dbname, atable.dbname);


//    undostate = atable.undostate;
//    undolist = new TList();
//    undolist->Clear();
//    for (j = atable.undolist->Count - 1; j >= 0; j--) {
//        ua = (UndoAtom*) atable.undolist->Items[j];
//        nb = ua->nbytes;
//        ad = ua->addr;
//        vl = ua->val;
//        ua = new UndoAtom;
//        ua->nbytes = (unsigned char) nb;
//        ua->addr = ad;
//        ua->val = vl;
//        undolist->Add(ua);
//    }
}

Tabella::~Tabella()
{
//    UndoAtom *ua;
//    int j;

//    for (j = undolist->Count - 1; j >= 0; j--) {
//        ua = (UndoAtom*) undolist->Items[j];
//        delete ua;
//    }
//    undolist->Clear();
//    undolist->Free();

    for (unsigned int i = 0; i < numblocks; i++ ) delete [] blocks[i];
    delete [] blocks;
    delete [] displarray;
    delete [] notes;
}

void Tabella::Clear()
{
//    UndoAtom *ua;
    unsigned int i;
//    int j;

    for(i = 0; i < size; i++) Array[i] = 0;
    rflag = 0;
    note = "";
    notes[currblock] = "";
//    for (j = undolist->Count - 1; j >= 0; j--) {
//        ua = (UndoAtom*) undolist->Items[j];
//        delete ua;
//    }
//    undolist->Clear();
}

Tabella& Tabella::operator =(const Tabella& atable)
{
//    UndoAtom *ua;
    unsigned int i, k;
//    int j;
//    int nb;
//    int ad;
//    unsigned int vl;

    for ( i = 0; i < numblocks; i++ ) delete [] blocks[i];
    delete [] blocks;
    delete [] displarray;
    delete [] notes;

    size = atable.size;
    numblocks = atable.numblocks;
    displarray = new unsigned int[numblocks];
    blocks = new unsigned char*[numblocks];
	notes = new Q3CString[numblocks];
    for (i = 0; i < numblocks; i++ ) {
       notes[i] = atable.notes[i];
       blocks[i] = new unsigned char[size];
       for (k = 0; k < size; k++ ) blocks[i][k] =  atable.blocks[i][k];
       displarray[i] = atable.displarray[i];
    }
    currblock = atable.currblock;
    displacement = atable.displacement;
    Array = blocks[currblock];
    note = notes[currblock];
    rflag = atable.rflag;
    strcpy(dbname, atable.dbname);

//    undostate = atable.undostate;
//    for (j = undolist->Count - 1; j >= 0; j--) {
//        ua = (UndoAtom*) undolist->Items[j];
//        delete ua;
//    }
//    undolist->Clear();
//    for (j = atable.undolist->Count - 1; j >= 0; j--) {
//        ua = (UndoAtom*) atable.undolist->Items[j];
//        nb = ua->nbytes;
//        ad = ua->addr;
//        vl = ua->val;
//        ua = new UndoAtom;
//        ua->nbytes = (unsigned char) nb;
//        ua->addr = ad;
//        ua->val = vl;
//        undolist->Add(ua);
//    }
    return *this;
}

bool Tabella::operator == (const Tabella& atable)
{
    if (size != atable.size) return false;
    if (numblocks != atable.numblocks) return false;
    for (unsigned int i = 0; i < numblocks; i++ ) {
       for (unsigned int j = 0; j < size; j++ )
          if (blocks[i][j] != atable.blocks[i][j]) return false;
       if (displarray[i] != atable.displarray[i]) return false;
    }
    return true;
}

bool Tabella::Write(char *fname)
{
    FILE *stream;
//    UndoAtom *ua;
    unsigned int i, k;
//    int j;
    int flag = 0;
    char db_id[9];

    for (i = 0; i < 9; i++) db_id[i] = 0;
    if (strlen(dbname) > 8) return false;
    strcpy(db_id,dbname);
    for (i = 0; i < 8; i++) if (db_id[i] == 0) db_id[i] = 1;

    if((stream = fopen(fname, "wb")) == NULL) {
    	fclose(stream);
    	return false;
    }

    if (numblocks == 1) {
       fprintf(stream,"%s","GENTAB_1_");

       for (i = 0; i < 8; i++) fputc(db_id[i], stream);

       for(i = 0; i < size; i++) flag |= fprintf(stream,"%4u",Array[i]);

       fputc(1, stream);
       fprintf(stream,"%s",note.data());
    }
    else {

       fprintf(stream,"%s","GENTAB_3_");

       for (i = 0; i < 8; i++) fputc(db_id[i], stream);

       flag |= fprintf(stream,"%4u",(unsigned char) ((numblocks & 0xFF000000) >> 24));
       flag |= fprintf(stream,"%4u",(unsigned char) ((numblocks & 0x00FF0000) >> 16));

       flag |= fprintf(stream,"%4u",(unsigned char) ((numblocks & 0x0000FF00) >> 8));
       flag |= fprintf(stream,"%4u",(unsigned char) (numblocks & 0x000000FF));

       for (k = 0; k < numblocks; k++) {
          for(i = 0; i < size; i++) flag |= fprintf(stream,"%4u",blocks[k][i]);
          fputc(1, stream);
       }
       for (k = 0; k < numblocks; k++) {

          flag |= fprintf(stream,"%4u",(unsigned char) ((notes[k].length() & 0xFF000000) >> 24));
          flag |= fprintf(stream,"%4u",(unsigned char) ((notes[k].length() & 0x00FF0000) >> 16));
          flag |= fprintf(stream,"%4u",(unsigned char) ((notes[k].length() & 0x0000FF00) >> 8));
          flag |= fprintf(stream,"%4u",(unsigned char) (notes[k].length() & 0x000000FF));

          fputc(1, stream);
          fprintf(stream,"%s",notes[k].data());
          fputc(1, stream);
       }
    }

    if (EOF == fclose(stream)) return false;
    if ((flag & EOF) == EOF) return false;

//    for (j = undolist->Count - 1; j >= 0; j--) {
//       ua = (UndoAtom*) undolist->Items[j];
//       delete ua;
//    }
//    undolist->Clear();

    return true;
}

unsigned char Tabella::ReadByte(FILE *stream)
{
    int a,b,c,d;
    a = fgetc(stream);
    b = fgetc(stream);
    c = fgetc(stream);
    d = fgetc(stream);
    if (a != ' ' &&  a != '-') {
    	rflag = 1;
        return 0;
    }
    if (a == '-') a = -1;
    else a = 1;
    if (b == '-') a = -1;
    if (b == ' ' || b == '-') b = 0;
    else b = b - '0';
    if (c == '-') a = -1;
    if (c == ' ' || c == '-') c = 0;
    else c = c - '0';
    if (d == ' ' || d == '-') {
    	rflag = 1;
        return 0;
    }
    else d = d - '0';
    if (d < 0 || d > 9) {

    	rflag = 1;
        return 0;
    }
    a = a * (b*100 + c*10 + d);
    if ( a > 255 ) {
    	rflag = 1;
        return 0;
    }
    if ( a < 0 ) {
    	return (unsigned char) (0x000000FF & ((0x000000FF & ~a) + 1));
    }
    return (unsigned char)a;
}

bool Tabella::Read(const char *fname)
{
    FILE *stream;
//    UndoAtom *ua;
    char hdr[10];
    char db_id[9];
    char buff[10];
    unsigned int i, k;
//    int j;

    rflag = 0;
    if((stream = fopen(fname, "rb")) == NULL) {
    	fclose(stream);
    	return false;
    }
    for (i = 0; i < 9; i++) hdr[i] = (char) fgetc(stream);
    hdr[9] = 0;

    if (strcmp("GENTAB_1_",hdr) == 0) {

       for (i = 0; i < 8; i++) db_id[i] = (char) fgetc(stream);
       for (i = 0; i < 8; i++) if(db_id[i] == 1) db_id[i] = 0;
       db_id[8] = 0;
       if (strcmp(dbname,db_id) != 0) {
    	  fclose(stream);
       return false;
       }

//       for (j = undolist->Count - 1; j >= 0; j--) {
//          ua = (UndoAtom*) undolist->Items[j];
//          delete ua;
//       }
//       undolist->Clear();

       for(i = 0; i < size; i++) blocks[currblock/*0*/][i] = ReadByte(stream);

       note = "";
       if (fgetc(stream) != 1) {
          return false;
          }
       while (!feof(stream)) {
          buff[0] = (char) fgetc(stream);
          buff[1] = 0;
          if (feof(stream)) break;
		  note += Q3CString(buff);
       }
       notes[currblock/*0*/] = note;
    }
    else if (strcmp("GENTAB_3_",hdr) == 0) {

       for (i = 0; i < 8; i++) db_id[i] = (char) fgetc(stream);
       for (i = 0; i < 8; i++) if(db_id[i] == 1) db_id[i] = 0;
       db_id[8] = 0;
       if (strcmp(dbname,db_id) != 0) {
    	  fclose(stream);
    	  return false;
       }

//       for (j = undolist->Count - 1; j >= 0; j--) {
//          ua = (UndoAtom*) undolist->Items[j];
//          delete ua;
//       }
//       undolist->Clear();

       unsigned int nblk;
       nblk = ReadByte(stream);
       nblk = nblk * 256 + ReadByte(stream);
       nblk = nblk * 256 + ReadByte(stream);
       nblk = nblk * 256 + ReadByte(stream);

       if (nblk != numblocks) {
    	  fclose(stream);
       return false;
       }

       for (k = 0; k < numblocks; k++) {
          for(i = 0; i < size; i++) blocks[k][i] = ReadByte(stream);
          if (fgetc(stream) != 1) {
              return false;
              }
       }

       note = "";

       for (k = 0; k < numblocks; k++) {
          unsigned int slen;
          slen = ReadByte(stream);
          slen = slen * 256 + ReadByte(stream);
          slen = slen * 256 + ReadByte(stream);
          slen = slen * 256 + ReadByte(stream);
          if (fgetc(stream) != 1) {
              return false;
              }
          notes[k] = "";
          for(i = 0; i < slen; i++) {
             buff[0] = (char) fgetc(stream);
             buff[1] = 0;
			 notes[k] += Q3CString(buff);
          }
          if (fgetc(stream) != 1) {
              return false;
              }
       }
       note = notes[currblock];
    }
    else {
    	fclose(stream);
    	return false;
    }

    if(EOF == fclose(stream)) {
        return false;
        }
    if (rflag) {
        return false;
        }

    return true;
}

bool Tabella::Send(portdef aport)
{
//    char id[10];
//    unsigned long address;
//    unsigned int len;
//    unsigned char packet[RXBFSIZE];
//
//    ComPort acom(aport);
//    if (acom.GetStatus()) return false;
//    if (!acom.SendReset()) return false;
//
//    if (!acom.GetMappaId(id)) return false;
//    if (strcmp(dbname,id)) return false;
//
//
//    TTrasfProgr *trpr = new TTrasfProgr(Application->Owner);
//    trpr->Caption = "Computer -> ECU";
//    trpr->Show();
//    trpr->ProgressBar1->Max = size;
//    trpr->ProgressBar1->Position = 0;
//    Application->ProcessMessages();
//
//    for (unsigned int k = 0; k < numblocks; k++) {
//
//       trpr->Caption = "Computer -> ECU : Block " + IntToStr(k + 1) + " / " + IntToStr(numblocks);
//       address = 0;
//
//       while (address < size) {
//
//          trpr->ProgressBar1->Position = address;
//          Application->ProcessMessages();
//
//    	  if ((size - address) >= (unsigned)aport.pklen) len = aport.pklen;
//          else len = size - address;
//          for(unsigned int i = 0; i < len; i++) {
//             packet[i] = blocks[k][address + i];
//          }
//          if (!acom.SendPacket(address + displarray[k], len, packet)) {
//             trpr->Close();
//             trpr->Free();
//             return false;
//          }
//    	  address += aport.pklen;
//       }
//    }
//
//    trpr->Close();
//    trpr->Free();
//
//   int mb_res = MessageBox(Main->Handle,"Password ?",
//                           "Password...",
//                           MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON1);
//   if (mb_res == IDYES) {
//      TPasswordForm *pf = new TPasswordForm(Application);
//      pf->ShowModal();
//      Q3CString pw =  pf->PasswordMaskEdit->Text;
//      pf->Free();
//      if (!acom.SendPassword(pw)) return false;
//   }
//
//   if (acom.GetStatus())
//      return false;
//   return true;
  return false;
}

bool Tabella::Receive(portdef aport)
{
//    unsigned long address;
//    unsigned int len;
//    unsigned char packet[RXBFSIZE];
//    int j;
//    UndoAtom *ua;
//
//
//    ComPort acom(aport);
//    if (acom.GetStatus()) return false;
//
//    address = 0;
//    if (!acom.SendReset()) return false;
//
//    for (j = undolist->Count - 1; j >= 0; j--) {
//        ua = (UndoAtom*) undolist->Items[j];
//        delete ua;
//    }
//    undolist->Clear();
//
//    TTrasfProgr *trpr = new TTrasfProgr(Application->Owner);
//    trpr->Caption = "ECU -> Computer";
//    trpr->Show();
//    trpr->ProgressBar1->Max = size;
//    trpr->ProgressBar1->Position = 0;
//    Application->ProcessMessages();
//
//    for (unsigned int k = 0; k < numblocks; k++) {
//
//       trpr->Caption = "ECU -> Computer : Block " + IntToStr(k + 1) + " / " + IntToStr(numblocks);
//       address = 0;
//
//       while (address < size) {
//
//          trpr->ProgressBar1->Position = address;
//          Application->ProcessMessages();
//
//    	  if ((size - address) >= (unsigned)aport.pklen) len = aport.pklen;
//          else len = size - address;
//          if (!acom.ReceivePacket(address + displarray[k], len, packet)) {
//             trpr->Close();
//             trpr->Free();
//             return false;
//          }
//          for(unsigned int i = 0; i < len; i++) {
//             blocks[k][address + i] = packet[i];
//          }
//    	  address += aport.pklen;
//       }
//    }
//
//    trpr->Close();
//    trpr->Free();
//
//    if (acom.GetStatus()) return false;
//    return true;
  return false;
}

int Tabella::GetSVal(int nbytes, int addr)
{
    if (nbytes == 1) {
    	return (signed char)GetVal(nbytes, addr);
    }
    else if (nbytes == 2) {
    	return (short)GetVal(nbytes, addr);
    }
    else if (nbytes == 4) {
        return (int)GetVal(nbytes, addr);
    }
    else throw ETabella("Bad dimension");
}

unsigned int Tabella::GetVal(int nbytes, int addr)
{
    unsigned int aval;

    if (addr < 0) throw ETabella("Out of range");
    if (addr >= (int)size) throw ETabella("Out of range");
    if (nbytes == 1) {
    	aval = Array[addr];
    }
    else if (nbytes == 2) {
    	aval = Array[addr];
    	aval = aval * 256 + Array[addr+1];
    }
    else if (nbytes == 4) {
    	aval = Array[addr];
    	aval = aval * 256 + Array[addr+1];
    	aval = aval * 256 + Array[addr+2];
    	aval = aval * 256 + Array[addr+3];
    }
    else throw ETabella("Bad dimension");
    return aval;
}

void Tabella::SetSVal(int nbytes, int addr, int val, bool undo)
{
    if (nbytes == 1) {
    	SetVal(nbytes, addr, ((unsigned int)val) & 0x000000FF, undo);
    }

    else if (nbytes == 2) {
    	SetVal(nbytes, addr, ((unsigned int)val) & 0x0000FFFF, undo);
    }
    else if (nbytes == 4) {
    	SetVal(nbytes, addr, (unsigned int) val, undo);

    }
    else throw ETabella("Bad dimension");
}

void Tabella::SetVal(int nbytes, int addr, unsigned int val, bool undo)
{
    unsigned int prev;
//    UndoAtom *ua;

    prev = GetVal(nbytes, addr);
//    if (undo && prev != val) {
//       ua = new UndoAtom;
//       ua->nbytes = (unsigned char) nbytes;
//       ua->addr = addr;
//       ua->val = prev;
//       undolist->Add(ua);
//    }
    if (addr < 0) throw ETabella("Out of range");
    if (addr >= (int)size) throw ETabella("Out of range");
    if (nbytes == 1) {
    	Array[addr] = (unsigned char)val;
    }
    else if (nbytes == 2) {
    	Array[addr] = (unsigned char)(val / 256);
    	Array[addr+1] = (unsigned char)(val % 256);
    }
    else if (nbytes == 4) {
    	Array[addr] = (unsigned char)(val / 16777216);
    	Array[addr+1] = (unsigned char)((val & 0x00FF0000) / 65536);

    	Array[addr+2] = (unsigned char)((val & 0x0000FF00) / 256);
    	Array[addr+3] = (unsigned char)(val % 256);
    }
    else throw ETabella("Bad dimension");
}

void Tabella::UndoClr()
{
//    UndoAtom *ua;
//
//    for (int j = undolist->Count - 1; j >= 0; j--) {
//        ua = (UndoAtom*) undolist->Items[j];
//        delete ua;
//    }
//    undolist->Clear();
}

void Tabella::Undo()
{
//    unsigned int val;
//    int nbytes;
//    int addr;
//    UndoAtom *ua;
//
//rptundo:
//
//    if (undolist->Count == 0) return;
//
//    ua = (UndoAtom*) undolist->Last();
//    nbytes = ua->nbytes;
//    addr = ua->addr;
//    val = ua->val;
//    undolist->Remove(ua);
//    delete ua;
//
//    if (nbytes == 199) {
//       undostate = 1;
//       goto exitchk;
//    }
//    if (nbytes == 99) {
//       undostate = 0;
//       return;
//    }
//
//    if (addr < 0) throw ETabella("Out of range");
//    if (addr >= (int)size) throw ETabella("Out of range");
//    if (nbytes == 1) {
//    	Array[addr] = (unsigned char)val;
//    }
//    else if (nbytes == 2) {
//    	Array[addr] = (unsigned char)(val / 256);
//    	Array[addr+1] = (unsigned char)(val % 256);
//    }
//    else if (nbytes == 4) {
//    	Array[addr] = (unsigned char)(val / 16777216);
//    	Array[addr+1] = (unsigned char)((val & 0x00FF0000) / 65536);
//    	Array[addr+2] = (unsigned char)((val & 0x0000FF00) / 256);
//    	Array[addr+3] = (unsigned char)(val % 256);
//    }
//    else throw ETabella("Bad dimension");
//
//exitchk:
//
//    if (undostate == 1) goto rptundo;
}

int Tabella::UndoCount()
{
//    return  undolist->Count;
    return -1;
}

void Tabella::StartUndoBlock()
{
//    UndoAtom *ua;
//    ua = new UndoAtom;
//    ua->nbytes = 99;
//    ua->addr = 0xFFFFFFFF;
//    ua->val = 0;
//    undolist->Add(ua);
}

void Tabella::EndUndoBlock()
{
//    UndoAtom *ua;
//    ua = new UndoAtom;
//    ua->nbytes = 199;
//    ua->addr = 0xFFFFFFFF;
//    ua->val = 0;
//    undolist->Add(ua);
}

Q3CString Tabella::GetNote()
{
   return note;
}

void Tabella::SetNote(Q3CString notestr)
{
   note = notestr;
   notes[currblock] = note;
}

void Tabella::SetCurrBlock(unsigned int cb)
{
   if (cb >= numblocks) throw ETabella("Block number NOT valid");
   currblock = cb;
   rflag = 0;
   note = notes[currblock];
   displacement = displarray[currblock];
   Array = blocks[currblock];

//   UndoAtom *ua;
//   int j;
//   for (j = undolist->Count - 1; j >= 0; j--) {
//        ua = (UndoAtom*) undolist->Items[j];
//        delete ua;
//   }
//   undolist->Clear();
}

unsigned int Tabella::GetCurrBlock()
{
   return currblock;
}

void Tabella::ClearAll()
{
//    UndoAtom *ua;
    unsigned int i, k;

    for(k = 0; k < numblocks; k++) {
       for(i = 0; i < size; i++) blocks[k][i] = 0;
       notes[k] = 0;
    }

    rflag = 0;
    note = "";

//    int j;
//    for (j = undolist->Count - 1; j >= 0; j--) {
//        ua = (UndoAtom*) undolist->Items[j];
//        delete ua;
//    }
//    undolist->Clear();
}

void Tabella::UpdateAllWithBlock(unsigned int n)
{
    unsigned int i, k;

    if (n >= numblocks)  throw ETabella("block number/1");
    for(k = 0; k < numblocks; k++) {
       if (n == k) continue;
       for(i = 0; i < size; i++) blocks[k][i] = blocks[n][i];
    }
}

void Tabella::BlockCopy(unsigned int dest, unsigned int source)
{
    if (dest >= numblocks) throw ETabella("block number/1");
    if (source >= numblocks) throw ETabella("block number/2");
    if (source == dest) throw ETabella("block number/3");

    for(unsigned int i = 0; i < size; i++) blocks[dest][i] = blocks[source][i];
}

void Tabella::SegmentCopy(unsigned int dest, unsigned int source,
        unsigned int start, unsigned int len)
{
    unsigned int i, p;

    if (dest >= numblocks) throw ETabella("block number/1");
    if (source >= numblocks) throw ETabella("block number/2");
    if (source == dest) throw ETabella("block number/3");

    for(i = 0; i < len; i++) {
       p = start + i;
       if (p >= size) throw ETabella("block number/4");
       blocks[dest][p] = blocks[source][p];
    }
}

//---------------------------------------------------------------------------
unsigned char* Tabella::ConvertTabToA20(unsigned char* destBuff,unsigned long Size)
{

if (!destBuff || !Size) return false;
Q3CString Carat(""),NumBytes("");
int RecordNumber = this->GetSize()  /  16;
bool ExistLastRecord = ((this->GetSize() % 16) != 0);
unsigned int LocStart = 0;
for (int i = 0; i < RecordNumber; i++)
    {
    unsigned char Chks;
	Q3CString Temp(""),t("");
    if ((this->GetDisplacement() + LocStart) < 0x10000)
       {
       Carat = "S1";
       NumBytes = "13";
       Chks = 0x13;
	   Q3CString P;
	   Q3CString t;
       P.sprintf("%4.4x",(this->GetDisplacement() + LocStart));
       Temp += Carat;
       Temp += NumBytes;
       Temp += P;
       Chks += (unsigned char) wtddahextol(P.mid(0,2));
       Chks += (unsigned char) wtddahextol(P.mid(2,2));
       }
    else
       {
       Carat = "S2";
       NumBytes = "14";
       Chks = 0x14;
	   Q3CString P("");
       P.sprintf("%6.6x",(this->GetDisplacement() + LocStart));
       Temp += Carat;
       Temp += NumBytes;
       Temp += P;
       Chks += (unsigned char) wtddahextol(P.mid(0,2));
       Chks += (unsigned char) wtddahextol(P.mid(2,2));
       Chks += (unsigned char) wtddahextol(P.mid(4,2));
       }
    for (int j = 0; j < 16; j++)
        {
        Chks += this->GetArrayElement(LocStart + j);
        t.sprintf("%2.2x",(this->GetArrayElement(LocStart + j)));
        Temp += t;
        }
    Chks =(unsigned char) ~Chks;
    t.sprintf("%2.2x",Chks);
    Temp += t;
    Temp += "\r\n";
    if (strlen((const char*)destBuff) > Size) return NULL;
    strcpy((char*)(destBuff),Temp);
    destBuff += Temp.length();
    LocStart += 16;
    //fprintf(stderr,"%s",Temp.data());
    }
/* ultimo record*/
if (ExistLastRecord)
   {
   unsigned char Nbytes = (unsigned char) (this->GetSize() - LocStart);
   unsigned char Chks;
   Q3CString Temp(""),t("");
   if ((this->GetDisplacement() + LocStart) < 0x10000)
      {
      Carat = "S1";
      t.sprintf("%2.2x",(Nbytes + 3));
      NumBytes = t;
      Chks = Nbytes;
	  Q3CString P("");
       P.sprintf("%4.4x",(this->GetDisplacement() + LocStart));
      Temp += Carat;
      Temp += NumBytes;
      Temp += P;
      Chks += (unsigned char) wtddahextol(P.mid(0,2));
      Chks += (unsigned char) wtddahextol(P.mid(2,2));
      }
   else
      {
      Carat = "S2";
      t.sprintf("%2.2x",(Nbytes + 3));
      NumBytes = t;
      Chks = Nbytes;
	  Q3CString P("");
       P.sprintf("%6.6x",(this->GetDisplacement() + LocStart));
      Temp += Carat;
      Temp += NumBytes;
      Temp += P;
      Chks += (unsigned char) wtddahextol(P.mid(0,2));
      Chks += (unsigned char) wtddahextol(P.mid(2,2));
      Chks += (unsigned char) wtddahextol(P.mid(4,2));
      }
   for (unsigned int j = LocStart; j < this->GetSize(); j++) {
     Chks += this->GetArrayElement(/*LocStart +*/ j);
     t.sprintf("%2.2x",(this->GetArrayElement(LocStart + j)));
     Temp += t;
     }
   Chks =(unsigned char) ~Chks;
   t.sprintf("%2.2x",Chks);
   Temp += t;
   Temp += "\r\n";
   if (strlen((const char*)destBuff) > Size) return NULL;
    strcpy((char*)(destBuff),Temp);
    destBuff += Temp.length();
   }
*destBuff = '\0';
return destBuff;
}
//---------------------------------------------------------------------------

