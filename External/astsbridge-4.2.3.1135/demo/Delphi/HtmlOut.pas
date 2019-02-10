unit HtmlOut;

interface

uses
  Classes, Graphics;

type
  THtmlTag = (htHtml, htHead, htBody, htTitle, htStyle,
    htH1, htH2, htH3, htH4, htH5, htH6, htNumbering, htBulleting, htTable,
    htRow, htColumn, htBold, htItalic,
    htUnderline, htTypeWriter, htSmall);
  THtmlTags = set of THtmlTag;

  THtmlWriter = class
  private
    FStream: TStream;
    FOpenTags: TList;
//    FFreeOnTerminate: Boolean;
  public
    procedure Body(Color: TColor = -1);
    procedure Bookmark(const Name: string);
    constructor Create(FileName: string); overload;
    constructor Create(Stream: TStream); overload;
    destructor Destroy; override;
    function EndTag: Boolean;
    procedure Head;
    procedure HRef(const Msg, Ref: string);
    procedure Indent;
    procedure ListItem;
    procedure NewLine;
    procedure Paragraph;
    procedure Write(const S: string);
    procedure Writeln(const S: string = '');
    procedure Table(Spacing, Padding, Border, AWidth: Integer);
    procedure TableRow(Color: TColor = -1);
    procedure TableCol(const Msg: string; Align: TAlignment = taLeftJustify);
    procedure TableColW(const Msg: string; Width: Integer;
      Align: TAlignment = taLeftJustify);
    procedure Tag(Tag: THtmlTag);
    procedure TaggedText(const Text: string; Tags: THtmlTags);
    procedure Separator;
    procedure Unindent;
    property Stream: TStream read FStream;
  end;

const
  CrLf        = #13#10;
  NbSp        = '&nbsp;';

implementation

uses
  SysUtils;

const
  HtmlTags: array [THtmlTag] of string =
    ('html', 'head', 'body', 'title', 'style',
      'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'ul', 'ol', 'table', 'tr', 'td',
      'b', 'i', 'u', 'tt', 'small');

const
  SAlignes: array [TAlignment] of string = ('left', 'right', 'center');

function SeparateLineTag(Tag: THtmlTag): Boolean;
begin
  Result := Tag < htBold;
end;

function GetColor(Color: TColor): string;
begin
  Result := '"#' + LowerCase(IntToHex(Color, 6)) + '"';
end;

{ THtmlWriter }

procedure THtmlWriter.Body(Color: TColor = -1);
begin
  if Color = -1 then Write('<body>' + CrLf)
  else Write(Format('<body bgcolor=%s>' + CrLf, [GetColor(Color)]));
  FOpenTags.Add(Pointer(htBody));
end;

constructor THtmlWriter.Create(Stream: TStream);
begin
  inherited Create;
  FStream := Stream;
  FOpenTags := TList.Create;
  Tag(htHtml);
end;

constructor THtmlWriter.Create(FileName: string);
begin
//  FFreeOnTerminate := True;
  Create(TFileStream.Create(FileName, fmCreate));
end;

destructor THtmlWriter.Destroy;
begin
  if Assigned(FOpenTags) then while EndTag do;
//  if FFreeOnTerminate then
    FStream.Free;
  FOpenTags.Free;
  inherited;
end;

function THtmlWriter.EndTag: Boolean;
var
  Idx: Integer;
  Tag: THtmlTag;
  S: string;
begin
  Idx := FOpenTags.Count - 1;
  if Idx >= 0 then
  begin
    Tag := THtmlTag(FOpenTags[Idx]);
    FOpenTags.Delete(Idx);
    S := '</' + HtmlTags[Tag] + '>';
    if SeparateLineTag(Tag) then S := S + CrLf;
    Write(S);
    Result := True;
  end
  else Result := False;
end;

procedure THtmlWriter.Head;
begin
  Tag(htHead);
end;

procedure THtmlWriter.Paragraph;
begin
  Write('<p>' + CrLf);
end;

procedure THtmlWriter.Separator;
begin
  Writeln('<hr>');
end;

procedure THtmlWriter.Write(const S: string);
begin
  FStream.WriteBuffer(Pointer(S)^, Length(S));
end;

procedure THtmlWriter.Writeln(const S: string = '');
begin
  Write(S + CrLf);
end;

procedure THtmlWriter.Tag(Tag: THtmlTag);
var
  S: string;
  P: Integer;
begin
  S := '<' + HtmlTags[Tag] + '>';
  if SeparateLineTag(Tag) then S := S + CrLf;
  Write(S);
  FOpenTags.Add(Pointer(Tag));

  if Tag = htHead then
  begin
    S := ExtractFileName(ParamStr(0));
    P := LastDelimiter('.', S);
    if P > 0 then Delete(S, P, MaxInt);
//    Writeln('<meta http-equiv="content-type" content="text/html; charset=windows-1251">');
    Writeln('<meta name="Generator" content="' + S +
      ' (by THtmlWriter for Delphi)">');
    Writeln('<meta name="Author" content="Gennadie Zuev (zuev@micex.com)">');
  end;
end;

procedure THtmlWriter.TaggedText(const Text: string; Tags: THtmlTags);
var
  Prefix, Suffix, STag: string;
  I: THtmlTag;
begin
  Prefix := '';
  Suffix := '';
  for I := Low(THtmlTag) to High(THtmlTag) do
    if I in Tags then
    begin
      Prefix := Prefix + '<' + HtmlTags[I] + '>';
      STag := '</' + HtmlTags[I] + '>';
      if SeparateLineTag(I) then STag := STag + CrLf;
      Suffix := STag + Suffix;
    end;
  Write(Prefix + Text + Suffix);
end;

procedure THtmlWriter.HRef(const Msg, Ref: string);
var
  S: string;
begin
  FmtStr(S, '<a href="%s">%s</a>', [Ref, Msg]);
  Write(S);
end;

procedure THtmlWriter.Bookmark(const Name: string);
begin
  Writeln(Format('<a name="%s"></a>', [Name]));
end;

procedure THtmlWriter.NewLine;
begin
  Write('<br>' + CrLf);
end;

procedure THtmlWriter.Table(Spacing, Padding, Border, AWidth: Integer);
begin
  Write(Format('<table cellspacing=%d cellpadding=%d border=%d width=%d%%>' +
    CrLf, [Spacing, Padding, Border, AWidth]));
  FOpenTags.Add(Pointer(htTable));
end;

procedure THtmlWriter.TableRow(Color: TColor = -1);
begin
  if Color = -1 then Write('<tr>' + CrLf)
  else Write(Format('<tr bgcolor=%s>' + CrLf, [GetColor(Color)]));
  FOpenTags.Add(Pointer(htRow));
end;

procedure THtmlWriter.TableCol(const Msg: string; Align: TAlignment = taLeftJustify);
var
  S: string;
begin
  if Align = taLeftJustify then
    FmtStr(S, '<td>%s</td>' + CrLf, [Msg])
  else
    FmtStr(S, '<td align=%s>%s</td>' + CrLf, [SAlignes[Align], Msg]);
  Write(S);
end;

procedure THtmlWriter.TableColW(const Msg: string; Width: Integer; Align: TAlignment = taLeftJustify);
var
  S: string;
begin
  if Align = taLeftJustify then
    FmtStr(S, '<td width=%d%%>%s</td>' + CrLf, [Width, Msg])
  else
    FmtStr(S, '<td width=%d%% align=%s>%s</td>' + CrLf, [Width, SAlignes[Align], Msg]);
  Write(S);
end;

procedure THtmlWriter.Indent;
begin
  Write('<ul>' + CrLf);
end;

procedure THtmlWriter.Unindent;
begin
  Write('</ul>' + CrLf);
end;

procedure THtmlWriter.ListItem;
begin
  Write('<li>');
end;

end.
