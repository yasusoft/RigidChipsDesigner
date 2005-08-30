object Form1: TForm1
  Left = 193
  Top = 108
  AutoScroll = False
  Caption = 'RigidChips Designer'
  ClientHeight = 490
  ClientWidth = 600
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
    Height = 480
    Anchors = [akTop, akRight, akBottom]
    BevelOuter = bvNone
    TabOrder = 1
    DesignSize = (
      105
      480)
    object ButtonOpen: TButton
      Left = 5
      Top = 0
      Width = 100
      Height = 25
      Caption = #38283#12369#12468#12510
      TabOrder = 0
      OnClick = ButtonOpenClick
    end
    object ButtonSendRC: TButton
      Left = 5
      Top = 35
      Width = 100
      Height = 25
      Caption = 'RigidChips'#12395#35501#36796
      TabOrder = 1
      OnClick = ButtonSendRCClick
    end
    object EditKeyTest: TEdit
      Left = 5
      Top = 460
      Width = 100
      Height = 20
      Anchors = [akLeft, akBottom]
      ImeMode = imDisable
      ReadOnly = True
      TabOrder = 4
      Text = 'KeyTest'
      OnKeyDown = EditKeyTestKeyDown
      OnKeyUp = EditKeyTestKeyUp
    end
    object PanelReload: TPanel
      Left = 5
      Top = 75
      Width = 100
      Height = 70
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 3
      object LabelReload: TLabel
        Left = 10
        Top = 20
        Width = 71
        Height = 12
        Caption = #35469#35672#38291#38548'[ms]'
        FocusControl = EditReload
      end
      object EditReload: TEdit
        Left = 10
        Top = 35
        Width = 80
        Height = 20
        ImeMode = imDisable
        TabOrder = 0
        Text = '1000'
      end
    end
    object CheckReload: TCheckBox
      Left = 10
      Top = 70
      Width = 85
      Height = 15
      Caption = #33258#21205#12522#12525#12540#12489
      TabOrder = 2
      OnClick = CheckReloadClick
    end
    object CheckGhost: TCheckBox
      Left = 5
      Top = 435
      Width = 80
      Height = 15
      Anchors = [akLeft, akBottom]
      Caption = 'ShowGhost'
      TabOrder = 5
      OnClick = CheckGhostClick
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
  object TimerReload: TTimer
    Enabled = False
    Interval = 10000
    OnTimer = TimerReloadTimer
    Left = 10
    Top = 45
  end
end
