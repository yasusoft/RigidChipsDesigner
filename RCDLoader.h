//---------------------------------------------------------------------------

#ifndef RCDLoaderH
#define RCDLoaderH
//---------------------------------------------------------------------------
class TRigidChip;
class TRigidChipCore;
class TRCDLoader
{
private:
        void skipsp(int fp);
        AnsiString readto(int fp, char to);
        bool skipcomment(int fp);
        AnsiString readkey(int fp);
        TRigidChip* procval(int fp, TRigidChipCore *core);
        TRigidChip* prockey(int fp, TRigidChipCore *core);
        TRigidChip* procbody(int fp, TRigidChip *core);
public:
        AnsiString ErrorMessage;
        TRigidChipCore* Load(AnsiString filename);
};
//---------------------------------------------------------------------------
#endif
