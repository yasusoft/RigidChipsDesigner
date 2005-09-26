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
        AnsiString readcomment(int fp);
        AnsiString readkey(int fp);
        TRigidChip* procval(int fp, TRigidChipCore *core);
        TRigidChip* prockey(int fp, TRigidChipCore *core);
        TRigidChip* procbody(int fp, TRigidChip *core);

        void skipsp(TStringStream *stream);
        AnsiString readto(TStringStream *stream, char to);
        bool skipcomment(TStringStream *stream);
        AnsiString readkey(TStringStream *stream);
public:
        AnsiString ErrorMessage;
        TRigidChipCore* Load(AnsiString filename);

        TRigidChip* StringToChip(AnsiString str, TRigidChip *addto = NULL);
};
//---------------------------------------------------------------------------
#endif
