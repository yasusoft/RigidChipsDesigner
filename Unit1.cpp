//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "RigidChip.h"
#include "RCDLoader.h"
#include "RCDSaver.h"
#include "RCScript.h"
#include <IniFiles.hpp>
#include <Clipbrd.hpp>
#include <stack>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::WMDropFiles(TWMDropFiles &Msg)
{
  HDROP hDrop = (HDROP)Msg.Drop;
  //int n = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
  char buf[MAX_PATH];
  DragQueryFile(hDrop, 0, buf, MAX_PATH);

  AnsiString filename = buf;
  if (ExtractFileExt(filename).LowerCase() == ".txt"
   || ExtractFileExt(filename).LowerCase() == ".rcd")
  {
    if (Modify)
      switch (Application->MessageBox("Data is modified.\nDo you want to save?", "Modified", MB_YESNOCANCEL | MB_ICONQUESTION))
      {
        case IDYES:
          KeySaveClick(NULL);
          break;
        case IDNO:
          Modify = false;
          break;
      }
    if (Modify)
      return;

    if (Core)
    {
      TRigidChip *c = Core;
      Origin = Core = NULL;
      delete c;
    }

    Origin = Core = RCDLoader->Load(filename);
    if (Core == NULL)
    {
      Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "G[", MB_OK);
      return;
    }
    FileName = filename;
    Modify = false;
    PaintPanelMouseDown(NULL, mbMiddle, TShiftState(), 0, 0);
    PageControl1->ActivePage = TabBody;
    ScriptOut->Clear();
    RunScript = true;
  }

  DragFinish(hDrop);
  EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
  randomize();
  Origin = Core = NULL;
  FileName = "";
  Modify = false;
  PageControl1->ActivePage = TabBody;

  RCDLoader = new TRCDLoader;
  RCDSaver = new TRCDSaver;

  PaintPanel = new TPaintPanel(this);
  PaintPanel->Parent = this;
  PaintPanel->Left = PaintPanelDummy->Left;
  PaintPanel->Top = PaintPanelDummy->Top;
  PaintPanel->Width = PaintPanelDummy->Width;
  PaintPanel->Height = PaintPanelDummy->Height;
  PaintPanel->Anchors = TAnchors() << akLeft << akTop << akRight << akBottom; //PaintPanelDummy->Anchors;
  PaintPanel->OnMouseDown = PaintPanelMouseDown;
  PaintPanel->OnMouseMove = PaintPanelMouseMove;
  PaintPanel->OnMouseUp = PaintPanelMouseUp;
  PaintPanel->OnPaint = PaintPanelPaint;
  PaintPanel->OnResize = PaintPanelResize;

  ghDC = GetDC(PaintPanel->Handle);
  //ghDC = PaintPanel->Canvas->Handle;

  // bSetupPixelFormat(ghDC)
  PIXELFORMATDESCRIPTOR pfd;
  ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.dwLayerMask = PFD_MAIN_PLANE;
  pfd.iPixelType = PFD_TYPE_RGBA; // PFD_TYPE_COLORINDEX
  pfd.cColorBits = 24;
  pfd.cDepthBits = 32;

  int pixelformat = ChoosePixelFormat(ghDC, &pfd);
  if (pixelformat == 0)
  {
    Application->MessageBox("ChoosePixelFormat failed", "Error", MB_ICONERROR | MB_OK);
    Close();
    return;
  }
  if (SetPixelFormat(ghDC, pixelformat, &pfd) == FALSE)
  {
    Application->MessageBox("SetPixelFormat failed", "Error", MB_ICONERROR | MB_OK);
    Close();
    return;
  }

  ghRC = wglCreateContext(ghDC);
  wglMakeCurrent(ghDC, ghRC);

  // InitilizeGL
  glClearColor(0, 0, 0, 0);
  glViewport(0, 0, PaintPanel->Width, PaintPanel->Height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-2, 2, -2, 2, 10, 1000);
  //glOrtho(-2, 2, -2, 2, 10, 1000);
  gluLookAt(0, 0, 40, 0, 0, 0, 0, 1, 0);

  glEnable(GL_DEPTH_TEST);

  // Lighting
  GLfloat col0[] = {0, 0, 0, 1};
  GLfloat col1[] = {1, 1, 1, 1};
  glEnable(GL_LIGHTING);
  // light0
  GLfloat pos0[] = {0, 0, 1000, 1};
  glLightfv(GL_LIGHT0, GL_AMBIENT, col1);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, col1);
  glLightfv(GL_LIGHT0, GL_POSITION, pos0);
  glEnable(GL_LIGHT0);
  // diffuse2
  GLfloat pos1[] = {0, 0, -1000, 1};
  glLightfv(GL_LIGHT1, GL_AMBIENT, col0);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, col1);
  glLightfv(GL_LIGHT1, GL_POSITION, pos1);
  glEnable(GL_LIGHT1);

  // Textures
  Graphics::TBitmap *bmp = new Graphics::TBitmap;
  try
  {
    BITMAP bm;

    #define LOAD_TEXTURE(rc, var) { \
        bmp->LoadFromResourceName((int)HInstance, rc); \
        if (GetObject(bmp->Handle, sizeof(BITMAP), &bm)) \
        { \
          glGenTextures(1, &var); \
          glBindTexture(GL_TEXTURE_2D, var); \
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits); \
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
          glBindTexture(GL_TEXTURE_2D, 0); \
        } \
    }
    LOAD_TEXTURE("TEX_CORE", TRigidChipCore::Texture);
    LOAD_TEXTURE("TEX_CHIP", TRigidChipChip::Texture);
    LOAD_TEXTURE("TEX_WEIGHT", TRigidChipWeight::Texture);
    LOAD_TEXTURE("TEX_WHEEL", TRigidChipWheel::Texture);
    LOAD_TEXTURE("TEX_RLW", TRigidChipRLW::Texture);
    LOAD_TEXTURE("TEX_JET", TRigidChipJet::Texture);
    LOAD_TEXTURE("TEX_BALL", TRigidChipJet::TextureBall);
    LOAD_TEXTURE("TEX_RUDDER", TRigidChipRudder::Texture);
    LOAD_TEXTURE("TEX_TRIM", TRigidChipTrim::Texture);
    LOAD_TEXTURE("TEX_ARM", TRigidChipArm::Texture);
    LOAD_TEXTURE("TEX_COWL0", TRigidChipCowl::Texture0);
    LOAD_TEXTURE("TEX_COWL2", TRigidChipCowl::Texture2);
    LOAD_TEXTURE("TEX_COWL34", TRigidChipCowl::Texture34);
    LOAD_TEXTURE("TEX_COWL5", TRigidChipCowl::Texture5);
  }
  __finally
  {
    delete bmp;
  }

  DragAcceptFiles(Handle, true);
  WM_RIGHTCHIP_LOAD = RegisterWindowMessage("WM_RIGHTCHIP_LOAD");

  if (ParamCount() >= 1 && FileExists(ParamStr(1)))
  {
    Origin = Core = RCDLoader->Load(ParamStr(1));
    if (Core == NULL)
    {
      Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "G[", MB_OK);
    }
    else
    {
      FileName = ParamStr(1);
      Modify = false;
      PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
      PageControl1->ActivePage = TabBody;
      ScriptOut->Clear();
      RunScript = true;
    }
  }
  if (Core == NULL)
    KeyNewClick(Sender);

  TIniFile *ini = NULL;
  try
  {
    ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
    KeySelectAdd->Checked = ini->ReadBool("Option", "SelectAdd", KeySelectAdd->Checked);
    RCDSaver->optObfuscate = ini->ReadBool("OptionSave", "Obfuscate", RCDSaver->optObfuscate);
    RCDSaver->optSpaceAfterBlockType = ini->ReadBool("OptionSave", "SpaceBlock", RCDSaver->optSpaceAfterBlockType);
    RCDSaver->optNewLineAfterBlockType = ini->ReadBool("OptionSave", "NewLineBlock", RCDSaver->optNewLineAfterBlockType);
    RCDSaver->optSpaceAfterOptions = ini->ReadBool("OptionSave", "SpaceOption", RCDSaver->optSpaceAfterOptions);
    RCDSaver->optNewLineAfterOptions = ini->ReadBool("OptionSave", "NewLineOption", RCDSaver->optNewLineAfterOptions);
    RCDSaver->optSpaceAfterComma = ini->ReadBool("OptionSave", "SpaceComma", RCDSaver->optSpaceAfterComma);
    RCDSaver->optNoSubNoNewLine = ini->ReadBool("OptionSave", "NoSubNoNewLine", RCDSaver->optNoSubNoNewLine);
    delete ini;
  }
  catch (...)
  {
    if (ini)
      delete ini;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
  if (Core)
    delete Core;

  if (ghRC)
    wglDeleteContext(ghRC);
  if (ghDC)
    ReleaseDC(PaintPanel->Handle, ghDC);

  delete PaintPanel;

  delete RCDSaver;
  delete RCDLoader;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
  TIniFile *ini = NULL;
  try
  {
    ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
    ini->WriteBool("Option", "SelectAdd", KeySelectAdd->Checked);
    ini->WriteBool("OptionSave", "Obfuscate", RCDSaver->optObfuscate);
    ini->WriteBool("OptionSave", "SpaceBlock", RCDSaver->optSpaceAfterBlockType);
    ini->WriteBool("OptionSave", "NewLineBlock", RCDSaver->optNewLineAfterBlockType);
    ini->WriteBool("OptionSave", "SpaceOption", RCDSaver->optSpaceAfterOptions);
    ini->WriteBool("OptionSave", "NewLineOption", RCDSaver->optNewLineAfterOptions);
    ini->WriteBool("OptionSave", "SpaceComma", RCDSaver->optSpaceAfterComma);
    ini->WriteBool("OptionSave", "NoSubNoNewLine", RCDSaver->optNoSubNoNewLine);
    delete ini;
  }
  catch (...)
  {
    if (ini)
      delete ini;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  KeyNewClick(Sender);
  CanClose = !Modify;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelPaint(TObject *Sender)
{
  Display();
  //Yield();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SelectionChange(TRigidChip *chip)
{
  ListNorth->Items->Clear();
  ListEast->Items->Clear();
  ListWest->Items->Clear();
  ListSouth->Items->Clear();
  Direction = rdCore;

  if (!Core) return;
  Core->Select = NULL;

  if (chip)
  {
    // e`bvðt¤ÌXgÉÇÁ
    if (chip->Parent)
      switch (chip->Direction)
      {
        case rdNorth:
          ListSouth->Items->AddObject("p:" + chip->Parent->GetTypeString(), (TObject*)chip->Parent);
          break;
        case rdEast:
          ListWest->Items->AddObject("p:" + chip->Parent->GetTypeString(), (TObject*)chip->Parent);
          break;
        case rdWest:
          ListEast->Items->AddObject("p:" + chip->Parent->GetTypeString(), (TObject*)chip->Parent);
          break;
        case rdSouth:
          ListNorth->Items->AddObject("p:" + chip->Parent->GetTypeString(), (TObject*)chip->Parent);
          break;
      }

    for (int i = 0; i < chip->SubChipsCount; i ++)
    {
      switch (chip->SubChips[i]->Direction)
      {
        case rdNorth:
          ListNorth->Items->AddObject(IntToStr(i) + ":" + chip->SubChips[i]->GetTypeString(), (TObject*)chip->SubChips[i]);
          break;
        case rdEast:
          ListEast->Items->AddObject(IntToStr(i) + ":" + chip->SubChips[i]->GetTypeString(), (TObject*)chip->SubChips[i]);
          break;
        case rdWest:
          ListWest->Items->AddObject(IntToStr(i) + ":" + chip->SubChips[i]->GetTypeString(), (TObject*)chip->SubChips[i]);
          break;
        case rdSouth:
          ListSouth->Items->AddObject(IntToStr(i) + ":" + chip->SubChips[i]->GetTypeString(), (TObject*)chip->SubChips[i]);
          break;
      }
    }
    OptionsEditor->Strings->Assign(chip->Options);
    ComboDirection->ItemIndex = chip->Direction - 1;
    ComboType->ItemIndex = ComboType->Items->IndexOf(chip->GetTypeString());

    Core->Select = chip;
    bool tmp = true;
    OptionsEditorSelectCell(NULL, OptionsEditor->Col, OptionsEditor->Row, tmp);
  }
  else
  {
    ComboDirection->ItemIndex = -1;
    ComboType->ItemIndex = -1;
    OptionsEditor->Strings->Clear();
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Display()
{
  if (!Core)
    return;

  Core->ShowCowl = CheckCowl->Checked;
  Core->ShowGhost = CheckGhost->Checked;
  Core->ShowBalloon = CheckBalloon->Checked;

  if (PickUp)
  {
    PickUp = false;

    const int SELBUFSIZE = 100;
    GLuint selbuf[SELBUFSIZE];
    GLint viewport[4];
    GLfloat matrix[16];

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetFloatv(GL_PROJECTION_MATRIX, matrix);
    glSelectBuffer(SELBUFSIZE, selbuf);
    glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(PickUpX, PickUpY, 2.0, 2.0, viewport);
    glMultMatrixf(matrix);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(-camera_x, -camera_y, -camera_z);
    glMultMatrixd(camera_matrix);

    Origin->Draw();
    Origin->DrawTranslucent();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    GLuint hitnum = glRenderMode(GL_RENDER);
    float minimum;
    GLuint name = 0;
    for (GLuint i = 0, j = 0; i < hitnum && j < SELBUFSIZE; i ++)
    {
      GLuint namenum = selbuf[j++];
      float depthmin = (float)selbuf[j] / 0x7FFFFFFF;
      j ++;
      //float depthmax = (float)selbuf[j] / 0x7FFFFFFF;
      j ++;
      for (GLuint k = 0; k < namenum; k ++)
      {
        GLuint n = selbuf[j++];
        if (n == 0)
          continue;
        if (name == 0 || depthmin < minimum)
        {
          name = n;
          minimum = depthmin;
        }
      }
    }

    Direction = rdCore;
    if (name == rdNorth || name == rdEast || name == rdWest || name == rdSouth)
    {
      Direction = (TRigidChipsDirection)name;
      PopupMenuAdd->Popup(Mouse->CursorPos.x, Mouse->CursorPos.y);
    }
    else if (name)
    {
      TRigidChip *chip = (TRigidChip*)name;
      if (Core->Select == chip)
        PopupMenu1->Popup(Mouse->CursorPos.x, Mouse->CursorPos.y);
      SelectionChange(chip);
    }
    else
    {
      if (Core->Select == NULL)
        Origin = Core;
      SelectionChange(NULL);
    }
  }

  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslated(-camera_x, -camera_y, -camera_z);
  glMultMatrixd(camera_matrix);

  Origin->Draw();
  Origin->DrawTranslucent();

  glPopMatrix();

  glFlush();
  SwapBuffers(ghDC);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
  switch (Button)
  {
    case mbLeft: MouseLeft = true; break;
    case mbMiddle: MouseMiddle = true; break;
    case mbRight: MouseRight = true; break;
  }

  mouse_x = X;
  mouse_y = Y;
  if (Button == mbRight)
  {
    mouse_right = true;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(camera_matrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, mouse_cm);
    glPopMatrix();
  }
  else if (mouse_right && Button == mbLeft)
  {
    mouse_left = true;
    mouse_cx = camera_x;
    mouse_cy = camera_y;
  }
  else if (Button == mbMiddle)
  {
    if (Core)
      SelectionChange(NULL);
    camera_x = 0;
    camera_y = 0;
    camera_z = 0;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glGetDoublev(GL_MODELVIEW_MATRIX, camera_matrix);
    glPopMatrix();
    Display();
  }
  else if (Button == mbLeft)
  {
    PickUpX = X;
    PickUpY = PaintPanel->Height - Y - 1;
    PickUp = true;
    Display();
  }

  if (Button != mbMiddle)
    EditKeyTest->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  MouseX = X;
  MouseY = Y;
  if (mouse_left && mouse_right)
  {
    camera_x = mouse_cx - (X - mouse_x) / 50.0;
    camera_y = mouse_cy + (Y - mouse_y) / 50.0;
    Display();
  }
  else if (mouse_right)
  {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRotated(X - mouse_x, 0, 1, 0);
    glRotated(Y - mouse_y, 1, 0, 0);
    glMultMatrixd(mouse_cm);
    glGetDoublev(GL_MODELVIEW_MATRIX, camera_matrix);
    glPopMatrix();
    Display();
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormMouseWheel(TObject *Sender, TShiftState Shift,
      int WheelDelta, TPoint &MousePos, bool &Handled)
{
  if (!EditKeyTest->Focused())
    return;
  if (WheelDelta < 0 && camera_z < 250)
    camera_z += camera_z > 0 ? camera_z * 0.1 + 1 : 1;
  if (WheelDelta > 0 && camera_z > -25)
    camera_z -= camera_z > 0 ? camera_z * 0.1 + 1 : 1;
  Display();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  switch (Button)
  {
    case mbLeft: MouseLeft = false; break;
    case mbMiddle: MouseMiddle = false; break;
    case mbRight: MouseRight = false; break;
  }

  //if (Button == mbLeft)
    mouse_left = false;
  //if (Button == mbRight)
    mouse_right = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PaintPanelResize(TObject *Sender)
{
  int max = PaintPanel->Width > PaintPanel->Height ? PaintPanel->Width : PaintPanel->Height;
  glViewport((PaintPanel->Width - max) / 2, (PaintPanel->Height - max) / 2, max, max);
  Display();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  Timer1->Enabled = false;
  if (Core)
  {
    Core->CheckVariable();
    for (int i = 0; i < 17; i ++)
      if (KeysDown[i])
        Core->ActKey(IntToStr(i));
    Core->StepVariable();
    Core->Act();

    if (RunScript && Core->Script->ScriptText != "")
    {
      ScriptOut->Items->BeginUpdate();
      try {
        for (int i = 0; i < ScriptOut->Items->Count; i ++)
          ScriptOut->Items->Strings[i] = "";
        Core->Script->Run();
      } catch (Exception &e) {
        RunScript = false;
        ScriptOut->Clear();
        ScriptOut->Items->Add("ScriptError: Near " + IntToStr(Core->Script->ProgramCounter));
        ScriptOut->Items->Add(e.Message);
        MemoScript->SelStart = Core->Script->ProgramCounter;
        MemoScript->SelLength = 1;
      }
      ScriptOut->Items->EndUpdate();
    }

    Display();
    Yield();
  }
  Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckAnimeClick(TObject *Sender)
{
  Timer1->Enabled = CheckAnime->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditKeyTestKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  CheckAnime->Checked = true;
  Timer1->Enabled = true;
  const bool f = true;
  switch (Key)
  {
    case VK_UP: KeysDown[0] = f; break;
    case VK_DOWN: KeysDown[1] = f; break;
    case VK_LEFT: KeysDown[2] = f; break;
    case VK_RIGHT: KeysDown[3] = f; break;
    case 'Z': case 'z': KeysDown[4] = f; break;
    case 'X': case 'x': KeysDown[5] = f; break;
    case 'C': case 'c': KeysDown[6] = f; break;
    case 'A': case 'a': KeysDown[7] = f; break;
    case 'S': case 's': KeysDown[8] = f; break;
    case 'D': case 'd': KeysDown[9] = f; break;
    case 'V': case 'v': KeysDown[10] = f; break;
    case 'B': case 'b': KeysDown[11] = f; break;
    case 'F': case 'f': KeysDown[12] = f; break;
    case 'G': case 'g': KeysDown[13] = f; break;
    case 'Q': case 'q': KeysDown[14] = f; break;
    case 'W': case 'w': KeysDown[15] = f; break;
    case 'E': case 'e': KeysDown[16] = f; break;

    case 'K': // JªãûüÖñ]
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glRotated(15, 1, 0, 0);
      glMultMatrixd(camera_matrix);
      glGetDoublev(GL_MODELVIEW_MATRIX, camera_matrix);
      glPopMatrix();
      Display();
      break;
    case 'L': // JªºûüÖñ]
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glRotated(-15, 1, 0, 0);
      glMultMatrixd(camera_matrix);
      glGetDoublev(GL_MODELVIEW_MATRIX, camera_matrix);
      glPopMatrix();
      Display();
      break;
    case 0xBC/*KEY_OEM_COMMA*/: // Jª¶ûüÖñ]
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glRotated(15, 0, 1, 0);
      glMultMatrixd(camera_matrix);
      glGetDoublev(GL_MODELVIEW_MATRIX, camera_matrix);
      glPopMatrix();
      Display();
      break;
    case 0xBE/*KEY_OEM_PERIOD*/: // JªEûüÖñ]
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glRotated(-15, 0, 1, 0);
      glMultMatrixd(camera_matrix);
      glGetDoublev(GL_MODELVIEW_MATRIX, camera_matrix);
      glPopMatrix();
      Display();
      break;
    case 'I': case 'i': // gå
      if (camera_z > -25)
        camera_z -= camera_z > 0 ? camera_z * 0.5 + 1 : 5;
      Display();
      break;
    case 'O': case 'o': // k¬
      if (camera_z < 250)
        camera_z += camera_z > 0 ? camera_z * 0.5 + 1 : 5;
      Display();
      break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::EditKeyTestKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  const bool f = false;
  switch (Key)
  {
    case VK_UP: KeysDown[0] = f; break;
    case VK_DOWN: KeysDown[1] = f; break;
    case VK_LEFT: KeysDown[2] = f; break;
    case VK_RIGHT: KeysDown[3] = f; break;
    case 'Z': case 'z': KeysDown[4] = f; break;
    case 'X': case 'x': KeysDown[5] = f; break;
    case 'C': case 'c': KeysDown[6] = f; break;
    case 'A': case 'a': KeysDown[7] = f; break;
    case 'S': case 's': KeysDown[8] = f; break;
    case 'D': case 'd': KeysDown[9] = f; break;
    case 'V': case 'v': KeysDown[10] = f; break;
    case 'B': case 'b': KeysDown[11] = f; break;
    case 'F': case 'f': KeysDown[12] = f; break;
    case 'G': case 'g': KeysDown[13] = f; break;
    case 'Q': case 'q': KeysDown[14] = f; break;
    case 'W': case 'w': KeysDown[15] = f; break;
    case 'E': case 'e': KeysDown[16] = f; break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerReloadTimer(TObject *Sender)
{
  if (FileName == "" || Modify)
  {
    TimerReload->Enabled = false;
    KeyAutoReload->Checked = false;
    return;
  }

  int age = FileAge(FileName);
  if (age == TimerReload->Tag)
    return;
  TimerReload->Tag = age;
  
  TRigidChipCore *corenew = RCDLoader->Load(FileName);
  if (corenew)
  {
    TRigidChipCore *coreback = Core;
    Origin = Core = corenew;
    Display();
    PageControl1Change(Sender);

    if (coreback)
      delete coreback;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckClick(TObject *Sender)
{
  Display();
}
//---------------------------------------------------------------------------
void ChangeDirection(TRigidChip *chip, int dir)
{
  int diff = dir - chip->Direction + 4;
  chip->Direction = (TRigidChipsDirection)dir;
  for (int i = 0; i < chip->SubChipsCount; i ++)
    ChangeDirection(chip->SubChips[i], (chip->SubChips[i]->Direction - 1 + diff) % 4 + 1);
}
void __fastcall TForm1::ComboDirectionChange(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (Core->Select == Core)
  {
    ComboDirection->ItemIndex = -1;
    return;
  }
  int d = ComboDirection->ItemIndex + 1;
  if (CheckRotate->Checked)
    ChangeDirection(Core->Select, d);
  else
    Core->Select->Direction = (TRigidChipsDirection)d;
  SelectionChange(Core->Select);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ComboTypeChange(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (Core->Select == Core)
  {
    ComboType->ItemIndex = -1;
    return;
  }

  AnsiString cap = ComboType->Text;

  // Tu`bvÉCowlÈOðÂ©`FbN
  bool haschip = false;
  for (int i = 0; i < Core->Select->SubChipsCount; i ++)
    if (Core->Select->SubChips[i]->GetType() != ctCowl)
    {
      haschip = true;
      break;
    }
  if (haschip && cap == "Cowl")
  {
    ComboType->ItemIndex = ComboType->Items->IndexOf(Core->Select->GetTypeString());
    Application->MessageBox("Chip which has sub chips can't be cowl", "ChangeType", MB_ICONERROR | MB_OK);
    return;
  }

  cap = "N:" + cap + "(){}";
  TRigidChip *chip = RCDLoader->StringToChip(cap);
  if (chip)
  {
    Core->Select->Parent->AddSubChip(chip);
    chip->Direction = Core->Select->Direction;
    chip->SetOptions(Core->Select->Options->Text);
    for (int i = 0; i < Core->Select->SubChipsCount; i ++)
      chip->AddSubChip(Core->Select->SubChips[i]);
    for (int i = Core->Select->SubChipsCount-1; i >= 0; i --)
      Core->Select->DelSubChip(i);

    TRigidChip *c = Core->Select;
    SelectionChange(chip);
    delete c;
  }
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OptionsEditorSelectCell(TObject *Sender, int ACol,
      int ARow, bool &CanSelect)
{
  if (!Core || !Core->Select) return;

  if (ACol == 1 && ARow > 0)
  {
    AnsiString key = OptionsEditor->Keys[ARow].LowerCase();
    if (key == "color")
      OptionsEditor->ItemProps[ARow-1]->EditStyle = esEllipsis;
    else if (key == "angle")
    {
      OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
      OptionsEditor->ItemProps[ARow-1]->PickList->Clear();
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("30");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("45");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("60");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("90");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("120");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("135");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("150");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("180");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-30");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-45");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-60");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-90");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-120");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-135");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-150");
      OptionsEditor->ItemProps[ARow-1]->PickList->Add("-180");
      for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
        OptionsEditor->ItemProps[ARow-1]->PickList->Add(i->first);
      for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
        OptionsEditor->ItemProps[ARow-1]->PickList->Add("-"+i->first);
    }
    else if (key == "power" || key == "brake")
    {
      OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
      OptionsEditor->ItemProps[ARow-1]->PickList->Clear();
      for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
        OptionsEditor->ItemProps[ARow-1]->PickList->Add(i->first);
      for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
        OptionsEditor->ItemProps[ARow-1]->PickList->Add("-"+i->first);
    }
    else if (key == "option")
    {
      if (Core->Select->GetType() == ctFrame
       || Core->Select->GetType() == ctRudderF
       || Core->Select->GetType() == ctTrimF)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1";
      }
      else if (Core->Select->GetType() == ctWheel)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1\n2";
      }
      else if (Core->Select->GetType() == ctJet)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1\n2";
      }
      else if (Core->Select->GetType() == ctCowl)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1\n2\n3\n4\n5";
      }
    }
    else if (key == "effect")
    {
      if (Core->Select->GetType() == ctWheel)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10";
      }
      else if (Core->Select->GetType() == ctJet)
      {
        OptionsEditor->ItemProps[ARow-1]->EditStyle = esPickList;
        OptionsEditor->ItemProps[ARow-1]->PickList->Text = "0\n1";
      }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OptionsEditorEditButtonClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;

  if (OptionsEditor->Keys[OptionsEditor->Row].LowerCase() != "color")
    return;
  double d = 0xFFFFFF;
  Core->StrToDouble(OptionsEditor->Values["color"], &d);
  int c = d;
  c = ((c&0xFF)<<16) | (c&0xFF00) | ((c&0xFF0000)>>16);
  ColorDialog->CustomColors->Text = "ColorA=" + IntToHex((int)c, 6);
  ColorDialog->Color = (TColor)c;
  if (ColorDialog->Execute())
  {
    c = ColorToRGB(ColorDialog->Color);
    c = ((c&0xFF)<<16) | (c&0xFF00) | ((c&0xFF0000)>>16);
    AnsiString s = "#"+IntToHex(c, 6);
    OptionsEditor->Values["color"] = s;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::OptionsEditorStringsChange(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  Core->Select->SetOptions(OptionsEditor->Strings->CommaText);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PopupMenuOptionsPopup(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  bool core = Core->Select->GetType() == ctCore;
  bool frame = Core->Select->GetType() == ctFrame
         || Core->Select->GetType() == ctRudderF
         || Core->Select->GetType() == ctTrimF;
  bool wheel = Core->Select->GetType() == ctWheel
         || Core->Select->GetType() == ctRLW;
  bool jet = Core->Select->GetType() == ctJet;
  bool arm = Core->Select->GetType() == ctArm;
  bool cowl = Core->Select->GetType() == ctCowl;
  KeyOptionsAngle->Visible = !core;
  KeyOptionsSpring->Visible = !core;
  KeyOptionsDamper->Visible = !core;
  KeyOptionsPower->Visible = wheel || jet || arm;
  KeyOptionsBrake->Visible = wheel;
  KeyOptionsOption->Visible = frame || wheel || jet || arm || cowl;
  KeyOptionsEffect->Visible = wheel || jet;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyDelOptionClick(TObject *Sender)
{
  if (OptionsEditor->Strings->Count)
    OptionsEditor->DeleteRow(OptionsEditor->Row);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptionsClick(TObject *Sender)
{
  AnsiString cap = ((TMenuItem*)Sender)->Caption;
  OptionsEditor->InsertRow(cap, "", true);
  bool tmp = true;
  OptionsEditorSelectCell(NULL, OptionsEditor->Col, OptionsEditor->Row, tmp);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyAboutClick(TObject *Sender)
{
  ShowMessage(
        "RigidChips Designer\n"
        "Copyright(C) Yasu software\n"
        "http://www.yasu.nu/"
  );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyNewClick(TObject *Sender)
{
  if (Modify)
    switch (Application->MessageBox("Data is modified.\nDo you want to save?", "Modified", MB_YESNOCANCEL | MB_ICONQUESTION))
    {
      case IDYES:
        KeySaveClick(Sender);
        if (Modify)
          return;
        break;
      case IDCANCEL:
        return;
    }

  if (Core)
  {
    TRigidChip *c = Core;
    Origin = Core = NULL;
    delete c;
  }
  
  Origin = Core = new TRigidChipCore;
  FileName = "";
  Modify = false;
  PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
  if (PageControl1->ActivePage != TabPanekit)
    PageControl1->ActivePage = TabBody;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOpenClick(TObject *Sender)
{
  if (Modify)
    switch (Application->MessageBox("Data is modified.\nDo you want to save?", "Modified", MB_YESNOCANCEL | MB_ICONQUESTION))
    {
      case IDYES:
        KeySaveClick(Sender);
        if (Modify)
          return;
        break;
      case IDCANCEL:
        return;
    }
  
  OpenDialog->FileName = "*.rcd;*.txt";
  if (OpenDialog->Execute())
  {
    if (Core)
    {
      TRigidChip *c = Core;
      Origin = Core = NULL;
      delete c;
    }

    Origin = Core = RCDLoader->Load(OpenDialog->FileName);
    if (Core == NULL)
    {
      Application->MessageBox(RCDLoader->ErrorMessage.c_str(), "G[", MB_OK);
      return;
    }
    FileName = OpenDialog->FileName;
    Modify = false;
    PaintPanelMouseDown(Sender, mbMiddle, TShiftState(), 0, 0);
    PageControl1->ActivePage = TabBody;
    ScriptOut->Clear();
    RunScript = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeySaveClick(TObject *Sender)
{
  if (!Core)
    return;
  if (FileName == "")
  {
    KeySaveAsClick(Sender);
    return;
  }

  RCDSaver->Save(FileName, Core);
  Modify = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeySaveAsClick(TObject *Sender)
{
  if (!Core)
    return;

  if (FileName != "")
  {
    SaveDialog->InitialDir = ExtractFileDir(FileName);
    SaveDialog->FileName = ExtractFileName(FileName);
  }
  else
    SaveDialog->FileName = "*.rcd;*.txt";
  if (SaveDialog->Execute())
  {
    FileName = SaveDialog->FileName;
    if (ExtractFileExt(FileName) == "")
      FileName = FileName + ".rcd";
    KeySaveClick(Sender);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyImportClick(TObject *Sender)
{
  KeyNewClick(Sender);
  if (Modify) return;
  PageControl1->ActivePage = TabPanekit;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyExitClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyRCLoadClick(TObject *Sender)
{
  if (FileName != "")
  {
    PostMessage(HWND_BROADCAST, WM_RIGHTCHIP_LOAD, UMSG_RCLOAD_START, 0);
    for (int i = 1; i <= FileName.Length(); i ++)
      PostMessage(HWND_BROADCAST, WM_RIGHTCHIP_LOAD, UMSG_RCLOAD_CHAR, FileName[i]);
    PostMessage(HWND_BROADCAST, WM_RIGHTCHIP_LOAD, UMSG_RCLOAD_END, 0);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyAutoReloadClick(TObject *Sender)
{
  KeyAutoReload->Checked = !KeyAutoReload->Checked;
  TimerReload->Enabled = KeyAutoReload->Checked;
  TimerReload->Tag = 0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeySelectAddClick(TObject *Sender)
{
  KeySelectAdd->Checked = !KeySelectAdd->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOptSaveClick(TObject *Sender)
{
  KeySaveObfuscate->Checked = RCDSaver->optObfuscate;
  KeySpaceBlock->Checked = RCDSaver->optSpaceAfterBlockType;
  KeyNLBlock->Checked = RCDSaver->optNewLineAfterBlockType;
  KeySpaceOpt->Checked = RCDSaver->optSpaceAfterOptions;
  KeyNLOpt->Checked = RCDSaver->optNewLineAfterOptions;
  KeySpaceComma->Checked = RCDSaver->optSpaceAfterComma;
  KeyNoSubNoNL->Checked = RCDSaver->optNoSubNoNewLine;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeySaveOptionClick(TObject *Sender)
{
  ((TMenuItem*)Sender)->Checked = !((TMenuItem*)Sender)->Checked;
  RCDSaver->optObfuscate = KeySaveObfuscate->Checked;
  RCDSaver->optSpaceAfterBlockType = KeySpaceBlock->Checked;
  RCDSaver->optNewLineAfterBlockType = KeyNLBlock->Checked;
  RCDSaver->optSpaceAfterOptions = KeySpaceOpt->Checked;
  RCDSaver->optNewLineAfterOptions = KeyNLOpt->Checked;
  RCDSaver->optSpaceAfterComma = KeySpaceComma->Checked;
  RCDSaver->optNoSubNoNewLine = KeyNoSubNoNL->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyEditClick(TObject *Sender)
{
  KeyUndo->Enabled = Core && Core->Deleted;
  
  KeyCut->Enabled = Core && Core->Select;
  KeyCopy->Enabled = Core && Core->Select;
  KeyPaste->Enabled = Core && Core->Select && Clipboard()->HasFormat(CF_TEXT);
  KeyDelete->Enabled = Core && Core->Select;

  KeyOrigin->Visible = Core && Core->Select;

  KeyCut2->Visible = Core && Core->Select;
  KeyCopy2->Visible = Core && Core->Select;
  KeyPaste2->Visible = Core && Core->Select;
  KeyPaste2->Enabled = Clipboard()->HasFormat(CF_TEXT);
  KeyDelete2->Visible = Core && Core->Select;

  KeyConvertCowlS->Visible = Core && Core->Select;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyUndoClick(TObject *Sender)
{
  if (!Core || !Core->Deleted) return;
  Core->Deleted->Parent->AddSubChip(Core->Deleted);
  if (KeySelectAdd->Checked)
    SelectionChange(Core->Deleted);
  else
    SelectionChange(Core->Select);
  Core->Deleted = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyCutClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;
  if (Core->Select == Core)
  {
    Application->MessageBox("Can't cut core", "Cut", MB_ICONWARNING | MB_OK);
    return;
  }
  KeyCopyClick(Sender);
  KeyDeleteClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyCopyClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;
  if (Core->Select == Core)
  {
    Application->MessageBox("Can't copy core", "Copy", MB_ICONWARNING | MB_OK);
    return;
  }
  Clipboard()->AsText = RCDSaver->ChipToString("", Core->Select);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyPasteClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;
  if (!Clipboard()->HasFormat(CF_TEXT))
    return;
  TRigidChip *chip = RCDLoader->StringToChip(Clipboard()->AsText);
  if (chip == NULL)
  {
    Application->MessageBox(("Invalid clipboard text\n\n" + RCDLoader->ErrorMessage).c_str(), "Paste", MB_ICONERROR | MB_OK);
    return;
  }
  if (Direction != rdCore)
    chip->Direction = Direction;
  Core->Select->AddSubChip(chip);
  if (KeySelectAdd->Checked)
    SelectionChange(chip);
  else
    SelectionChange(Core->Select);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyDeleteClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;
  if (Core->Select == Core)
  {
    Application->MessageBox("Can't delete core", "Delete", MB_ICONWARNING | MB_OK);
    return;
  }
  if (Core->Deleted)
    delete Core->Deleted;
  Core->Deleted = Core->Select;
  Core->Select->Parent->DelSubChip(Core->Select);
  SelectionChange(Core->Select->Parent);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyOriginClick(TObject *Sender)
{
  if (!Core) return;
  if (Core->Select)
    Origin = Core->Select;
  else
    Origin = Core;
  Display();
}
//---------------------------------------------------------------------------
bool ConvertCowl(TRigidChip *chip)
{
  if (chip->GetType() == ctCowl)
    return true;

  bool haschip = false;
  for (int i = 0; i < chip->SubChipsCount; i ++)
  {
    if (chip->SubChips[i]->GetType() != ctCowl)
    {
      if (ConvertCowl(chip->SubChips[i]))
        i --;
      else
        haschip = true;
    }
  }
  if (haschip)
    return false;

  if (chip->GetType() != ctChip
   && chip->GetType() != ctFrame
   && chip->GetType() != ctWeight
  )
    return false;

  TRigidChip *cowl = new TRigidChipCowl;
  chip->Parent->AddSubChip(cowl);
  cowl->Direction = chip->Direction;
  cowl->SetOptions(chip->Options->Text);
  for (int i = 0; i < chip->SubChipsCount; i ++)
    cowl->AddSubChip(chip->SubChips[i]);
  for (int i = chip->SubChipsCount-1; i >= 0; i --)
    chip->DelSubChip(i);

  if (chip->GetType() == ctChip || chip->GetType() == ctWeight)
    cowl->Options->Values["option"] = "0";
  else if (chip->GetType() == ctFrame)
    cowl->Options->Values["option"] = "1";

  delete chip;
  return true;
}
void __fastcall TForm1::KeyConvertCowlClick(TObject *Sender)
{
  if (!Core) return;

  SelectionChange(NULL);
  ConvertCowl(Core);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyConvertCowlSClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;

  ConvertCowl(Core->Select);
  SelectionChange(NULL);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListNorthContextPopup(TObject *Sender,
      TPoint &MousePos, bool &Handled)
{
  if (Sender == ListNorth)
    Direction = rdNorth;
  else if (Sender == ListEast)
    Direction = rdEast;
  else if (Sender == ListWest)
    Direction = rdWest;
  else if (Sender == ListSouth)
    Direction = rdSouth;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PopupMenuAddPopup(TObject *Sender)
{
  bool sel = Core && Core->Select;
  bool cowl = sel && Core->Select->GetType() == ctCowl;
  KeyAddChip->Visible = sel && !cowl;
  KeyAddFrame->Visible = sel && !cowl;
  KeyAddWeight->Visible = sel && !cowl;
  KeyAddWheel->Visible = sel && !cowl;
  KeyAddRLW->Visible = sel && !cowl;
  KeyAddJet->Visible = sel && !cowl;
  KeyAddRudder->Visible = sel && !cowl;
  KeyAddRudderF->Visible = sel && !cowl;
  KeyAddTrim->Visible = sel && !cowl;
  KeyAddTrimF->Visible = sel && !cowl;
  KeyAddArm->Visible = sel && !cowl;
  KeyAddCowl->Visible = sel;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::KeyAddChipClick(TObject *Sender)
{
  if (!Core || !Core->Select)
    return;

  AnsiString cap = ((TMenuItem*)Sender)->Caption;
  if (Core->Select->GetType() == ctCowl && cap != "Cowl")
  {
    Application->MessageBox(("Can't join " + cap + " to cowl").c_str(), "cowl", MB_ICONERROR | MB_OK);
    return;
  }

  cap = "N:" + cap + "(){}";
  TRigidChip *chip = RCDLoader->StringToChip(cap);
  if (chip)
  {
    if (Direction != rdCore)
      chip->Direction = Direction;
    Core->Select->AddSubChip(chip);
  }
  if (KeySelectAdd->Checked)
    SelectionChange(chip);
  else
    SelectionChange(Core->Select);
  Modify = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonParentClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  SelectionChange(Core->Select->Parent);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListNorthClick(TObject *Sender)
{
  if (!Core || !Core->Select) return;
  if (((TListBox*)Sender)->ItemIndex == -1) return;
  SelectionChange((TRigidChip*)((TListBox*)Sender)->Items->Objects[((TListBox*)Sender)->ItemIndex]);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PageControl1Change(TObject *Sender)
{
  if (!Core) return;
  if (PageControl1->ActivePage == TabVal)
  {
    ListVariables->Items->Clear();
    for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
      ListVariables->Items->AddObject(i->first, (TObject*)i->second);
  }
  else if (PageControl1->ActivePage == TabKey)
  {
    ComboKeyVarName->Items->Clear();
    for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
      ComboKeyVarName->Items->AddObject(i->first, (TObject*)i->second);
    ComboKeyNoChange(Sender);
  }
  else if (PageControl1->ActivePage == TabScript)
  {
    if (Core->Script->ScriptText != MemoScript->Text)
      MemoScript->Text = Core->Script->ScriptText;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListVariablesClick(TObject *Sender)
{
  if (ListVariables->ItemIndex == -1) return;
  TRigidChipsVariable *var = (TRigidChipsVariable*)ListVariables->Items->Objects[ListVariables->ItemIndex];
  EditVarName->Text = ListVariables->Items->Strings[ListVariables->ItemIndex];
  EditVarDefault->Text = var->FlagColor ? "#"+IntToHex((int)var->Default, 6) : FloatToStr(var->Default);
  EditVarMin->Text = FloatToStr(var->Min);
  EditVarMax->Text = var->FlagMax ? FloatToStr(var->Max) : AnsiString();
  EditVarStep->Text = var->FlagStep ? FloatToStr(var->Step) : AnsiString();
  CheckVarDisp->Checked = var->Disp;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonVarDelClick(TObject *Sender)
{
  if (!Core) return;
  if (ListVariables->ItemIndex == -1) return;
  Core->Variables.erase(ListVariables->Items->Strings[ListVariables->ItemIndex]);
  Modify = true;
  PageControl1Change(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonVarOkClick(TObject *Sender)
{
  if (!Core) return;
  if (EditVarName->Text == "")
  {
    Application->MessageBox("Input variable name", "variable", MB_ICONINFORMATION | MB_OK);
    return;
  }

  EditVarName->Text = EditVarName->Text.UpperCase();
  TRigidChipsVariableMap::iterator i = Core->Variables.find(EditVarName->Text);
  TRigidChipsVariable *var;
  if (i != Core->Variables.end())
    var = i->second;
  else
  {
    var = new TRigidChipsVariable(Core, "");
    Core->Variables[EditVarName->Text] = var;
  }
  var->FlagColor = EditVarDefault->Text.SubString(1, 1) == "#";
  var->Default = 0;
  Core->StrToDouble(EditVarDefault->Text, &var->Default);
  var->Min = StrToFloatDef(EditVarMin->Text, 0);
  var->FlagMax = EditVarMax->Text != "";
  var->Max = StrToFloatDef(EditVarMax->Text, 0);
  var->FlagStep = EditVarStep->Text != "";
  var->Step = StrToFloatDef(EditVarStep->Text, 0);
  var->Disp = CheckVarDisp->Checked;

  Modify = true;
  PageControl1Change(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonVarColorClick(TObject *Sender)
{
  if (!Core) return;

  double d = 0xFFFFFF;
  Core->StrToDouble(EditVarDefault->Text, &d);
  int c = d;
  c = ((c&0xFF)<<16) | (c&0xFF00) | ((c&0xFF0000)>>16);
  ColorDialog->CustomColors->Text = "ColorA=" + IntToHex((int)c, 6);
  ColorDialog->Color = (TColor)c;
  if (ColorDialog->Execute())
  {
    c = ColorToRGB(ColorDialog->Color);
    c = ((c&0xFF)<<16) | (c&0xFF00) | ((c&0xFF0000)>>16);
    AnsiString s = "#"+IntToHex(c, 6);
    EditVarDefault->Text = s;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ComboKeyNoChange(TObject *Sender)
{
  if (!Core) return;
  ComboKeyNo->Text = IntToStr(ComboKeyNo->Text.ToIntDef(0));
  ListKeyVar->Items->Clear();
  for (int i = 0; i < Core->KeyList->Count; i ++)
  {
    if (Core->KeyList->Strings[i] == ComboKeyNo->Text)
      ListKeyVar->Items->AddObject(Core->Keys[i]->Variable, (TObject*)Core->Keys[i]);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListKeyVarClick(TObject *Sender)
{
  if (ListKeyVar->ItemIndex == -1) return;
  TRigidChipsKey *key = (TRigidChipsKey*)ListKeyVar->Items->Objects[ListKeyVar->ItemIndex];
  ComboKeyVarName->Text = key->Variable;
  EditKeyStep->Text = FloatToStr(key->Step);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonKeyDelClick(TObject *Sender)
{
  if (!Core) return;
  if (ListKeyVar->ItemIndex == -1) return;
  TRigidChipsKey *key = (TRigidChipsKey*)ListKeyVar->Items->Objects[ListKeyVar->ItemIndex];
  int i = Core->KeyList->IndexOfObject((TObject*)key);
  if (i != -1)
    Core->KeyList->Delete(i);
  Modify = true;
  PageControl1Change(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonKeyOkClick(TObject *Sender)
{
  if (!Core) return;
  if (ComboKeyVarName->Text == "")
  {
    Application->MessageBox("Input variable name", "key", MB_ICONINFORMATION | MB_OK);
    return;
  }

  ComboKeyNo->Text = IntToStr(ComboKeyNo->Text.ToIntDef(0));
  bool found = false;
  for (int i = 0; i < Core->KeyList->Count; i ++)
  {
    if (Core->KeyList->Strings[i] == ComboKeyNo->Text
     && Core->Keys[i]->Variable.UpperCase() == ComboKeyVarName->Text.UpperCase())
    {
      Core->Keys[i]->Step = StrToFloatDef(EditKeyStep->Text, 0);
      found = true;
    }
  }
  
  if (!found)
  {
    TRigidChipsKey *key = new TRigidChipsKey(Core, ComboKeyVarName->Text, "step="+EditKeyStep->Text);
    Core->AddKey(ComboKeyNo->Text, key);
  }

  Modify = true;
  PageControl1Change(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonScriptResetClick(TObject *Sender)
{
  if (!Core) return;
  if (Core->Script->ScriptText != MemoScript->Text)
  {
    Core->Script->ScriptText = MemoScript->Text;
    Modify = true;
  }
  for (TRigidChipsVariableMap::iterator i = Core->Variables.begin(); i != Core->Variables.end(); i ++)
    i->second->Value = i->second->Default;
  ScriptOut->Clear();
  RunScript = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonScriptStopClick(TObject *Sender)
{
  RunScript = false;
}
//---------------------------------------------------------------------------
int CheckPanekitFile(int fp)
{ // plLbgÌt@C©`FbN
  char c;

  char *match = "SC\x13\x04plLbg";
  int len = AnsiString(match).Length();
  int pos = 0;
  // match¶ñðTµÈªçXLbv
  while (FileRead(fp, &c, 1) == 1)
  {
    if (c == match[pos])
    {
      pos ++;
      if (pos == len)
        break;
    }
    else
    {
      FileSeek(fp, -pos, 1);
      pos = 0;
    }
  }

  if (pos != len)
  {
    FileSeek(fp, -pos, 1);
    return -1;
  }
  return FileSeek(fp, -pos, 1);
}
const char* const PanekitStringMap1 =
"@??????????????????????????????OPQRSTUVWX??????????????`abcdefghijklmn"
"opqrstuvwxyabcde????????????"
" ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎ"
"ÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñ@ABCDEFGHIJKL"
"MNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvÖ×Øz{|"
"}~??ABCDEFGHIJLMNOPQRSTUV"
"WXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~??"
" ¡¢£¤¥¦§¨©ª«¬????????TUVWXY"
"Z[\]??êñOlÜZµªã\SçëëóQÞEäÝb³¸Oã¶Eãºå¬áàO½­dyø"
"Uhè«¤CóúÎØàynÑRÄ§ÉÆI^üVæàÛÅÔ^®Ô@òsí¼p®Í"
"ìdl??³ùñ@~tSü¡`³Àå½­ã»SâºóDøplõê³G`à|_¬³´É"
"°Ôa¯KEeFEÒöb°´éhÖøSn~ÕPçT@Rmd¸ï`¯»ôj¹¶½Åe"
"¤cégk«²ÑR·ºgÂ©ÎtÄH~¼ìk\ s{§Ssæ¬ºjÔìãc"
"¡lL{¨cÌß±ÍÅçìt]Z|îXeå¼ÊkírqØ÷¾¡ºaû¬]ËÃã´"
"n¢NªbéI~}Ù­i«ÀÍpx@¾YÕ¬²å³êVvÇ¤óôePü¡åjVK"
"±@¹ìZ??EY³cÆ¥gêA·å«âÎsvc`HìÓC°yï¯nQPur¢¹Më"
"awé§ v¶fëiÁó¾ðgY¬ ¹ªõöC£|¿÷ØÔÙlRÜMäÈwÅI"
"¼}zd]¢EÕN¥Á????????"
;
const char* const PanekitStringMap2 =
"L2R2L1R1¢~ ª¨«© ß¶G"
"owxylsirb??????????????"
;
AnsiString ReadPanekitString(int fp, int len)
{
  AnsiString n;
  bool pre = true;
  unsigned char c1, c2;
  int i;
  while (len-- > 0 && FileRead(fp, &c1, 1) == 1)
  {
    if (pre)
    {
      if (len-- <= 0 || FileRead(fp, &c2, 1) != 1) return n;
      i = (c1 << 4) | (c2 >> 4);
    }
    else
    {
      i = ((c2 & 0xF) << 8) | c1;
    }
    if (i < 0x340)
    {
      i *= 2;
      n += AnsiString(PanekitStringMap1[i]) + AnsiString(PanekitStringMap1[i+1]);
    }
    else if (0x400 <= i && i < 0x420)
    {
      i = (i - 0x400) * 2;
      n += AnsiString(PanekitStringMap2[i]) + AnsiString(PanekitStringMap2[i+1]);
    }
    pre = !pre;
  }
  return n;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonPanekitBrowseClick(TObject *Sender)
{
  OpenDialogPanekit->FileName = EditPanekitFile->Text;
  if (!OpenDialogPanekit->Execute())
    return;
  
  EditPanekitFile->Text = OpenDialogPanekit->FileName;

  if (EditPanekitFile->Text == "")
  {
    Application->MessageBox("Select panekit file", "LoadList", MB_OK | MB_ICONINFORMATION);
    return;
  }

  ListPanekit->Items->Clear();
  MemoPanekit->Lines->Clear();

  int fp = FileOpen(EditPanekitFile->Text, fmOpenRead | fmShareDenyWrite);
  if (fp == -1)
  {
    Application->MessageBox("Can't open panekit file", "LoadList", MB_OK | MB_ICONERROR);
    return;
  }

  int pos = CheckPanekitFile(fp);
  if (pos == -1)
  {
    FileClose(fp);
    Application->MessageBox("This file is not panekit file", "LoadList", MB_OK | MB_ICONERROR);
    return;
  }

  FileSeek(fp, 0x600, 1);

  // ¼OÌÇÝÝ
  for (int i = 0; i < 32; i ++)
  {
    AnsiString n = ReadPanekitString(fp, 15);
    if (n == "") n = "-";
    ListPanekit->Items->Add(n);
  }

  // «ë­¤f
  FileSeek(fp, pos, 0);
  FileSeek(fp, 0x800, 1);
  FileSeek(fp, 0x380, 1);
  AnsiString n;
  for (int i = 0; i < 10; i ++)
  {
    unsigned short c;
    FileRead(fp, &c, 2);
    if (c < 0x340)
    {
      c *= 2;
      n += AnsiString(PanekitStringMap1[c]) + AnsiString(PanekitStringMap1[c+1]);
    }
    else if (0x400 <= c && c < 0x420)
    {
      c = (i - 0x400) * 2;
      n += AnsiString(PanekitStringMap2[c]) + AnsiString(PanekitStringMap2[c+1]);
    }
  }
  ListPanekit->Items->Add(n);

  FileClose(fp);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ListPanekitClick(TObject *Sender)
{
  KeyNewClick(Sender);
  if (Modify)
  {
    ListPanekit->ItemIndex = -1;
    return;
  }

  MemoPanekit->Lines->Clear();

  if (ListPanekit->ItemIndex == -1 || ListPanekit->Items->Strings[ListPanekit->ItemIndex] == "-")
    return;

  if (EditPanekitFile->Text == "")
  {
    ListPanekit->Items->Clear();
    Application->MessageBox("Select panekit file", "LoadModel", MB_OK | MB_ICONINFORMATION);
    return;
  }

  int fp = FileOpen(EditPanekitFile->Text, fmOpenRead | fmShareDenyWrite);
  if (fp == -1)
  {
    ListPanekit->Items->Clear();
    Application->MessageBox("Can't open panekit file", "LoadModel", MB_OK | MB_ICONERROR);
    return;
  }

  if (CheckPanekitFile(fp) == -1)
  {
    FileClose(fp);
    ListPanekit->Items->Clear();
    Application->MessageBox("This file is not panekit file", "LoadModel", MB_OK | MB_ICONERROR);
    return;
  }

  if (ListPanekit->ItemIndex < 32)
  { // f
    FileSeek(fp, 0x1000, 1);
    FileSeek(fp, 0x380 * ListPanekit->ItemIndex, 1);
  }
  else
  { // «ë­¤f
    FileSeek(fp, 0x800, 1);
  }
  
  // o[W
  FileSeek(fp, 4, 1);
  // GfBbgñ
  FileSeek(fp, 4, 1);
  // GfBbgÔ[1/60s]
  FileSeek(fp, 4, 1);
  // H
  FileSeek(fp, 4, 1);

  // RAîñ{{^gptO
  unsigned char coreflag;
  FileRead(fp, &coreflag, 1);
  int corecon = coreflag & 0xF;
  // {^gptO
  FileSeek(fp, 1, 1);
  // ep[cgp
  FileSeek(fp, 9, 1);

  // ep[cîñ 7bytes x 99
  int angles[] = {
        0, -165, -150, -135, -120, -105, -90, -75, -60, -45, -30, -15,
        0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165,
  };
  int colors[] = {
        0xC0C0C0, 0x808080, 0x404040, 0x202020,
        0xFF2020, 0xFF0000, 0x800000, 0x402020,
        0x400000, 0x802000, 0xC0A080, 0xC08000,
        0xFF8000, 0xA08000, 0xA0A000, 0x80A000,
        0x002000, 0x004000, 0x408000, 0x008000,
        0x004040, 0x008040, 0x00A080, 0x8080A0,
        0x202040, 0x404080, 0x4040C0, 0x000000,
        0x402040, 0x802080, 0x804080, 0xFF80A0,
  };
  int buttons[] = {
        7,  // L2¨A
        12, // R2¨F
        8,  // L1¨S
        9,  // R1¨D
        10, // ¢¨V
        6,  // ¨C
        5,  // ~¨X
        4,  //  ¨Z
        -1, -1, -1, -1,
        0,  // ª
        3,  // ¨
        1,  // «
        2,  // ©
  };
  AnsiString err;
  std::stack<TRigidChip*> stackChip;
  std::stack<int> stackCon;
  std::stack<bool> stackInvertNS, stackInvertWE;
  for (int i = 0; err == ""; )
  {
    TRigidChipsDirection dir;
    TRigidChip *parent = Core;
    if (corecon & 1)
    {
      dir = rdNorth;
      corecon &= ~1;
    }
    else if (corecon & 2)
    {
      dir = rdEast;
      corecon &= ~2;
    }
    else if (corecon & 4)
    {
      dir = rdSouth;
      corecon &= ~4;
    }
    else if (corecon & 8)
    {
      dir = rdWest;
      corecon &= ~8;
    }
    else // if (dir == rdCore)
    {
      FileSeek(fp, (99-i)*7, 1);
      break;
    }
    
    const TRigidChipsDirection dirmap[2][2][5] = {
      { // invertNS = false
        {rdCore, rdNorth, rdEast, rdSouth, rdWest}, // invertWE = false
        {rdCore, rdNorth, rdWest, rdSouth, rdEast}, // invertWE = true
      },
      {
        {rdCore, rdSouth, rdEast, rdNorth, rdWest}, // invertWE = false
        {rdCore, rdSouth, rdWest, rdNorth, rdEast}, // invertWE = true
      },
    };
    bool invertNS = false, invertWE = false;
    
    while (i++ < 99)
    {
      TRigidChip *chip;
      unsigned char data[7];
      FileRead(fp, data, 7);

      // Ú±óÔ(0:Èµ 1:¼i, 2:E 4:¶ ÉÚ± è)Ì_a
      int con = data[0] >> 4;

      // px
      int angle = data[3] & 0x1F;
      angle = angle < 24 ? angles[angle] : 0;
      if (!(invertNS ^ invertWE)) // PANEKITÆRCÍt
        angle *= -1;

      // {^
      int button[] = {8,
        data[1]&0xF,
        data[1]>>4,
        data[2]&0xF,
        data[2]>>4,
      };
      for (int j = 0; j < 5; j ++)
        button[j] = button[j] < 0x10 ? buttons[button[j]] : -1;
      // p[^(plÈO)
      int param[] = {
        ((data[3]&0xC0)>>3) | (data[6]>>5),
        data[4] & 0x1F,
        ((data[5]&3)<<3) | (data[4]>>5),
        (data[5]>>2) & 0x1F,
        data[6] & 0x1F,
      };
      int hold = data[5]>>7;

      AnsiString opt;

      // p[cíÞ
      int type = data[0] & 0xF;
      if (type == 2)
      { // pl
        // F
        int color = data[4];
        color = color < 0x20 ? colors[color] : 0xFFFFFF;

        // ¿
        // chip[5];
        // 0x80
        // chip[6];

        chip = new TRigidChipChip;
        if (angle != 0) opt += AnsiString(opt != "" ? "," : "") + "angle=" + IntToStr(angle);
        if (color != 0xFFFFFF) opt += AnsiString(opt != "" ? "," : "") + "color=#" + IntToHex(color,6);
      }
      else if (type == 3)
      { // XWCg
        // «üè
        chip = new TRigidChipChip;
        if (angle != 0) chip->Options->Values["angle"] = IntToStr(angle);
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);

        chip = new TRigidChipFrame;
        chip->Options->Values["angle"] = IntToStr(-75+angle);
        chip->Options->Values["damper"] = "1";
        chip->Options->Values["option"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
        chip = new TRigidChipFrame;
        chip->Options->Values["angle"] = "150";
        chip->Options->Values["damper"] = "1";
        chip->Options->Values["option"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;

        chip = new TRigidChipFrame;

        for (int j = 0; j < 5; j ++)
        {
          param[j] = param[j] < 24 ? angles[param[j]] : 0;
          // PANEKITÆRCÍt
          if (!(invertNS ^ invertWE)) param[j] *= -1;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*5));
          Core->Variables["JOINT"+IntToStr(i)] = var;
          chip->Options->Values["angle"] = "JOINT"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "JOINT"+IntToStr(i), "step="+IntToStr((param[j]-param[0])/5)));
        }
        else
          if (param[0] != 0) chip->Options->Values["angle"] = IntToStr(param[0]);
        if (hold == 0) chip->Options->Values["spring"] = "0.5";

        chip->Options->Values["option"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
        opt = "";
        chip = new TRigidChipFrame;
        chip->Options->Values["angle"] = "-150";
        chip->Options->Values["damper"] = "1";
        chip->Options->Values["option"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;

        chip = new TRigidChipChip;
        chip->Options->Values["angle"] = "-105";
        chip->Options->Values["damper"] = "1";
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;

        chip = NULL;
        if (dir == rdNorth || dir == rdSouth) invertNS = !invertNS;
        if (dir == rdEast || dir == rdWest) invertWE = !invertWE;
      }
      else if (type == 4 || type == 5)
      { // Y/ZWCg
        if (CheckPanekitTruly->Checked)
        {
          chip = new TRigidChipFrame;
          opt += AnsiString(opt != "" ? "," : "") + "angle=" + IntToStr(-105+angle);
          opt += AnsiString(opt != "" ? "," : "") + "damper=1";
          opt += AnsiString(opt != "" ? "," : "") + "option=1";
          chip->SetOptions(opt);
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          opt = "";
          chip = new TRigidChipFrame;
          chip->SetOptions("angle=-150,damper=1,option=1");
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          chip = new TRigidChipChip;
          chip->SetOptions("angle=-105,damper=1");
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
        }
        else if (angle != 0)
        {
          chip = new TRigidChipFrame;
          opt += AnsiString(opt != "" ? "," : "") + "angle=" + IntToStr(180+angle);
          opt += AnsiString(opt != "" ? "," : "") + "damper=1";
          opt += AnsiString(opt != "" ? "," : "") + "option=1";
          chip->SetOptions(opt);
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          opt = "";
          if (dir == rdNorth || dir == rdSouth) invertNS = !invertNS;
          if (dir == rdEast || dir == rdWest) invertWE = !invertWE;
        }

        if (type == 4)
          chip = new TRigidChipRudder;
        else // if (type == 5)
          chip = new TRigidChipTrim;

        for (int j = 0; j < 5; j ++)
        {
          param[j] = param[j] < 24 ? angles[param[j]] : 0;
          // PANEKITÆRC¯¶ATrimÍ½]µÈ¢
          if (type == 4 && invertNS ^ invertWE) param[j] *= -1;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*5));
          Core->Variables["JOINT"+IntToStr(i)] = var;
          opt += AnsiString(opt != "" ? "," : "") + "angle=JOINT"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "JOINT"+IntToStr(i), "step="+IntToStr((param[j]-param[0])/5)));
        }
        else
          if (param[0] != 0) opt += AnsiString(opt != "" ? "," : "") + "angle=" + IntToStr(param[0]);
        if (hold == 0) opt += AnsiString(opt != "" ? "," : "") + "spring=0.5";

        if (CheckPanekitTruly->Checked)
        {
          chip->SetOptions(opt);
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          opt = "";

          chip = new TRigidChipFrame;
          chip->SetOptions("angle=-105,damper=1,option=1");
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          opt = "";
          chip = new TRigidChipFrame;
          chip->SetOptions("angle=-150,damper=1,option=1");
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;
          chip = new TRigidChipChip;
          chip->SetOptions("angle=75,damper=1");
          chip->Direction = dirmap[invertNS][invertWE][dir];
          parent->AddSubChip(chip);
          parent = chip;

          chip = NULL;
          if (dir == rdNorth || dir == rdSouth) invertNS = !invertNS;
          if (dir == rdEast || dir == rdWest) invertWE = !invertWE;
        }
      }
      else if (type == 6)
      { // [^[
        chip = new TRigidChipFrame;
        opt += AnsiString(opt != "" ? "," : "") + "angle=" + IntToStr(15+angle);
        opt += AnsiString(opt != "" ? "," : "") + "damper=1";
        opt += AnsiString(opt != "" ? "," : "") + "option=1";
        chip->SetOptions(opt);
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
        opt = "";
        chip = new TRigidChipFrame;
        chip->SetOptions("angle=150,damper=1,option=1");
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;

        chip = new TRigidChipWheel;
        opt += AnsiString(opt != "" ? "," : "") + "angle=105";

        for (int j = 0; j < 5; j ++)
        {
          param[j] -= 0x0C;
          param[j] *= 5000;
          // [^[Í½]µÈ¢
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*5000));
          Core->Variables["MOTOR"+IntToStr(i)] = var;
          opt += AnsiString(opt != "" ? "," : "") + "power=MOTOR"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "MOTOR"+IntToStr(i), "step="+IntToStr(param[j]-param[0])));
        }
        else
          if (param[0] != 0) opt += AnsiString(opt != "" ? "," : "") + "power=" + IntToStr(param[0]);
        if (hold == 0) opt += AnsiString(opt != "" ? "," : "") + "spring=0.5";

        chip->SetOptions(opt);
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
        opt = "";

        chip = new TRigidChipFrame;
        chip->SetOptions("angle=105,damper=1,option=1");
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
        chip = new TRigidChipFrame;
        chip->SetOptions("angle=150,damper=1,option=1");
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
        parent = chip;
        chip = new TRigidChipFrame;
        opt = "angle=-165,option=0";
      }
      else if (type == 7)
      { // ^C
        chip = new TRigidChipWheel;
        if (angle != 0) opt += AnsiString(opt != "" ? "," : "") + "angle=" + IntToStr(angle);

        for (int j = 0; j < 5; j ++)
        {
          param[j] -= 0x0C;
          param[j] *= 500;
          if (invertNS ^ invertWE) param[j] *= -1;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*500));
          Core->Variables["TIRE"+IntToStr(i)] = var;
          opt += AnsiString(opt != "" ? "," : "") + "power=TIRE"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "TIRE"+IntToStr(i), "step="+IntToStr(param[j]-param[0])));
        }
        else
          if (param[0] != 0) opt += AnsiString(opt != "" ? "," : "") + "power=" + IntToStr(param[0]);
      }
      else if (type == 8)
      { // WFbg
        chip = new TRigidChipJet;
        if (angle != 0) opt += AnsiString(opt != "" ? "," : "") + "angle=" + IntToStr(angle);

        for (int j = 0; j < 5; j ++)
        {
          param[j] -= 0x0C;
          param[j] *= 10000;
          if (invertNS ^ invertWE) param[j] *= -1;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(hold*5000));
          Core->Variables["JET"+IntToStr(i)] = var;
          opt += AnsiString(opt != "" ? "," : "") + "power=JET"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "JET"+IntToStr(i), "step="+IntToStr(param[j]-param[0])));
        }
        else
          if (param[0] != 0) opt += AnsiString(opt != "" ? "," : "") + "power=" + IntToStr(param[0]);
      }
      else if (type == 9)
      { // V[^[
        chip = new TRigidChipArm;
        if (angle != 0) opt += AnsiString(opt != "" ? "," : "") + "angle=" + IntToStr(angle);

        for (int j = 0; j < 5; j ++)
        {
          param[j] -= 0x0C;
          param[j] *= 25000;
        }
        if (button[1] != -1 || button[2] != -1 || button[3] != -1 || button[4] != -1)
        {
          // ÏÌÇÁ
          int min = 99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] < min) min = param[j];
          int max = -99999;
          for (int j = 0; j < 5; j ++)
            if (j == 0 || button[j] != -1 && param[j] > max) max = param[j];
          TRigidChipsVariable *var = new TRigidChipsVariable(Core, "default="+IntToStr(param[0])+",min="+IntToStr(min)+",max="+IntToStr(max)+",step="+IntToStr(max));
          Core->Variables["SHOT"+IntToStr(i)] = var;
          opt += AnsiString(opt != "" ? "," : "") + "option="+IntToStr(max);
          opt += AnsiString(opt != "" ? "," : "") + "power=SHOT"+IntToStr(i);
          // L[ÌÇÁ
          for (int j = 1; j < 5; j ++)
            if (button[j] != -1) Core->AddKey(IntToStr(button[j]), new TRigidChipsKey(Core, "SHOT"+IntToStr(i), "step="+IntToStr(param[j]-param[0])));
        }
        else
          if (param[0] != 0) opt += AnsiString(opt != "" ? "," : "") + "option=" + IntToStr(param[0]) + ",power=" + IntToStr(param[0]);
      }
      else
      {
        err = "¢mÌp[c^CvÔ(" + IntToStr(type) + ")";
        break;
      }
      if (chip)
      {
        chip->SetOptions(opt);
        chip->Direction = dirmap[invertNS][invertWE][dir];
        parent->AddSubChip(chip);
      }
      
      if (con == 0)
      { // Ú±Èµ(s«~Üè)
        if (stackChip.size() == 0)
          break;

        chip = stackChip.top(); stackChip.pop();
        con = stackCon.top(); stackCon.pop();
        invertNS = stackInvertNS.top(); stackInvertNS.pop();
        invertWE = stackInvertWE.top(); stackInvertWE.pop();
        dir = dirmap[invertNS][invertWE][chip->Direction];
      }
      if (con & 1)
      { // ¼iûüÉÚ± è
        con &= ~1;
      }
      else if (con & 2)
      { // EÉÚ± è
        con &= ~2;
        dir = (TRigidChipsDirection)((dir % 4) + 1);
      }
      else if (con & 8)
      { // ¶ÉÚ± è
        con &= ~8;
        dir = (TRigidChipsDirection)(((dir - 2 + 4) % 4) + 1);
      }
      if (chip)
        parent = chip;
      if (con)
      { // ¼ÉàÚ± è
        stackChip.push(parent);
        stackCon.push(con);
        stackInvertNS.push(invertNS);
        stackInvertWE.push(invertWE);
      }
    }
  }

  // dlRg
  if (err == "")
    for (int i = 0; i < 8; i ++)
      MemoPanekit->Lines->Add(ReadPanekitString(fp, 15));
  else
  {
    MemoPanekit->Lines->Add(err);
    MemoPanekit->Lines->Add("t@CÊu: 0x" + IntToHex(FileSeek(fp, 0, 1), 4));
  }

  FileClose(fp);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CheckPanekitTrulyClick(TObject *Sender)
{
  ListPanekitClick(Sender);
}
//---------------------------------------------------------------------------

