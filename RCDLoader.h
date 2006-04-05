//---------------------------------------------------------------------------

#ifndef RCDLoaderH
#define RCDLoaderH
//---------------------------------------------------------------------------
class TRigidChip;
class TRigidChipCore;
class TRCDLoader
{
private:
        void skipsp(TStream *stream);
        AnsiString readto(TStream *stream, char to);
        AnsiString readcomment(TStream *stream);
        AnsiString readkey(TStream *stream);

        TRigidChip* readchips(TStream *stream, TRigidChip *addto);
        TRigidChip* procval(TStream *stream, TRigidChipCore *core);
        TRigidChip* prockey(TStream *stream, TRigidChipCore *core);
        TRigidChip* procbody(TStream *stream, TRigidChip *core);
        TRigidChipCore* load(TStream *stream);
public:
        AnsiString ErrorMessage;
        TRigidChipCore* LoadFromFile(AnsiString filename);
        TRigidChipCore* LoadFromString(AnsiString str);

        TRigidChip* StringToChip(AnsiString str, TRigidChip *addto);
};
//---------------------------------------------------------------------------
#endif
