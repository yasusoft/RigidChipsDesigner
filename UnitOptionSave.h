//---------------------------------------------------------------------------

#ifndef UnitOptionSaveH
#define UnitOptionSaveH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TRigidChipCore;
class TFormOptionSave : public TForm
{
__published:	// IDE 管理のコンポーネント
        TGroupBox *GroupForm;
        TCheckBox *CheckSpaceBlockType;
        TMemo *MemoPreview;
        TCheckBox *CheckNewLineBlockType;
        TCheckBox *CheckSpaceOptions;
        TCheckBox *CheckNewLineOptions;
        TCheckBox *CheckSpaceComma;
        TCheckBox *CheckSubNewLine;
        TButton *ButtonOk;
        TRadioButton *RadioTab;
        TRadioButton *RadioSpace;
        TEdit *EditSpace;
        void __fastcall CheckSpaceBlockTypeClick(TObject *Sender);
        void __fastcall RadioTabClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
private:	// ユーザー宣言
        TRigidChipCore *Core;
public:		// ユーザー宣言
        __fastcall TFormOptionSave(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormOptionSave *FormOptionSave;
//---------------------------------------------------------------------------
#endif
