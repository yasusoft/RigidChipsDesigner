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
__published:	// IDE �Ǘ��̃R���|�[�l���g
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
private:	// ���[�U�[�錾
        TRigidChipCore *Core;
public:		// ���[�U�[�錾
        __fastcall TFormOptionSave(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormOptionSave *FormOptionSave;
//---------------------------------------------------------------------------
#endif
