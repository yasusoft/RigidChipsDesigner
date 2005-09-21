//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include "RigidChip.h"
//---------------------------------------------------------------------------
class TRCDLoader;
class TRCDSaver;
//---------------------------------------------------------------------------
class TPaintPanel : public TPanel
{ // OnPaintを実装したパネル
private:
        TNotifyEvent FOnPaint;
        void __fastcall Paint()
        {
          if (FOnPaint)
            FOnPaint(this);
        }
__published:
        __property TNotifyEvent OnPaint = {read=FOnPaint, write=FOnPaint};
public:
        __fastcall TPaintPanel(TComponent *AOwner)
                : TPanel(AOwner)
        {
        }
};
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE 管理のコンポーネント
        TTimer *Timer1;
        TPanel *PaintPanelDummy;
        TOpenDialog *OpenDialog;
        TTimer *TimerReload;
        TColorDialog *ColorDialog;
        TSaveDialog *SaveDialog;
        TMainMenu *MainMenu1;
        TMenuItem *KeyFile;
        TMenuItem *KeyNew;
        TMenuItem *KeyOpen;
        TMenuItem *KeySave;
        TMenuItem *KeySaveAs;
        TMenuItem *KeyFileBar1;
        TMenuItem *KeyExit;
        TMenuItem *KeyEdit;
        TMenuItem *KeyCut;
        TMenuItem *KeyCopy;
        TMenuItem *KeyDelete;
        TMenuItem *KeyTool;
        TMenuItem *KeyRCLoad;
        TMenuItem *KeyAutoReload;
        TMenuItem *KeyOption;
        TMenuItem *KeyOptSave;
        TMenuItem *KeySpaceBlock;
        TMenuItem *KeyNLBlock;
        TMenuItem *KeySpaceOpt;
        TMenuItem *KeyNLOpt;
        TMenuItem *KeySpaceComma;
        TMenuItem *KeyNoSubNoNL;
        TPopupMenu *PopupMenu1;
        TMenuItem *KeyPaste2;
        TMenuItem *KeyOrigin;
        TMenuItem *KeyCut2;
        TMenuItem *KeyCopy2;
        TMenuItem *KeyDelete2;
        TMenuItem *KeyPaste;
        TMenuItem *KeyBar1;
        TPopupMenu *PopupMenuOptions;
        TMenuItem *KeyOptionsColor;
        TMenuItem *KeyOptionsAngle;
        TMenuItem *KeyOptionsSpring;
        TMenuItem *KeyOptionsDamper;
        TMenuItem *KeyOptionsName;
        TMenuItem *KeyOptionsPower;
        TMenuItem *KeyOptionsBrake;
        TMenuItem *KeyOptionsOption;
        TMenuItem *KeyOptionsEffect;
        TMenuItem *KeyOptionsUser1;
        TMenuItem *KeyOptionsUser2;
        TPageControl *PageControl1;
        TTabSheet *TabVal;
        TTabSheet *TabKey;
        TTabSheet *TabBody;
        TListBox *ListNorth;
        TButton *ButtonParent;
        TListBox *ListEast;
        TListBox *ListWest;
        TListBox *ListSouth;
        TLabel *LabelDirection;
        TLabel *LabelType;
        TComboBox *ComboType;
        TComboBox *ComboDirection;
        TValueListEditor *OptionsEditor;
        TEdit *EditKeyTest;
        TCheckBox *CheckBalloon;
        TCheckBox *CheckGhost;
        TCheckBox *CheckCowl;
        TListBox *ListVariables;
        TPanel *PanelVariable;
        TLabel *LabelVarName;
        TEdit *EditVarName;
        TLabel *LabelVarDefault;
        TEdit *EditVarDefault;
        TLabel *LabelVarMin;
        TEdit *EditVarMin;
        TLabel *LabelVarMax;
        TEdit *EditVarMax;
        TLabel *LabelVarStep;
        TEdit *EditVarStep;
        TButton *ButtonVarOk;
        TCheckBox *CheckVarDisp;
        TPanel *PanelVarLabel;
        TButton *ButtonVarDel;
        TButton *ButtonVarColor;
        TLabel *LabelKeyNo;
        TComboBox *ComboKeyNo;
        TLabel *LabelKeyVar;
        TComboBox *ComboKeyVarName;
        TLabel *LabelKeyVarName;
        TLabel *LabelKeyStep;
        TEdit *EditKeyStep;
        TListBox *ListKeyVar;
        TButton *ButtonKeyDel;
        TButton *ButtonKeyOk;
        TCheckBox *CheckAnime;
        TMenuItem *Help1;
        TMenuItem *KeyAbout;
        TMenuItem *KeyConvertCowl;
        TMenuItem *KeyBar2;
        TMenuItem *KeyConvertCowlS;
        TTabSheet *TabScript;
        TPanel *PanelScript;
        TButton *ButtonScriptReset;
        TMemo *MemoScript;
        TListBox *ScriptOut;
        TButton *ButtonScriptStop;
        TPopupMenu *PopupMenuAdd;
        TMenuItem *KeyAddChip;
        TMenuItem *KeyAddFrame;
        TMenuItem *KeyAddWeight;
        TMenuItem *KeyAddWheel;
        TMenuItem *KeyAddRLW;
        TMenuItem *KeyAddJet;
        TMenuItem *KeyAddRudder;
        TMenuItem *KeyAddRudderF;
        TMenuItem *KeyAddTrim;
        TMenuItem *KeyAddTrimF;
        TMenuItem *KeyAddArm;
        TMenuItem *KeyAddCowl;
        TCheckBox *CheckRotate;
        TMenuItem *KeyDelOption;
        TMenuItem *N1;
        TMenuItem *KeyUndo;
        TMenuItem *KeyEditBar1;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall PaintPanelPaint(TObject *Sender);
        void __fastcall PaintPanelMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall PaintPanelMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall PaintPanelMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall PaintPanelResize(TObject *Sender);
        void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall EditKeyTestKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall EditKeyTestKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall TimerReloadTimer(TObject *Sender);
        void __fastcall CheckClick(TObject *Sender);
        void __fastcall OptionsEditorSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
        void __fastcall OptionsEditorEditButtonClick(TObject *Sender);
        void __fastcall OptionsEditorStringsChange(TObject *Sender);
        void __fastcall KeyOpenClick(TObject *Sender);
        void __fastcall KeySaveClick(TObject *Sender);
        void __fastcall KeyNewClick(TObject *Sender);
        void __fastcall KeySaveAsClick(TObject *Sender);
        void __fastcall KeyExitClick(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall KeyRCLoadClick(TObject *Sender);
        void __fastcall KeyAutoReloadClick(TObject *Sender);
        void __fastcall ComboDirectionChange(TObject *Sender);
        void __fastcall KeySaveOptionClick(TObject *Sender);
        void __fastcall KeyOptSaveClick(TObject *Sender);
        void __fastcall KeyCutClick(TObject *Sender);
        void __fastcall KeyEditClick(TObject *Sender);
        void __fastcall KeyCopyClick(TObject *Sender);
        void __fastcall KeyDeleteClick(TObject *Sender);
        void __fastcall KeyOriginClick(TObject *Sender);
        void __fastcall KeyPasteClick(TObject *Sender);
        void __fastcall KeyAddChipClick(TObject *Sender);
        void __fastcall ComboTypeChange(TObject *Sender);
        void __fastcall ButtonParentClick(TObject *Sender);
        void __fastcall ListNorthClick(TObject *Sender);
        void __fastcall KeyOptionsClick(TObject *Sender);
        void __fastcall PopupMenuOptionsPopup(TObject *Sender);
        void __fastcall ListVariablesClick(TObject *Sender);
        void __fastcall ButtonVarDelClick(TObject *Sender);
        void __fastcall ButtonVarOkClick(TObject *Sender);
        void __fastcall PageControl1Change(TObject *Sender);
        void __fastcall ButtonVarColorClick(TObject *Sender);
        void __fastcall ComboKeyNoChange(TObject *Sender);
        void __fastcall ListKeyVarClick(TObject *Sender);
        void __fastcall ButtonKeyDelClick(TObject *Sender);
        void __fastcall ButtonKeyOkClick(TObject *Sender);
        void __fastcall CheckAnimeClick(TObject *Sender);
        void __fastcall KeyAboutClick(TObject *Sender);
        void __fastcall KeyConvertCowlClick(TObject *Sender);
        void __fastcall KeyConvertCowlSClick(TObject *Sender);
        void __fastcall ButtonScriptResetClick(TObject *Sender);
        void __fastcall ButtonScriptStopClick(TObject *Sender);
        void __fastcall PopupMenuAddPopup(TObject *Sender);
        void __fastcall ListNorthContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
        void __fastcall KeyDelOptionClick(TObject *Sender);
        void __fastcall KeyUndoClick(TObject *Sender);
private:	// ユーザー宣言
        HGLRC ghRC; // レンダリングコンテキスト
        HDC   ghDC; // デバイスコンテキスト

        TRigidChipCore *Core; // コアチップ
        TRigidChip *Origin;
        TRigidChipsDirection Direction;
        void __fastcall SelectionChange(TRigidChip *chip);

        bool PickUp;
        int PickUpX, PickUpY;
        double camera_x, camera_y, camera_z; // カメラ座標
        double camera_matrix[16]; // カメラ行列

        bool mouse_left, mouse_right; // マウス投下中？
        int mouse_x, mouse_y; // 投下開始マウス位置
        double mouse_cx, mouse_cy, mouse_cz; // 投下時カメラ座標
        double mouse_cm[16]; // 投下時カメラ行列

        bool RunScript;

        TRCDLoader *RCDLoader;
        TRCDSaver *RCDSaver;

        AnsiString FFileName;
        void SetCaption()
        {
          if (FileName == "")
            Caption = "RigidChip Designer";
          else
            Caption = "RCD - " + ExtractFileName(FileName);
          if (Modify)
            Caption = Caption + " [Modified]";
        }
#pragma warn -8026
        void SetFileName(AnsiString fn){ FFileName = fn; SetCaption(); }
        __property AnsiString FileName = {read=FFileName, write=SetFileName};
        bool FModify;
        void SetModify(bool m)
        {
          FModify = m;
          SetCaption();
          if (m) Display();
        }
        __property bool Modify = {read=FModify, write=SetModify};

        UINT WM_RIGHTCHIP_LOAD;
        enum { UMSG_RCLOAD_START = 0, UMSG_RCLOAD_CHAR = 1, UMSG_RCLOAD_END = 2 };
protected:
	void __fastcall WMDropFiles(TWMDropFiles &Msg);

#pragma warn -8027
        BEGIN_MESSAGE_MAP
        VCL_MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFiles)
        END_MESSAGE_MAP(TForm)
#pragma warn .8027
public:		// ユーザー宣言
        TPaintPanel *PaintPanel; // 表示用ウィンドウ
        bool KeysDown[17];
        int MouseX, MouseY;
        bool MouseLeft, MouseMiddle, MouseRight;
        void __fastcall Display();
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
