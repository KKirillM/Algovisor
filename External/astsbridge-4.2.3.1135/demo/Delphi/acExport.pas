unit AcExport;

interface

uses
  AcStruc, MTEApi, mteTypes;

procedure ExportDescToFile(Iface: TMicexIface; const FileName, Url: string);

implementation

uses
  HtmlOut, SysUtils, TypInfo, Graphics, Classes;

const
  TableColor  = $F0F0F0;
  HeaderColor = clLtGray;

  STableFlags: array [Boolean, Boolean] of string =
    (('(не обновляемая)', '(ошибка)'),
     ('(обновляемая)', '(обновляемая, очищать при обновлении)'));
  SNoFields: array [Boolean] of string = ('Входн', 'Выходн');
  SFieldTypes: array [TTEFieldType] of string =
    ('CHAR', 'INTEGER', '.00', 'PRICE', 'DATE', 'TIME', 'FLOAT');
  SBools: array [Boolean] of string = (NbSp, 'X');

function ToCell(const Msg: string): string;
begin
  if Trim(Msg) = '' then Result := NbSp else Result := Msg;
end;

procedure ExportDesc(Msg: TMicexMessage; Html: THtmlWriter);
begin
{  if Assigned(Msg) and (Msg.FDescr <> '') then
  begin
    Html.Write(Msg.FDescr);
    Html.Paragraph;
  end;}
end;

procedure SaveFields(Fields: TMicexFields; IsOutput: Boolean;
  Html: THtmlWriter; var FldDescRef: Integer);
var
  I: Integer;
  S, FldDesc: string;
begin
  FldDesc := '';
  with Html do
  begin
    if Fields.Count = 0 then
    begin
      Write(SNoFields[IsOutput] + 'ых полей нет');
      NewLine;
    end
    else
    begin
      Table(1, 1, 0, 90);
        TableRow(HeaderColor);
          TableCol(SNoFields[IsOutput] + 'ое поле');
          TableCol('Наименование');
          TableCol('Тип');
          TableCol('Размер', taRightJustify);
          TableCol('Ключ', taCenter);
          TableCol('Код бумаги', taCenter);
          TableCol('Перечислимый тип');
          if not IsOutput then TableCol('По умолчанию');
        EndTag;

        for I := 0 to Fields.Count - 1 do
          with TMicexField(Fields[I]) do
        begin
          TableRow(TableColor);
            S := FName;
(*            if FDescr <> '' then
            begin
              S := Format('<a href="#f%d">', [FldDescRef]) + S + '</a>';

{              FldDesc := FldDesc + Format('<p><a name="f%d">',
                [FldDescRef]) + '<i>' + FName + ': </i>' + FDescr + CrLf;}
              FldDesc := FldDesc + Format('<tr><td width=15%%><p><a name="f%d">',
                [FldDescRef]) + FName + '</td><td>' + FDescr + '</td></tr>' + CrLf;

              Inc(FldDescRef);
            end;*)
            TableCol(S);
            TableCol(ToCell(FCaption));
            TableCol(SFieldTypes[FType]);
            TableCol(IntToStr(FSize), taRightJustify);
            TableCol(SBools[FKey], taCenter);
            TableCol(SBools[FSecCode], taCenter);
            if Assigned(FEnumPtr) then
            begin
              Tag(htColumn);
              S := FEnumPtr.FName;
              HRef(S, '#e' + S);
              EndTag;
            end
            else TableCol(NbSp);

            if not IsOutput then TableCol(ToCell(FDefValue));
          EndTag;
        end;
      EndTag;

      if FldDesc <> '' then
      begin
        Paragraph;
        Table(1, 1, 0, 90);
          Write(FldDesc);
        EndTag;
      end;
    end;
  end;
end;

procedure ExportIface(Iface: TMicexIface; Html: THtmlWriter);
var
  I, FldDescRef: Integer;
  Upd, Clr: Boolean;
begin
  FldDescRef := 0;
  with Html do
  begin
    Separator;
{    Bookmark('i');
    TaggedText(Format('Интерфейс %s - %s', [Iface.FName, Iface.FCaption]), [htH2]);
    Separator;}
    Bookmark('it');
    TaggedText('Таблицы', [htH3]);
    Indent;

    for I := 0 to Iface.FTables.Count - 1 do
      with Iface.FTables[I] do
    begin
      Upd := FUpdateable;
      Clr := FClearOnUpdate;
      Bookmark('t' + IntToStr(I));
      TaggedText(Format('%s - %s %s',
        [FName, FCaption, STableFlags[Upd, Clr]]), [htH4]);

      Indent;
{      if FDescr <> '' then
      begin
        Html.Write(FDescr);
        Html.Paragraph;
      end
      else
      begin
        ExportDesc(FStatic.FMsg, Html);
        ExportDesc(FChange.FMsg, Html);
      end;}
      SaveFields(FInFields, False, Html, FldDescRef);
      if FInFields.Count > 0 then Paragraph;
      SaveFields(FOutFields, True, Html, FldDescRef);
      Unindent;
    end;

    Unindent;
    Separator;
    Bookmark('ir');
    TaggedText('Транзакции', [htH3]);
    Indent;
    for I := 0 to Iface.FTransactions.Count - 1 do
      with Iface.FTransactions[I] do
    begin
      Bookmark('r' + IntToStr(I));
      TaggedText(Format('%s - %s', [FName, FCaption]), [htH4]);
      Indent;
{      if FDescr <> '' then
      begin
        Html.Write(FDescr);
        Html.Paragraph;
      end
      else
        ExportDesc(FStatic.FMsg, Html);}
      SaveFields(FInFields, False, Html, FldDescRef);
      Unindent;
    end;
    Unindent;
  end;
end;

procedure ExportEnums(Iface: TMicexIface; Html: THtmlWriter);
var
  I, Idx: Integer;
  S: string;
begin
  with Html do
  begin
    Bookmark('e');
    TaggedText('Перечислимые типы', [htH3]);
    Indent;

    for I := 0 to Iface.FTypes.Count - 1 do
      with Iface.FTypes[I] do
    begin
      Bookmark('e' + FName);
      TaggedText(Format('%s - %s (длина: %d, тип: %s)',
        [FName, FCaption, FSize,
          GetEnumName(TypeInfo(TTEEnumKind), Integer(FKind))]), [htH4]);

      Indent;
{      if FDescr <> '' then
      begin
        Write(FDescr);
        Paragraph;
      end;}

      Table(1, 1, 0, 50);
        TableRow(HeaderColor);
          Write('<td width=20%>Константа</td>' + CrLf);

//          TableCol('Константа');
          TableCol('Описание');
        EndTag;

        for Idx := 0 to GetConstCount - 1 do
        begin
          TableRow(TableColor);
            S := GetConst(Idx);
            TableCol(ToCell(S));
            TableCol(ToCell(GetLongName(Idx)));
          EndTag;
        end;
      EndTag;
      Unindent;
    end;
    Unindent;
  end;
end;

procedure ExportContents(Iface: TMicexIface; Html: THtmlWriter);
var
  I, Y: Integer;
begin
  with Html, Iface do
  begin
    TaggedText('Содержание', [htH3]);
    HRef('Перечислимые типы', '#e');
    Writeln;
    Indent;
    for I := 0 to FTypes.Count - 1 do
      with FTypes[I] do
    begin
      HRef(FName + ' - ' + FCaption, '#e' + FName);
      NewLine;
    end;
    Unindent;

    HRef('Таблицы', '#it');
    Writeln;
    Indent;
    for Y := 0 to FTables.Count - 1 do
      with FTables[Y] do
    begin
      HRef(FName + ' - ' + FCaption, '#t' + IntToStr(Y));
      NewLine;
    end;

    Unindent;
    HRef('Транзакции', '#ir');
    Writeln;
    Indent;
    for Y := 0 to FTransactions.Count - 1 do
      with FTransactions[Y] do
    begin
      HRef(FName + ' - ' + FCaption, '#r' + IntToStr(Y));
      NewLine;
    end;

    Unindent;
  end;
end;

procedure ExportDescToFile(Iface: TMicexIface; const FileName, Url: string);
var
  Html: THtmlWriter;
begin
  Html := THtmlWriter.Create(FileName);
  with Html do
  try
    Head;
      TaggedText('Объекты интерфейса ' + Iface.FName, [htTitle]);
    EndTag;

    Body(clWhite);
//      Writeln('<font face="Arial Cyr, Arial, Verdana, Helvetica">');

      TaggedText(Format('Объекты интерфейса %s - %s',
        [Iface.FName, Iface.FCaption]), [htH2]);
      Separator;
      
      ExportContents(Iface, Html);

      ExportEnums(Iface, Html);
      ExportIface(Iface, Html);

      Paragraph;
      Separator;
      Writeln(Format('<font size=-1>Документ создан при помощи TEClient на основе "%s" %s</font>',
        [Url, DateTimeToStr(Now)]));

      Writeln('</font>');
    EndTag;
  finally
    Free;
  end;
end;

end.
