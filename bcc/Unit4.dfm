object Form4: TForm4
  Left = 0
  Top = 0
  Caption = 'Form4'
  ClientHeight = 411
  ClientWidth = 734
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 605
    Top = 71
    Width = 3
    Height = 13
  end
  object conSource: TEdit
    Left = 605
    Top = 8
    Width = 121
    Height = 21
    TabOrder = 0
    Text = 'rtsp://localhost:8554/'
  end
  object Button1: TButton
    Left = 605
    Top = 35
    Width = 121
    Height = 30
    Caption = 'Connect'
    TabOrder = 1
    OnClick = Button1Click
  end
end
