//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitOptionSave.h"
#include "Unit1.h"
#include "RigidChip.h"
#include "RCDLoader.h"
#include "RCDSaver.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormOptionSave *FormOptionSave;
//---------------------------------------------------------------------------
__fastcall TFormOptionSave::TFormOptionSave(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionSave::FormCreate(TObject *Sender)
{
  Core = Form1->RCDLoader->LoadFromString(MemoPreview->Text);
  /*
  Core = new TRigidChipCore;
  Core->MemoModel =
    "Preview Model\r\n"
    "This is ModelMemo"
  ;
  Core->Variables["VAR1"] = new TRigidChipsVariable(Core, "default=123,min=0,max=1234");
  Core->Variables["VAR2"] = new TRigidChipsVariable(Core, "default=123,min=0,max=1234");
  Core->AddKey("0", new TRigidChipsKey(Core, "VAR1", "step=5"));
  Core->AddKey("0", new TRigidChipsKey(Core, "VAR2", "step=5"));
  Core->AddKey("1", new TRigidChipsKey(Core, "VAR1", "step=-5"));
  Core->AddKey("1", new TRigidChipsKey(Core, "VAR2", "step=-5"));
  TRigidChip *chip1 = new TRigidChipChip;
  chip1->SetOptions("angle=90,name=hoge");
  chip1->Direction = rdSouth;
  chip1->MemoChip =
    "chip for hoge\r\n"
    "this is ChipMemo"
  ;
  Core->AddSubChip(chip1);
  TRigidChip *chip2 = new TRigidChipCowl;
  chip2->SetOptions("angle=VAR1");
  chip2->Direction = rdNorth;
  chip2->MemoChip = "cooooowl";
  chip1->AddSubChip(chip2);
  TRigidChip *chip3 = new TRigidChipJet;
  chip3->SetOptions("power=VAR2,option=1");
  chip3->Direction = rdWest;
  chip3->MemoChip =
    "jeeeeet\r\n"
    "balooooon"
  ;
  chip1->AddSubChip(chip3);
  */

  CheckSpaceBlockType->Checked = Form1->RCDSaver->optSpaceAfterBlockType;
  CheckNewLineBlockType->Checked = Form1->RCDSaver->optNewLineAfterBlockType;
  CheckSpaceOptions->Checked = Form1->RCDSaver->optSpaceAfterOptions;
  CheckNewLineOptions->Checked = Form1->RCDSaver->optNewLineAfterOptions;
  CheckSpaceComma->Checked = Form1->RCDSaver->optSpaceAfterComma;
  CheckSubNewLine->Checked = Form1->RCDSaver->optNoSubNoNewLine;
  if (Form1->RCDSaver->optTabSpaces == 0)
    RadioTab->Checked = true;
  else
  {
    RadioSpace->Checked = true;
    EditSpace->Text = IntToStr(Form1->RCDSaver->optTabSpaces);
  }
  CheckSpaceBlockTypeClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionSave::FormDestroy(TObject *Sender)
{
  delete Core;
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionSave::CheckSpaceBlockTypeClick(TObject *Sender)
{
  bool * const opts[] = {
    &Form1->RCDSaver->optSpaceAfterBlockType,
    &Form1->RCDSaver->optNewLineAfterBlockType,
    &Form1->RCDSaver->optSpaceAfterOptions,
    &Form1->RCDSaver->optNewLineAfterOptions,
    &Form1->RCDSaver->optSpaceAfterComma,
    &Form1->RCDSaver->optNoSubNoNewLine,
  };
  if (Sender)
  {
    int i = ((TCheckBox*)Sender)->Tag;
    *opts[i] = ((TCheckBox*)Sender)->Checked;
  }
  MemoPreview->Text = Form1->RCDSaver->Save("", Core);
}
//---------------------------------------------------------------------------
void __fastcall TFormOptionSave::RadioTabClick(TObject *Sender)
{
  if (RadioTab->Checked)
    Form1->RCDSaver->optTabSpaces = 0;
  else
    Form1->RCDSaver->optTabSpaces = EditSpace->Text.ToIntDef(0);
  MemoPreview->Text = Form1->RCDSaver->Save("", Core);
}
//---------------------------------------------------------------------------

