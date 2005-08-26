object Form1: TForm1
  Left = 192
  Top = 107
  Width = 608
  Height = 517
  Caption = 'RigidChips Designer'
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnMouseWheel = FormMouseWheel
  DesignSize = (
    600
    490)
  PixelsPerInch = 96
  TextHeight = 12
  object PaintPanelDummy: TPanel
    Left = 5
    Top = 5
    Width = 480
    Height = 480
    BevelOuter = bvNone
    TabOrder = 0
    Visible = False
    OnMouseDown = PaintPanelMouseDown
    OnMouseMove = PaintPanelMouseMove
    OnMouseUp = PaintPanelMouseUp
    OnResize = PaintPanelResize
  end
  object PanelRight: TPanel
    Left = 490
    Top = 5
    Width = 105
    Height = 200
    Anchors = [akTop, akRight]
    BevelOuter = bvNone
    TabOrder = 1
    object ButtonOpen: TButton
      Left = 5
      Top = 5
      Width = 100
      Height = 25
      Caption = #38283#12369#12504#12477#12398#12468#12510
      TabOrder = 0
      OnClick = ButtonOpenClick
    end
  end
  object Timer1: TTimer
    Interval = 25
    OnTimer = Timer1Timer
    Left = 10
    Top = 10
  end
  object OpenDialog: TOpenDialog
    FileName = '*.rcd'
    Filter = 'RigidChipFiles(*.rcd,*.txt)|*.rcd;*.txt|All Files(*.*)|*.*'
    Title = 'open'
    Left = 45
    Top = 10
  end
end
