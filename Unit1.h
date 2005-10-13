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
class TRCLua;
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
        TMenuItem *Help1;
        TMenuItem *KeyAbout;
        TMenuItem *KeyConvertCowl;
        TMenuItem *KeyBar2;
        TMenuItem *KeyConvertCowlS;
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
        TMenuItem *KeyOptionDelete;
        TMenuItem *N1;
        TMenuItem *KeyUndo;
        TMenuItem *KeyEditBar1;
        TOpenDialog *OpenDialogPanekit;
        TMenuItem *KeyFileBar2;
        TMenuItem *KeyImport;
        TMenuItem *KeySelectAdd;
        TMenuItem *KeyShowVoidOptions;
        TMenuItem *KeyOptionUp;
        TMenuItem *KeyOptionDown;
        TMenuItem *KeyOptionsSort;
        TMenuItem *KeyPasteDirection;
        TMenuItem *KeyAddBar1;
        TMenuItem *KeyDefViewThis;
        TMenuItem *KeyDefViewInitilize;
        TMenuItem *KeyDefaultView;
        TMenuItem *KeyDeleteOne2;
        TMenuItem *KeyDeleteOne;
        TPanel *PanelRight;
        TPageControl *PageControlRight;
        TTabSheet *TabVal;
        TListBox *ListVariables;
        TPanel *PanelVariable;
        TLabel *LabelVarName;
        TLabel *LabelVarDefault;
        TLabel *LabelVarMin;
        TLabel *LabelVarMax;
        TLabel *LabelVarStep;
        TEdit *EditVarName;
        TEdit *EditVarDefault;
        TEdit *EditVarMin;
        TEdit *EditVarMax;
        TEdit *EditVarStep;
        TButton *ButtonVarOk;
        TCheckBox *CheckVarDisp;
        TButton *ButtonVarColor;
        TPanel *PanelVarLabel;
        TButton *ButtonVarDel;
        TTabSheet *TabKey;
        TLabel *LabelKeyNo;
        TLabel *LabelKeyVar;
        TLabel *LabelKeyVarName;
        TLabel *LabelKeyStep;
        TComboBox *ComboKeyNo;
        TComboBox *ComboKeyVarName;
        TEdit *EditKeyStep;
        TListBox *ListKeyVar;
        TButton *ButtonKeyDel;
        TButton *ButtonKeyOk;
        TTabSheet *TabBody;
        TLabel *LabelDirection;
        TLabel *LabelType;
        TListBox *ListNorth;
        TButton *ButtonParent;
        TListBox *ListEast;
        TListBox *ListWest;
        TListBox *ListSouth;
        TComboBox *ComboType;
        TComboBox *ComboDirection;
        TCheckBox *CheckRotate;
        TButton *ButtonPlus;
        TButton *ButtonMinus;
        TTabSheet *TabScript;
        TSplitter *SplitterScript;
        TPanel *PanelScript;
        TButton *ButtonScriptReset;
        TButton *ButtonScriptStop;
        TMemo *MemoScript;
        TMemo *ScriptOut;
        TTabSheet *TabLua;
        TSplitter *SplitterLua;
        TPanel *Panel1;
        TButton *ButtonSetLua;
        TButton *ButtonStopLua;
        TMemo *MemoLua;
        TMemo *LuaOut;
        TTabSheet *TabPanekit;
        TListBox *ListPanekit;
        TPanel *PanelPanekit;
        TLabel *LabelPanekitFile;
        TEdit *EditPanekitFile;
        TButton *ButtonPanekitBrowse;
        TCheckBox *CheckPanekitTruly;
        TCheckBox *CheckPanekitDamper;
        TMemo *MemoPanekit;
        TEdit *EditKeyTest;
        TCheckBox *CheckAnime;
        TSplitter *Splitter1;
        TMenuItem *KeyHide;
        TMenuItem *KeyHideCowl;
        TMenuItem *KeyHideGhost;
        TMenuItem *KeyHideBalloon;
        TMenuItem *KeyShowCGravity;
        TPageControl *PageControlBody;
        TTabSheet *TabOptions;
        TValueListEditor *OptionsEditor;
        TTabSheet *TabMemoChip;
        TTabSheet *TabMemoModel;
        TMemo *MemoChip;
        TMemo *MemoModel;
        TTreeView *TreeViewBody;
        TButton *ButtonSwitch;
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
        void __fastcall PageControlRightChange(TObject *Sender);
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
        void __fastcall KeyOptionDeleteClick(TObject *Sender);
        void __fastcall KeyUndoClick(TObject *Sender);
        void __fastcall ButtonPanekitBrowseClick(TObject *Sender);
        void __fastcall KeyImportClick(TObject *Sender);
        void __fastcall ListPanekitClick(TObject *Sender);
        void __fastcall KeySelectAddClick(TObject *Sender);
        void __fastcall CheckPanekitTrulyClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall KeyShowVoidOptionsClick(TObject *Sender);
        void __fastcall KeyOptionUpClick(TObject *Sender);
        void __fastcall KeyOptionDownClick(TObject *Sender);
        void __fastcall KeyOptionsSortClick(TObject *Sender);
        void __fastcall KeyDefViewThisClick(TObject *Sender);
        void __fastcall KeyDefViewInitilizeClick(TObject *Sender);
        void __fastcall ButtonPlusClick(TObject *Sender);
        void __fastcall ButtonMinusClick(TObject *Sender);
        void __fastcall ButtonSetLuaClick(TObject *Sender);
        void __fastcall ButtonStopLuaClick(TObject *Sender);
        void __fastcall TabBodyResize(TObject *Sender);
        void __fastcall HideClick(TObject *Sender);
        void __fastcall KeyShowCGravityClick(TObject *Sender);
        void __fastcall MemoChipChange(TObject *Sender);
        void __fastcall MemoModelChange(TObject *Sender);
        void __fastcall ButtonSwitchClick(TObject *Sender);
        void __fastcall TreeViewBodyChange(TObject *Sender,
          TTreeNode *Node);
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
        double camera_angleH, camera_angleV;
        double def_cx, def_cy, def_cz;
        double def_caH, def_caV;

        bool mouse_left, mouse_right; // マウス投下中？
        int mouse_x, mouse_y; // 投下開始マウス位置
        double mouse_cx, mouse_cy, mouse_cz; // 投下時カメラ座標
        double mouse_caH, mouse_caV;

        bool RunScript, RunLua;
        TRCLua *RCLua;

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

        bool Keys[17], KeysOld[17], KeyDown[17], KeyUp[17];
        int MouseX, MouseY;
        bool MouseLeft, MouseMiddle, MouseRight;

        TRCDLoader *RCDLoader;
        TRCDSaver *RCDSaver;

        void __fastcall Display();
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
