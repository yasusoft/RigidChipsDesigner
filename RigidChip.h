//---------------------------------------------------------------------------

#ifndef RigidChipH
#define RigidChipH
//---------------------------------------------------------------------------
#include <GL/gl.h>
#include <GL/glu.h>
//---------------------------------------------------------------------------
class TRigidChip;
class TRigidChipCore;
//---------------------------------------------------------------------------
class TRigidChipsVariable
{
private:
        TRigidChipCore *core;
        TStringList *FOptions;
        double FValue, def, min, max, step;
        bool fstep;
        void SetValue(double v);
public:
        __property TStringList *Options = {read=FOptions};

        __property double Value = {read=FValue, write=SetValue};
        void Act();

        TRigidChipsVariable(TRigidChipCore *c, AnsiString opt);
        ~TRigidChipsVariable();
};
//---------------------------------------------------------------------------
typedef enum { rdCore, rdNorth, rdEast, rdWest, rdSouth } TRigidChipsDirection;
//---------------------------------------------------------------------------
class TRigidChip
{
private:
        TStringList *FOptions;
        TList *ChipsList;

        int GetSubChipsCount();
        TRigidChip *GetSubChip(int i);
protected:
        void translate();
        void rotate();
        void sphere(float r);
        void setcolor();
        void rotatetex();

        TRigidChip();
        ~TRigidChip();
public:
        TRigidChipsDirection Direction;

        __property TStringList *Options = {read=FOptions};
        void SetOptions(AnsiString opt);

        virtual void Draw();
        virtual void Act();
        virtual void Delete(){ delete this; }

        TRigidChipCore *Core;
        TRigidChip *Parent;
        __property int SubChipsCount = {read=GetSubChipsCount};
        __property TRigidChip *SubChips[int i] = {read=GetSubChip};
        void AddSubChip(TRigidChip *chip);
};
//---------------------------------------------------------------------------
class TRigidChipCore : public TRigidChip
{
private:
        TStringList *VariableList;
        TRigidChipsVariable* GetVariable(AnsiString name);
public:
        __property TRigidChipsVariable *Variables[AnsiString name] = {read=GetVariable};
        void AddVariable(AnsiString name, TRigidChipsVariable *var);

        bool StrToDouble(AnsiString str, double *d);

        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
        TRigidChipCore();
        ~TRigidChipCore();
};
//---------------------------------------------------------------------------
class TRigidChipChip : public TRigidChip
{
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
class TRigidChipFrame : public TRigidChipChip
{
public:
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
//---------------------------------------------------------------------------
class TRigidChipWheel : public TRigidChip
{
private:
        int angle;
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Act();
        virtual void Delete(){ delete this; }
        TRigidChipWheel();
};
//---------------------------------------------------------------------------
class TRigidChipJet : public TRigidChip
{
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
//---------------------------------------------------------------------------
class TRigidChipRudder : public TRigidChip
{
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
class TRigidChipRudderF : public TRigidChipRudder
{
public:
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
//---------------------------------------------------------------------------
class TRigidChipTrim : public TRigidChip
{
public:
        static GLuint Texture;
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
class TRigidChipTrimF : public TRigidChipTrim
{
public:
        virtual void Draw();
        virtual void Delete(){ delete this; }
};
//---------------------------------------------------------------------------
#endif
 