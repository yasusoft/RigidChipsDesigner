object FormOptionSave: TFormOptionSave
  Left = 615
  Top = 109
  Width = 488
  Height = 327
  BorderIcons = []
  Caption = 'Save Options'
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  DesignSize = (
    480
    300)
  PixelsPerInch = 96
  TextHeight = 12
  object GroupForm: TGroupBox
    Left = 300
    Top = 5
    Width = 175
    Height = 190
    Anchors = [akTop, akRight]
    Caption = 'Save &Form'
    TabOrder = 1
    object CheckSpaceBlockType: TCheckBox
      Left = 10
      Top = 20
      Width = 150
      Height = 15
      Caption = 'Space after BlockType'
      TabOrder = 0
      OnClick = CheckSpaceBlockTypeClick
    end
    object CheckNewLineBlockType: TCheckBox
      Tag = 1
      Left = 10
      Top = 40
      Width = 150
      Height = 15
      Caption = 'NewLine after BlockType'
      TabOrder = 1
      OnClick = CheckSpaceBlockTypeClick
    end
    object CheckSpaceOptions: TCheckBox
      Tag = 2
      Left = 10
      Top = 60
      Width = 150
      Height = 15
      Caption = 'Space after Options'
      TabOrder = 2
      OnClick = CheckSpaceBlockTypeClick
    end
    object CheckNewLineOptions: TCheckBox
      Tag = 3
      Left = 10
      Top = 80
      Width = 150
      Height = 15
      Caption = 'NewLine after Options'
      TabOrder = 3
      OnClick = CheckSpaceBlockTypeClick
    end
    object CheckSpaceComma: TCheckBox
      Tag = 4
      Left = 10
      Top = 100
      Width = 150
      Height = 15
      Caption = 'Space after comma'
      TabOrder = 4
      OnClick = CheckSpaceBlockTypeClick
    end
    object CheckSubNewLine: TCheckBox
      Tag = 5
      Left = 10
      Top = 120
      Width = 150
      Height = 15
      Caption = 'No SubChip, No NewLine'
      TabOrder = 5
      OnClick = CheckSpaceBlockTypeClick
    end
    object RadioTab: TRadioButton
      Left = 10
      Top = 145
      Width = 60
      Height = 15
      Caption = 'Tab'
      TabOrder = 6
      OnClick = RadioTabClick
    end
    object RadioSpace: TRadioButton
      Left = 10
      Top = 165
      Width = 60
      Height = 15
      Caption = 'Space x'
      TabOrder = 7
      OnClick = RadioTabClick
    end
    object EditSpace: TEdit
      Left = 75
      Top = 160
      Width = 50
      Height = 20
      ImeMode = imDisable
      TabOrder = 8
      Text = '2'
      OnChange = RadioTabClick
    end
  end
  object MemoPreview: TMemo
    Left = 5
    Top = 5
    Width = 290
    Height = 290
    Anchors = [akLeft, akTop, akRight, akBottom]
    Color = clBtnFace
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = #65325#65331' '#12468#12471#12483#12463
    Font.Style = []
    ImeMode = imDisable
    Lines.Strings = (
      '// Preview Model'
      '// This is ModelMemo'
      'Val{'
      ' VAR1(default=123,max=1234)'
      ' VAR2(default=123,max=1234)'
      '}'
      'Key{'
      ' 0:VAR1(step=5),VAR2(step=5)'
      ' 1:VAR1(step=-5),VAR2(step=-5)'
      '}'
      'Body{'
      ' // kore wa core'
      ' Core(){'
      '  // this is ChipMemo for hoge'
      '  S:Chip(angle=90,name=hoge){'
      '   // cooooowl'
      '   Cowl(angle=VAR1){}'
      '   // jeeeeet'
      '   // balooooon'
      '   W:Jet(power=VAR2,option=1){}'
      '  }'
      ' }'
      '}')
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 0
    WantReturns = False
    WordWrap = False
  end
  object ButtonOk: TButton
    Left = 375
    Top = 270
    Width = 100
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = '&OK'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
end
