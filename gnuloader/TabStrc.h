//---------------------------------------------------------------------------
#include "arnComPort.h"
#include <stdio.h>
#include "Containers.h"

#ifndef TabStrcH
#define TabStrcH

// costanti relazioni

#define R_READONLY			0
#define R_MIN_X				1
#define R_MAX_X				2
#define R_MIN_Y				3
#define R_MAX_Y				4
#define R_MIN_Z				5
#define R_MAX_Z				6
#define R_ONBIT_X			7
#define R_ONBIT_Y			8
#define R_ONBIT_Z			9
#define R_ARRAY_X			10
#define R_ARRAY_Y			11
#define R_ONBIT_X_A			12
#define R_ONBIT_Y_A			13
#define R_ONBIT_X_V			14
#define R_ONBIT_Y_V			15
#define R_ONBIT_Z_V			16
#define R_FLOOR_X			17
#define R_CEIL_X			18
#define R_FLOOR_Y			19
#define R_CEIL_Y			20
#define R_FLOOR_Z			21
#define R_CEIL_Z			22

//---------------------------------------------------------------------------

class EElement : public wtddException {
    public:
	EElement(QString Mess) : wtddException(Mess) { }
} ;         // Exception class

class ETabella : public wtddException {
    public:
	ETabella(QString Mess) : wtddException(Mess) { }
} ;			// Exception class

class Tabella;
class Table;

bool ChkBitN(unsigned int numb, int pos);

enum elemtype{undef, table, map, variable};

class Element {
  protected:
    elemtype id;
    QString name;
    QString memo;
    bool readonly;
    QString xname_on;
    QString xname_off;
    double xmin_on;
    double xmin_off;
    double xmax_on;
    double xmax_off;
    int xbpos;
    QString yname_on;
    QString yname_off;
    double ymin_on;
    double ymin_off;
    double ymax_on;
    double ymax_off;
    int ybpos;
    QString zname_on;
    QString zname_off;
    double zmin_on;
    double zmin_off;
    double zmax_on;
    double zmax_off;
    int zbpos;
    unsigned int indxon;
    unsigned int indxoff;
    unsigned int indyon;
    unsigned int indyoff;

    unsigned int xvmaxon;
    unsigned int xvminon;
    unsigned int xvmaxoff;
    unsigned int xvminoff;
    unsigned int yvmaxon;
    unsigned int yvminon;
    unsigned int yvmaxoff;
    unsigned int yvminoff;
    unsigned int zvmaxon;
    unsigned int zvminon;
    unsigned int zvmaxoff;
    unsigned int zvminoff;



  public:
    Element();
    virtual ~Element(void) {;}
    elemtype GetId() { return id; }
    QString GetName() { return name; }
    QString GetMemo() { return memo; }
    void SetReadOnly() { readonly = true; }
    bool GetReadOnly() { return readonly; }
    virtual  void SetOB(int xyz,int bitpos,QString onname, double onmin, double onmax,
    QString offname, double offmin, double offmax) = 0;
    virtual  void SetOBT(int xyz,int bitpos,unsigned int idx1, unsigned int idx2) = 0;
    virtual  void SetOBV(int xyz,int bitpos,unsigned int idx1, unsigned int idx2,
      unsigned int idx3, unsigned int idx4,QString onname,QString offname) = 0;
    virtual  void RunOB(int xyz, unsigned int number) = 0;
    virtual  void RunOBT(int xyz, unsigned int number, Element **earr, Tabella *ctab) = 0;
    virtual  void RunOBV(int xyz, unsigned int number, Element **earr, Tabella *ctab) = 0;
    virtual  void RunMinMax(int mode,double value) = 0;
    virtual  void RunArray(int mode,Table *el,Tabella *ctab) = 0;
    virtual  double GetMinMax(Tabella *ctab) = 0;
    virtual  unsigned int GetBinImg(Tabella *ctab) = 0;
    virtual  int NumDecim() = 0;
    virtual void RunLimits(int mode, Tabella *ctab) = 0;
    virtual void SetLimits(double val,int mode) = 0;
};

class Table : public Element {
  protected:
    bool segno;
    int gruppo;
    int addr;
    int size;
    int bytes;
    QString xname;
    double xmin;
    double xmax;
    QString yname;
    double ymin;
    double ymax;
    double visymin;
    double visymax;
    double *xpt;
    double supy;
    double infy;

  public:
  	Table(int grp,
          int caddr,
          QString tname,
          int tsize,
          int tbytes,
          QString txname,
          double txmin,
          double txmax,
          QString tyname,
          double tymin,
          double tymax,
          QString cmemo);
  ~Table();
  int GetSize() { return size; }
  double GetXVal(int i);
  double GetYVal(Tabella *ctab, int i);
  double GetXZeroN(void);
  double GetYZeroN(void);
  double GetYZeroD(void);
  double GetXValN(int i);
  double GetYValN(Tabella *ctab, int i);
  double GetYValD(Tabella *ctab, int i);
  void SetYVal(Tabella *ctab, double val, int i, bool undo = true);
  void SetYValN(Tabella *ctab, double val, int i);
  void SetYValD(Tabella *ctab, double val, int i);
  void SetXVal(double val, int i);
  QString GetXName(){ return xname; }
  QString GetYName(){ return yname; }
  double TabMin() { return ymin; }
  double TabMax() { return ymax; }
  void SetTabMin(double v) {visymin = v;}
  void SetTabMax(double v) {visymax = v;}
  bool IsDrawable();
  virtual void SetOB(int xyz,int bitpos,QString onname, double onmin, double onmax,
  		QString offname, double offmin, double offmax);
  virtual  void SetOBT(int xyz,int bitpos,unsigned int idx1, unsigned int idx2);

  virtual  void SetOBV(int xyz,int bitpos,unsigned int idx1, unsigned int idx2,
    	unsigned int idx3, unsigned int idx4,QString onname,QString offname);
  virtual  void RunOB(int xyz, unsigned int number);
  virtual  void RunOBT(int xyz, unsigned int number, Element **earr, Tabella *ctab);
  virtual  void RunOBV(int xyz, unsigned int number, Element **earr, Tabella *ctab);
  virtual  void RunMinMax(int mode,double value);
  virtual  void RunArray(int mode,Table *el,Tabella *ctab);
  virtual  double GetMinMax(Tabella *ctab);
  virtual  unsigned int GetBinImg(Tabella *ctab);
  virtual  int NumDecim();
  virtual void RunLimits(int mode, Tabella *ctab);
  virtual void SetLimits(double val,int mode);

};

class Map : public Element {
  protected:
    bool segno;
    int gruppo;
    int addr;
    int xsize;
    int ysize;
    int bytes;
    QString xname;
    double xmin;
    double xmax;
    QString yname;
    double ymin;
    double ymax;
    QString zname;
    double zmin;
    double zmax;
    double viszmin;
    double viszmax;
    double *xpt;
    double *ypt;
    double supz;
    double infz;

  public:
  	Map(int grp,
    	int caddr,
        QString mname,
        int mxsize,
        int mysize,
        int mbytes,
        QString mxname,
        double mxmin,
        double mxmax,
        QString myname,
        double mymin,
        double mymax,
        QString mzname,
        double mzmin,
        double mzmax,
        QString cmemo);

  ~Map();
  int GetXSize() { return xsize; }
  int GetYSize() { return ysize; }
  double GetXVal(int x);
  double GetYVal(int y);
  double GetZVal(Tabella *ctab, int x, int y);
  double GetXValN(int x);
  double GetYValN(int y);
  double GetZValN(Tabella *ctab, int x, int y);
  double GetZValD(Tabella *ctab, int x, int y);
  void SetZVal(Tabella *ctab, double val, int x, int y, bool undo = true);
  void SetZValN(Tabella *ctab, double val, int x, int y);
  void SetZValD(Tabella *ctab, double val, int x, int y);
  double MapMin() { return zmin; }
  double MapMax() { return zmax; }
  int GetBytes() {return bytes;}
  double XMax() { return xmax; }
  double XMin() { return xmin; }
  double YMax() { return ymax; }
  double YMin() { return ymin; }
  void SetMapMin(double v) {viszmin = v;}
  void SetMapMax(double v) {viszmax = v;}
  QString GetXName(){ return xname; }
  QString GetYName(){ return yname; }
  QString GetZName(){ return zname; }
  bool IsDrawable();
  virtual void SetOB(int xyz,int bitpos,QString onname, double onmin, double onmax,
  		QString offname, double offmin, double offmax);
  virtual  void SetOBT(int xyz,int bitpos,unsigned int idx1, unsigned int idx2);
  virtual  void SetOBV(int xyz,int bitpos,unsigned int idx1, unsigned int idx2,
    	unsigned int idx3, unsigned int idx4,QString onname,QString offname);
  virtual  void RunOB(int xyz, unsigned int number);
  virtual  void RunOBT(int xyz, unsigned int number, Element **earr, Tabella *ctab);
  virtual  void RunOBV(int xyz, unsigned int number, Element **earr, Tabella *ctab);
  virtual  void RunMinMax(int mode,double value);
  virtual  void RunArray(int mode,Table *el,Tabella *ctab);
  virtual  double GetMinMax(Tabella *ctab);
  virtual  unsigned int GetBinImg(Tabella *ctab);
  virtual  int NumDecim();
  virtual void RunLimits(int mode, Tabella *ctab);
  virtual void SetLimits(double val,int mode);
};

class Variable : public Element {
  protected:
    bool segno;
    int gruppo;
    int addr;
    int bytes;
    QString xname;
    double xmin;
    double xmax;
    double supx;
    double infx;

  public:
  	Variable(int grp,
          int caddr,
          QString vname,
          int vbytes,
          QString vxname,
          double vxmin,
          double vxmax,
          QString cmemo);
  ~Variable();
  QString GetVDescr() { return xname; }
  double GetValue(Tabella *ctab);
  double GetMax(Tabella */*ctab*/){ return xmax; }
  double GetMin(Tabella */*ctab*/){ return xmin; }
  void SetValue(Tabella *ctab, double val, bool undo = true);
  void Increase(Tabella *ctab);
  void Decrease(Tabella *ctab);
  virtual void SetOB(int xyz,int bitpos,QString onname, double onmin, double onmax,
  		QString offname, double offmin, double offmax);
  virtual  void SetOBT(int xyz,int bitpos,unsigned int idx1, unsigned int idx2);
  virtual  void SetOBV(int xyz,int bitpos,unsigned int idx1, unsigned int idx2,
    	unsigned int idx3, unsigned int idx4,QString onname,QString offname);
  virtual  void RunOB(int xyz, unsigned int number);
  virtual  void RunOBT(int xyz, unsigned int number, Element **earr, Tabella *ctab);
  virtual  void RunOBV(int xyz, unsigned int number, Element **earr, Tabella *ctab);
  virtual  void RunMinMax(int mode,double value);
  virtual  void RunArray(int mode,Table *el,Tabella *ctab);
  virtual  double GetMinMax(Tabella *ctab);
  virtual  unsigned int GetBinImg(Tabella *ctab);
  bool IsDrawable();
  virtual  int NumDecim();
  virtual void RunLimits(int mode, Tabella *ctab);
  virtual void SetLimits(double val,int mode);
};


////////////////////////////////////////////////////////////////////////////////

struct UndoAtom {               // undo structure
       unsigned char nbytes;
       int addr;
       unsigned int val;
};

////////////////////////////////////////////////////////////////////////////////

class Tabella {

public :

    Tabella(unsigned int tsize, KaArray<unsigned int>& displ, unsigned int nb,const char *name);
    Tabella(const Tabella& atable);
    Tabella& operator =(const Tabella& atable);
    bool operator == (const Tabella& atable);
    ~Tabella();
    bool Write(char *fname);
    bool Read(const char *fname);
    bool Send(portdef aport);
    bool Receive(portdef aport);

    void SetCurrBlock(unsigned int cb);  // multimappa
    unsigned int GetCurrBlock();
    void ClearAll();
    void UpdateAllWithBlock(unsigned int n); // tutti i blocchi come blocco n
    void BlockCopy(unsigned int dest, unsigned int source);
    void SegmentCopy(unsigned int dest, unsigned int source,
        unsigned int start, unsigned int len);

    // si riferiscono al blocco corrente
    void Clear();
    unsigned int GetVal(int nbytes, int addr);
    void SetVal(int nbytes, int addr, unsigned int val, bool undo = true);
    int GetSVal(int nbytes, int addr);
    void SetSVal(int nbytes, int addr, int val, bool undo = true);
    void StartUndoBlock();
    void EndUndoBlock();
    void Undo();
    void UndoClr();
    int UndoCount();
    QString GetNote();
    void SetNote(QString notestr);

    // stand alone

    unsigned int GetSize(void) {return size;}
    unsigned int GetDisplacement(void) {return displacement;}
    unsigned char GetArrayElement(int Index) {return Array[Index];}
    unsigned char* ConvertTabToA20(unsigned char* destBuff,unsigned long Size);

private:

    unsigned char *Array;
    unsigned int size;
    unsigned int displacement;
    int rflag;
    char dbname[20];
    QString note;
//    TList *undolist;
//    int undostate;

    unsigned char **blocks;        // multimap gentab 3
    unsigned int numblocks;
    unsigned int *displarray;
    unsigned int currblock;
    QString *notes;


    unsigned char ReadByte(FILE *stream);
};

#endif
