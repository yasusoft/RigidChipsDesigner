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
#include <GL/gl.h>
#include <GL/glu.h>
#include "RigidChip.h"
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
        TPanel *PanelRight;
        TButton *ButtonOpen;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall PaintPanelPaint(TObject *Sender);
        void __fastcall PaintPanelMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall PaintPanelMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall PaintPanelMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall PaintPanelResize(TObject *Sender);
        void __fastcall ButtonOpenClick(TObject *Sender);
        void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled);
        void __fastcall Timer1Timer(TObject *Sender);
private:	// ユーザー宣言
        HGLRC ghRC; // レンダリングコンテキスト
        HDC   ghDC; // デバイスコンテキスト
        TPaintPanel *PaintPanel; // 表示用ウィンドウ

        TRigidChip *Core; // コアチップ
        double camera_x, camera_y, camera_z; // カメラ座標
        double camera_rx, camera_ry; // カメラ回転

        bool mouse_left, mouse_right; // マウス投下中？
        int mouse_x, mouse_y; // 投下開始マウス位置
        double mouse_cx, mouse_cy, mouse_cz; // 投下時カメラ座標
        double mouse_crx, mouse_cry; // 投下時カメラ座標
public:		// ユーザー宣言
        void __fastcall Display();
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
