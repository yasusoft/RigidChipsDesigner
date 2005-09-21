//---------------------------------------------------------------------------

#ifndef RCDSaverH
#define RCDSaverH
//---------------------------------------------------------------------------
class TRigidChip;
class TRigidChipCore;
class TRCDSaver
{
public:
        bool optSpaceAfterBlockType;
        bool optNewLineAfterBlockType;
        bool optSpaceAfterOptions;
        bool optNewLineAfterOptions;
        bool optSpaceAfterComma;
        bool optNoSubNoNewLine;
        int optTabSpaces;

        AnsiString ErrorMessage;
        AnsiString ChipToString(AnsiString tabs, TRigidChip *chip);
        void Save(AnsiString filename, TRigidChipCore *core);

        TRCDSaver();
};
//---------------------------------------------------------------------------
#endif
