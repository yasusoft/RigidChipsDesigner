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
{ // OnPaint�����������p�l��
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
__published:	// IDE �Ǘ��̃R���|�[�l���g
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
private:	// ���[�U�[�錾
        HGLRC ghRC; // �����_�����O�R���e�L�X�g
        HDC   ghDC; // �f�o�C�X�R���e�L�X�g
        TPaintPanel *PaintPanel; // �\���p�E�B���h�E

        TRigidChip *Core; // �R�A�`�b�v
        double camera_x, camera_y, camera_z; // �J�������W
        double camera_rx, camera_ry; // �J������]

        bool mouse_left, mouse_right; // �}�E�X�������H
        int mouse_x, mouse_y; // �����J�n�}�E�X�ʒu
        double mouse_cx, mouse_cy, mouse_cz; // �������J�������W
        double mouse_crx, mouse_cry; // �������J�������W
public:		// ���[�U�[�錾
        void __fastcall Display();
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
