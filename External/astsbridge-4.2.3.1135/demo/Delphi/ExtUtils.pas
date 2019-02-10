{$IMPORTEDDATA ON}
unit ExtUtils;

interface

uses
  Classes;

function EAbs(X: Integer): Integer;
function EBSwap(HostNetLong: Integer): Integer;
function EBSwapWord(HostNetWord: Word): Word;
function EByteToInt(A: Byte): Integer;
function EDateToUnixDate(Date: TDateTime): Integer;
function ETimeToUnixTime(Time: TDateTime): Integer;
function EDivMod(Dividend, Divisor: Integer; var Reminder: Integer): Integer;
function EExtractWord(const S: string; Num: Integer): string;
function EChkFormatNumber(const S: string; Decimals: Byte): string;
function EFormatNumber(const S: string; Decimals: Byte): string;
function EFormatNumberPChar(Buffer: PChar; Len: Byte; Decimals: Byte): string;
function EGetShiftState: TShiftState;
function EGetTempFileName(const FilePrefix: string): string;
function EHexToInt(const S: string; DefValue: Integer): Integer;
function Einet_ntoa(Address: Integer): string;
function EIntIIF(Condition: Boolean; TrueExpr, FalseExpr: Integer): Integer;
function EIntToStr(Value: Integer): string;
function EIsEmpty(const S: string): Boolean;
function EIsTsmrNumber(const S: string): Boolean;
function EIsZero(const S: string): Boolean;
function EMin(A, B: Integer): Integer;
function EMax(A, B: Integer): Integer;
function EPadLeft(const S: string; Len: Integer): string;
function EPadRight(const S: string; Len: Integer): string;
function EPosInMemory(Symbol: Char; P: Pointer; var LengthOfP: Integer): Pointer;
function EPosNum(Symbol: Char; const S: string; Occurence: Integer): Integer;
function EStrComp(const Str1, Str2: string): Integer;
function EStrIIF(Condition: Boolean; const TrueExpr, FalseExpr: string): Pointer;
function EStrToInt(const S: string; DefValue: Integer): Integer;
function EStrToIntPChar(Buffer: PChar; Len: Byte; DefValue: Integer): Integer;
function EUnixDateToStr(Date: Integer): string;
function EUnixDateToDate(Date: Integer): TDateTime;
function EUnixStrToDate(const Date: string): Integer;
function EUnixStrToTime(const Time: string): Integer;
function EUnixTimeToDecimal(Time: Integer): Integer;
function EUnixTimeToTime(Time: Integer): TDateTime;
function EUnixTimeToStr(Time: Integer): string;
function EUnixStampToDateTime(UnixStamp: Integer): TDateTime;
function EVarToChar(const V: Variant): Char;
function EVarToInt(const V: Variant): Integer;
function EWordNum(Separator: Char; const S: string; Num: Integer): string;
function EDuplicate(C: Char; Len: Integer): string;
function ETrimSize(const S: string; Size: Integer): string;
function ESlashStr(const S: string): string;
function ESetString(Buffer: PChar; Len: Integer): string;
procedure ESetLength(var S: string; NewLength: Integer);
function ESign(Value: Integer): Integer;
function EWebTimeToDateTime(const S: string): TDateTime;

// Bit Mask handling routines
function EIsBitSet(Value, BitNo: Integer): Boolean;
function ESetBitCount(Value: Integer): Integer;
procedure EToggleBit(var Value: Integer; BitNo: Integer);
procedure ESetBit(var Value: Integer; BitNo: Integer);
procedure EClearBit(var Value: Integer; BitNo: Integer);
procedure EDeleteBit(var Value: Integer; BitNo: Integer);
procedure EInsertBit(var Value: Integer; BitNo: Integer);

function EDisplayInt(const S: string): string;
function EWinToDos(const S: string): string;

var
// Used in EUnixTimeToStr & EUnixStrToTime functions
  UnixBias: Integer = - 3 * 60 * 60; // UTC = local time + bias

implementation

uses
  SysUtils, Windows;

type
  TByteArray = array [0..100000] of Byte;
  PByteArray =  ^TByteArray;

var
  BaseUnixDate: TDateTime;
  BaseUnixDays: Integer;
  DecimalSeparator: Char;
  DateFmtStr: string;

{ Ёффективное изменение длины строки - если строка уникальна и выделенной
 под нее пам€ти достаточно дл€ нового размера, просто изменим длину
 This proc just makes a call to _LStrSetLength in SYSTEM unit }
procedure ESetLength(var S: string; NewLength: Integer);
begin
{ ->EAX   Pointer to S
    EDX   NewLength }
  asm
        MOV      ECX,[EAX]
        TEST     ECX,ECX
        JZ       @@Standard             // ≈сли пуста€ строка, надо создать
        CMP      DWORD PTR [ECX-8],1    // RefCount
        JNE      @@Standard             // ≈сли ссылок много, надо скопировать
        CMP      [ECX-4],EDX            // Length(S)
        JL       @@Standard             // New Size + 1 byte for trailing #0
        MOV      [ECX-4],EDX            // Set new length
        MOV      byte ptr [ECX+EDX],0   // Set trailing #0
        RET
    @@Standard:
  end;
  SetLength(S, NewLength);
end;

{ More then twice as fast as SysUtils.IntToStr function }
(*function EIntToStr(Value: Integer): string;
asm
{     ->EAX     Value
        EDX     Pointer to Result string }

        PUSH    ESI         // Instructed to save this staff
        PUSH    EDI
        PUSH    EBX

        MOV     EDI,EDX     // Save Result address
        MOV     ESI,10      // Decimal Base (=10)
        XOR     EBX,EBX
        MOV     ECX,EBX

        CMP     EAX,$7FFFFFFF
        JBE     @@first_digit
        INC     ECX         // Add minus "-" sign
        NEG     EAX

    @@first_digit:
        CMP     EAX,10
        JB      @@last_digit

    @@next_digit_pos:
        XOR     EDX,EDX     // Extract decimal digits in reverse order
        DIV     ESI
        INC     EBX
        PUSH    EDX         // Save extracted digit to stack
        CMP     EAX,10
        JAE     @@next_digit_pos

    @@last_digit:
        PUSH    EAX
        INC     EBX
        DEC     ECX
        JNZ     @@set_length
        PUSH    '-' - '0'   // Store minus sign
        INC     EBX

    @@set_length:
// Now set Result length to number of extracted digits
        MOV     EAX,EDI     // EAX = first param = String
        MOV     EDX,EBX     // EDX = second param = NewStringLength
        CALL    ESetLength  // SetLength(EAX, EDX)

// Fill in new string with extracted digits
        MOV     ESI,[EDI]
    @@clear_stack:
        POP     EDX         // Get extracted digits from stack
        ADD     DL,'0'
        MOV     [ESI],DL
        INC     ESI
        DEC     EBX
        JNZ     @@clear_stack

        POP     EBX
        POP     EDI
        POP     ESI
end;
*)

{ Eight times as fast as SysUtils.IntToStr function }
function EIntToStr(Value: Integer): string;
asm
{     ->EAX     Value
        EDX     Pointer to Result string }

        PUSH    ESI         // Instructed to save this staff
        PUSH    EDI
        PUSH    EBX

        MOV     EDI,EDX     // Save Result address
        XOR     EBX,EBX
        MOV     ECX,EBX

        CMP     EAX,$7FFFFFFF
        JBE     @@first_digit
        INC     ECX         // Add minus "-" sign
        NEG     EAX

    @@first_digit:
        CMP     EAX,10
        JB      @@last_digit

    @@next_digit_pos:
        MOV     ESI,EAX
        MOV     EDX,$CCCCCCCD     // Extract decimal digits in reverse order
        MUL     EDX
        SHR     EDX,3             // EDX := EAX div 10
        MOV     EAX,EDX
        LEA     EDX,[EDX+4*EDX]   // EDX := EDX * 5
        ADD     EDX,EDX           // EDX := EDX * 2 -> (OldEDX * 10)
        SUB     ESI,EDX           // ESI = remainder
        PUSH    ESI               // Save extracted digit to stack
        INC     EBX
        CMP     EAX,10
        JAE     @@next_digit_pos


    @@last_digit:
        PUSH    EAX
        INC     EBX
        DEC     ECX
        JNZ     @@set_length
        PUSH    '-' - '0'   // Store minus sign
        INC     EBX

    @@set_length:
// Now set Result length to number of extracted digits
        MOV     EAX,EDI     // EAX = first param = String
        MOV     EDX,EBX     // EDX = second param = NewStringLength
        CALL    ESetLength  // SetLength(EAX, EDX)

// Fill in new string with extracted digits
        MOV     ESI,[EDI]
    @@clear_stack:
        POP     EDX         // Get extracted digits from stack
        ADD     DL,'0'
        MOV     [ESI],DL
        INC     ESI
        DEC     EBX
        JNZ     @@clear_stack

        POP     EBX
        POP     EDI
        POP     ESI
end;

{ Returns absolute value of X }
function EAbs(X: Integer): Integer;
asm
{     ->EAX     X
      <-EAX     Abs(X) }

        MOV     ECX,EAX
        SAR     ECX,31
        XOR     EAX,ECX     // 1Тs complement if x<0, else donТt modify
        SUB     EAX,ECX     // 2Тs complement if x<0, else donТt modify
end;

{ Return mimimum of two signed integers }
function EMax(A, B: Integer): Integer;
asm
{     ->EAX     A
        EDX     B
      <-EAX     Min(A, B) }

        CMP     EAX,EDX
        JGE     @@Exit
        MOV     EAX,EDX
  @@Exit:
end;

{ Return maximum of two signed integers }
function EMin(A, B: Integer): Integer;
asm
{     ->EAX     A
        EDX     B
      <-EAX     Max(A, B) }

        CMP     EAX,EDX
        JLE     @@Exit
        MOV     EAX,EDX
  @@Exit:
end;

{ Works the same way as ntohl/htonl }
function EBSwap(HostNetLong: Integer): Integer;
asm
{     ->EAX(0):EAX(1):EAX(2):EAX(3)     HostNetLong
      <-EAX(3):EAX(2):EAX(1):EAX(0) }

        BSWAP   EAX
end;

function EBSwapWord(HostNetWord: Word): Word;
asm
{     ->EAX(0):EAX(1)
      <-EAX(1):EAX(0) }

        XCHG    AH,AL
end;

{ Works the same way as StrComp, but compares strings (not PChars) }
function EStrComp(const Str1, Str2: string): Integer;
asm
{     ->EAX     pointer to Str1
        EDX     pointer to Str2
      <-EAX     < 0, if Str1 < Str2
                = 0, if Str1 = Str2
                > 0, if Str1 > Str2 }
                     
        PUSH    ESI         // Instructed to save this staff
        PUSH    EDI

        TEST    EAX,EAX
        JNZ     @@CheckStr2
        TEST    EDX,EDX
        JZ      @@Exit
        DEC     EAX
        JMP     @@Exit

  @@CheckStr2:
        TEST    EDX,EDX
        JNZ     @@DoCompare
        MOV     EAX,1
        JMP     @@Exit

  @@DoCompare:
        MOV     ESI,EAX
        MOV     EDI,EDX
        MOV     ECX,[ESI-4] // Get Str1 length
        INC     ECX
        REPE    CMPSB
        MOVZX   EAX,BYTE PTR [ESI-1]
        MOVZX   EDX,BYTE PTR [EDI-1]
        SUB     EAX,EDX

  @@Exit:
        POP     EDI
        POP     ESI
end;

{ Returns True, if string S represents ZERO in any form
  (with or w/o sign, with or w/o decimal point or comma) }
function EIsZero(const S: string): Boolean;
asm
{     ->EAX     Pointer to string S
      <-EAX     Result }

        TEST    EAX,EAX
        JZ      @@Zero
//        MOV     EDX,EAX      // Pointer to S data
//        MOV     EAX,[EAX-4]  // Obtain Length(S)

  @@SkipBlanks:
        MOV     DL,[EAX]
        INC     EAX
        CMP     DL,' '
        JE      @@SkipBlanks

        CMP     DL,'+'
        JE      @@GetDigit
        CMP     DL,'-'
        JNE     @@Digits

  @@GetDigit:
        MOV     DL,[EAX]
        INC     EAX
        TEST    DL,DL
        JZ      @@NotZero

  @@Digits:
        CMP     DL,'0'
        JE      @@OkChar
        CMP     DL,32   // Space
        JE      @@OkChar
        CMP     DL,'.'
        JE      @@OkChar
        CMP     DL,','
        JNE     @@NotZero
  @@OkChar:
        MOV     DL,[EAX]
        INC     EAX
        TEST    DL,DL
        JNZ     @@Digits

  @@Zero:
        MOV     EAX,True
        RET

  @@NotZero:
        XOR     EAX,EAX
end;

// Returns True if S is valid TSMR number (e.g. S = '-000001354')
function EIsTsmrNumber(const S: string): Boolean;
asm
{     ->EAX     Pointer to string S
      <-EAX     Result }

        TEST    EAX,EAX
        JZ      @@NotNumber
        MOV     EDX,EAX      // Pointer to S data
        MOV     EAX,[EAX-4]  // Obtain Length(S)

        MOV     CL,[EDX]
        CMP     CL,'-'
        JE      @@Next
        CMP     CL,'+'
        JNE     @@NextChar2

  @@Next:
        INC     EDX
        DEC     EAX
        JZ      @@NotNumber

  @@NextChar:
        MOV     CL,[EDX]
  @@NextChar2:      
        INC     EDX
        SUB     CL,$30
        CMP     CL,9
        JA      @@NotNumber
        DEC     EAX
        JNZ     @@NextChar

        MOV     EAX,True
        RET

  @@NotNumber:
        XOR     EAX,EAX
end;

function EIsEmpty(const S: string): Boolean;
asm
{     ->EAX     Pointer to string S
      <-EAX     Result }

        OR      EAX,EAX
        JZ      @@Empty
        MOV     EDX,EAX      // Pointer to S data
        MOV     EAX,[EAX-4]  // Obtain Length(S)

  @@NextChar:
        CMP     BYTE PTR [EDX],' '
        JNE     @@NotEmpty
        INC     EDX
        DEC     EAX
        JNZ     @@NextChar

  @@Empty:
        INC     EAX
        RET

  @@NotEmpty:
        XOR     EAX,EAX
end;

procedure _StrToInt;
asm
{     ->EAX     Pointer to Buffer
        EDX     DefValue
        EBP     Pointer after last char of Buffer
      <-EAX     Decimal value of string }

        PUSH    ESI
        PUSH    EDI
        PUSH    EBX

        MOV     EBX,EDX       // Save default value
        MOV     ESI,EAX
        MOV     EAX,0
        MOV     EDX,EAX
        MOV     EDI,07FFFFFFFH / 10     // overflow limit
        MOV     CL,AL         // Sign

  @@SkipBlanks:
        CMP     ESI,EBP
        JAE     @@Exception
        MOV     DL,[ESI]
        INC     ESI
        CMP     DL,' '
        JE      @@SkipBlanks

        CMP     DL,'+'
        JE      @@GetDigit
        CMP     DL,'-'
        JNE     @@Digits
        INC     CL             // Negative number

  @@GetDigit:
        CMP     ESI,EBP
        JAE     @@Exception
        MOV     DL,[ESI]
        INC     ESI

  @@Digits:
        SUB     DL,$30
        CMP     DL,9
        JA      @@Exception
        CMP     EAX,EDI        // value > overflow limit ?
        JA      @@Exception
        LEA     EAX,[EAX+4*EAX]// EAX := EAX * 5
        ADD     EAX,EAX        // EAX := EAX * 2 -> (OldEAX * 10)
        ADD     EAX,EDX
        CMP     ESI,EBP
        JAE     @@End
        MOV     DL,[ESI]
        INC     ESI
        JMP     @@Digits

  @@End:
        DEC     CL
        JZ      @@Negate
        TEST    EAX,EAX
        JL      @@Exception

  @@Exit:
        POP     EBX
        POP     EDI
        POP     ESI
        RET

  @@Negate:
        NEG     EAX
        JLE     @@Exit
        JS      @@Exit

  @@Exception:
        MOV     EAX,EBX
        JMP     @@Exit
end;

{ Returns decimal value represented by string S
  or DefValue if S is not valid integer number }
function EStrToInt(const S: string; DefValue: Integer): Integer;
asm
{     ->EAX     Pointer to string
        EDX     DefValue
      <-EAX     Decimal value of string }

        TEST    EAX,EAX
        JZ      @@Exception
        PUSH    EBP
        MOV     EBP,EAX
        ADD     EBP,[EAX-4]   // Pointer to last char
        CALL    _StrToInt
        POP     EBP
        RET

  @@Exception:
        MOV     EAX,EDX
end;

function EStrToIntPChar(Buffer: PChar; Len: Byte; DefValue: Integer): Integer;
asm
{     ->EAX     Pointer to Buffer
        EDX     Length of buffer
        ECX     DefValue
      <-EAX     Decimal value of string }

        PUSH    EBP
        MOV     EBP,EAX
        MOVZX   EDX,DL
        ADD     EBP,EDX
        MOV     EDX,ECX
        CALL    _StrToInt
        POP     EBP
end;

function EByteToInt(A: Byte): Integer;
asm
        MOVSX   EAX,AL
end;

{ Assembler version is more complex, but even less effective }
function EPadRight(const S: string; Len: Integer): string;
begin
  ESetLength(Result, Len);
  FillChar(Pointer(Result)^, Len, ' ');
  if S <> '' then Move(Pointer(S)^, Pointer(Result)^, EMin(Len, Length(S)));
end;

function EPadLeft(const S: string; Len: Integer): string;
var
  CopyLen: Integer;
begin
  ESetLength(Result, Len);
  FillChar(Pointer(Result)^, Len, ' ');
  CopyLen := EMin(Len, Length(S));
  Move(Pointer(S)^, PByteArray(Result)^[Len - CopyLen], CopyLen);
end;

{ if Condition is True, returns TrueExpr else FalseExpr }
function EIntIIF(Condition: Boolean; TrueExpr, FalseExpr: Integer): Integer;
asm
{     ->AL      Condition
        EDX     TrueExpt
        ECX     FalseExpr
      <-EAX     Result }

        TEST    AL,AL
        JZ      @@False
        MOV     EAX,EDX
        RET
  @@False:
        MOV     EAX,ECX
end;

{ if Condition is True, returns TrueExpr else FalseExpr }
function EStrIIF(Condition: Boolean; const TrueExpr, FalseExpr: string): Pointer;
asm
{     ->AL      Condition
        EDX     TrueExpt
        ECX     FalseExpr
      <-EAX     Result }

        TEST    AL,AL
        JZ      @@False
        MOV     EAX,EDX
        RET
  @@False:
        MOV     EAX,ECX
end;

{ Returns hexadecimal value represented by hexadecimal string  S
  or DefValue if S is not valid integer number }
function EHexToInt(const S: string; DefValue: Integer): Integer;
asm
{     ->EAX     Pointer to string
        EDX     DefValue
      <-EAX     Hexadecimal value of string }

        PUSH    ESI
        PUSH    EDI
        PUSH    EBX

        MOV     EBX,EDX       // Save default value
        TEST    EAX,EAX
        JZ      @@Exception
        MOV     ESI,EAX
        MOV     EAX,0
        MOV     EDX,EAX
        MOV     EDI,0FFFFFFFH  // overflow limit
        MOV     CL,AL          // Sign

  @@SkipBlanks:
        MOV     AL,[ESI]
        INC     ESI
        CMP     AL,' '
        JE      @@SkipBlanks

        CMP     AL,'+'
        JE      @@GetDigit
        CMP     AL,'-'
        JNE     @@Digits
        INC     CL             // Negative number

  @@GetDigit:
        MOV     AL,[ESI]
        INC     ESI
        TEST    AL,AL
        JZ      @@Exception

  @@Digits:
        SUB     AL,'0'
        CMP     AL,9
        JBE     @@ValidDigit
        AND     AL,0dfh
        SUB     AL,'A'-'0'
        CMP     AL,5
        JA      @@Exception
        ADD     AL,10
  @@ValidDigit:
        CMP     EDX,EDI        // value > overflow limit ?
        JA      @@Exception
        SHL     EDX,4
        ADD     EDX,EAX
        MOV     AL,[ESI]
        INC     ESI
        TEST    AL,AL
        JNZ     @@Digits

  @@End:
        MOV     EAX,EDX
        DEC     CL
        JNZ     @@Exit
        NEG     EAX
        JMP     @@Exit

  @@Exception:
        MOV     EAX,EBX

  @@Exit:
        POP     EBX
        POP     EDI
        POP     ESI
end;

function EVarToChar(const V: Variant): Char;
begin
  if (TVarData(V).VType <> varNull) and (V <> '') then
    Result := string(V)[1] else Result := #0;
end;

function EVarToInt(const V: Variant): Integer;
begin
  if TVarData(V).VType <> varNull then Result := V else Result := 0;
end;

function Einet_ntoa(Address: Integer): string;
var
  B: TByteArray absolute Address;
begin
  Result := Format('%d.%d.%d.%d', [B[0], B[1], B[2], B[3]]);
end;

function EDivMod(Dividend, Divisor: Integer; var Reminder: Integer): Integer;
asm
{     ->EAX     Dividend
        EDX     Divisor
        ECX     Pointer to Reminder
      <-EAX     Whole part of division }

        PUSH    EBX
        MOV     EBX,EDX
        CDQ
        IDIV    EBX
        MOV     [ECX],EDX
        POP     EBX
end;

function EUnixTimeToStr(Time: Integer): string;
var
  Hours, Min: Integer;
begin
  Hours := EDivMod((Time - UnixBias) mod (60 * 60 * 24), 3600, Time);
  Min := EDivMod(Time, 60, Time);
  Result := Format('%2.2d:%2.2d:%2.2d', [Hours, Min, Time]);
end;

function EUnixTimeToDecimal(Time: Integer): Integer;
var
  Hours, Min: Integer;
begin
  Hours := EDivMod((Time - UnixBias) mod (60 * 60 * 24), 3600, Time);
  Min := EDivMod(Time, 60, Time);
  Result := Hours * 10000 + Min * 100 + Time;
end;

function EUnixStrToTime(const Time: string): Integer;
begin
  Result := (EStrToInt(EWordNum(':', Time, 1), 0) * 60 +
    EStrToInt(EWordNum(':', Time, 2), 0)) * 60 +
      EStrToInt(EWordNum(':', Time, 3), 0) + UnixBias;
end;

{ Convert string representation of number:
  - strips leading zeros;
  - inserts decimal point at specified position.
  Ex.: ConvertNumber('-000000324', 2) = '-3.24'
}
(*
function EFormatNumber(const S: string; Decimals: Integer): string;
var
  Len, I, NewLen, Minus: Integer;
begin
  Len := Length(S);
  I := 1;
  Minus := 0;
  if (Len > 0) then
    if S[1] = '-' then
      Minus := 1
    else
      if S[1] = '+' then Inc(I);
  Inc(I, Minus);

  while I <= Len do if S[I] = '0' then Inc(I) else Break; // Strip leading zeros

  NewLen := Len - I + 1;
  if Decimals <= 0 then           // don't place decimal point
  begin
    if NewLen = 0 then Inc(NewLen);
    SetLength(Result, NewLen + Minus);
    if I > Len then Result[Minus + 1] := '0'
      else Move(S[I], Result[Minus + 1], NewLen);
  end
  else
  begin                           // place decimal point at correct place
    if NewLen <= Decimals then NewLen := Decimals + 2 + Minus
      else NewLen := NewLen + 1 + Minus;

    SetLength(Result, NewLen);
    I := NewLen;
    while I > Minus do
    begin
      if Decimals = 0 then Result[I] := '.' else
      begin     // LPad with zeros if not enough digits
        if Len > Minus then Result[I] := S[Len] else Result[I] := '0';
        Dec(Len);
      end;
      Dec(Decimals);
      Dec(I);
    end;
  end;
  if Minus > 0 then Result[1] := '-';
end;
*)
function EChkFormatNumber(const S: string; Decimals: Byte): string;
{const
  DecimalPoint = ',';}
asm
{     ->EAX     Pointer to string S
        EDX     Decimals
        ECX     Pointer to Result string }

        PUSH    ESI
        PUSH    EDI
        PUSH    EBX

        TEST    EAX,EAX
        JZ      @@Exception
        MOV     ESI,EAX
        MOV     EDI,ECX
        MOV     DH,0           // Sign = '+'
        MOV     EBX,ESP        // Stack counter

  @@SkipBlanks:
        MOV     AL,[ESI]
        INC     ESI
        CMP     AL,' '
        JE      @@SkipBlanks

        CMP     AL,'+'
        JE      @@GetDigit
        CMP     AL,'-'
        JNE     @@SkipZeroes
        INC     DH             // Negative number

  @@GetDigit:
        MOV     AL,[ESI]
        INC     ESI

  @@SkipZeroes:
        CMP     AL,'0'
        JNE     @@StartParse

  @@GetZero:
        MOV     AL,[ESI]
        INC     ESI
        CMP     AL,'0'
        JE      @@GetZero

  @@CheckEmpty:
        TEST    AL,AL          // Empty string or '00000000'
        JNZ     @@StartParse
        MOV     ECX,ESI        // Save first not '0' char
        DEC     ESI
        MOV     DH,0           // '-0' turns into '0'
        JMP     @@PushDigit

  @@StartParse:
        MOV     ECX,ESI        // Save first not '0' char

  @@Digits:
        SUB     AL,'0'
        CMP     AL,9
        JA      @@Exception
        MOV     AL,[ESI]
        INC     ESI
        TEST    AL,AL
        JNZ     @@Digits

        DEC     ESI

  @@PushDigit:                 // Push digits from right to left <--
        DEC     ESI
        MOV     AL,[ESI]
        PUSH    EAX
        DEC     DL             // Check Decimals
        JNZ     @@DontPlace
        PUSH    dword ptr [DecimalSeparator]

  @@DontPlace:
        CMP     ESI,ECX
        JAE     @@PushDigit

// Check, if we have any digits before '.' (e.g. '.23')
        TEST    DL,DL
        JL      @@CheckSign

  @@LeadZeroes:
        PUSH    '0'
        DEC     DL
        JG      @@LeadZeroes
        JL      @@CheckSign

  @@PlacePoint:
        PUSH    [dword ptr DecimalSeparator]
        PUSH    '0'

  @@CheckSign:
        TEST    DH,DH          // Check sign
        JZ      @@SetLength
        PUSH    '-'

    @@SetLength:
        SUB     EBX,ESP
        SHR     EBX,2
// Now set Result length to number of extracted digits
        MOV     EAX,EDI     // EAX = first param = String
        MOV     EDX,EBX     // EDX = second param = NewStringLength
        CALL    ESetLength  // SetLength(EAX, EDX)

// Fill in new string with extracted digits
        MOV     ESI,[EDI]

    @@ClearStack:
        POP     EAX         // Get extracted digits from stack
        MOV     [ESI],AL
        INC     ESI
        DEC     EBX
        JNZ     @@ClearStack

  @@Exception:

  @@Exit:
        POP     EBX
        POP     EDI
        POP     ESI
end;

procedure _FormatNumber;
asm
{     ->EAX     Pointer to Buffer
        EBX     Pointer to last char of Buffer (#0 for AnsiString)
        ECX     Pointer to Result string
        EDX     Decimals }

        PUSH    ESI
        PUSH    EDI
        MOV     ESI,EAX
        MOV     EDI,ECX
        MOV     DH,0           // Sign = '+'

  @@SkipBlanks:
        MOV     AL,[ESI]
        INC     ESI
        CMP     AL,' '
        JE      @@SkipBlanks

        CMP     AL,'+'
        JE      @@GetDigit
        CMP     AL,'-'
        JNE     @@SkipZeroes
        INC     DH             // Negative number

  @@GetDigit:
        MOV     AL,[ESI]
        INC     ESI

  @@SkipZeroes:
        CMP     ESI,EBX
        JA      @@Finish
        
        CMP     AL,'0'
        JE      @@GetDigit
        CMP     AL,' '
        JE      @@GetDigit

  @@Finish:
{       CMP     AL,0
        SETNZ   CL}
        CMP     ESI,EBX
        SETBE   CL

        MOVSX   ECX,CL
        MOV     EAX,EBX
        ADD     EAX,ECX
        SUB     EAX,ESI
        AND     EAX,$FF        // EAX := Length of non '0' remainder of S
        CMP     AL,DL          // Length <= Decimals ?
        JG      @@LengthOk     // (assume Length(S) < 256)!!!
        MOV     AL,DL
        INC     AL             // space for leading zero
        CMP     DL,0
        JZ      @@AddSignPlace
        INC     AL
        JMP     @@AddSignPlace

  @@LengthOk:
        TEST    DL,DL
        SETNZ   CL             // Place for decimal point
        ADD     AL,CL

  @@AddSignPlace:
        ADD     AL,DH          // Space for possible '-' sign

// Now set Result length to number of extracted digits
        PUSH    EAX
        PUSH    EDX
        MOV     EDX,EAX     // EDX = second param = NewStringLength
        MOV     EAX,EDI     // EAX = first param = String
        CALL    ESetLength  // SetLength(EAX, EDX)
        POP     EDX
        POP     EAX

// Fill in new string with extracted digits
        MOV     EDI,[EDI]
        ADD     EDI,EAX

        CMP     EBX,ESI
        JB      @@ZeroStr

  @@PushDigit:                 // Push digits from right to left <--
        DEC     EBX
        MOV     AL,[EBX]
        DEC     EDI
        MOV     [EDI],AL
        DEC     DL             // Check Decimals
        JNZ     @@DontPlace
        DEC     EDI
        MOV     AL,[DecimalSeparator]
        MOV     BYTE PTR [EDI],AL

  @@DontPlace:
        CMP     EBX,ESI
        JAE     @@PushDigit

// Check, if we have any digits before '.' (e.g. '.23')
  @@ZeroStr:
        TEST    DL,DL
        JL      @@CheckSign

  @@LeadZeroes:
        DEC     EDI
        MOV     BYTE PTR [EDI],'0'
        DEC     DL
        JG      @@LeadZeroes
        JL      @@CheckSign

//  @@PlacePoint:
        DEC     EDI
        MOV     AL,[DecimalSeparator]
        MOV     BYTE PTR [EDI],AL
        DEC     EDI
        MOV     BYTE PTR [EDI],'0'

  @@CheckSign:
        TEST    DH,DH          // Check sign
        JZ      @@Exit
        DEC     EDI
        MOV     BYTE PTR [EDI],'-'

  @@Exit:
        POP     EDI
        POP     ESI
end;

function EFormatNumber(const S: string; Decimals: Byte): string;
const
  Null: Char = #0;
asm
{     ->EAX     Pointer to string S
        EDX     Decimals
        ECX     Pointer to Result string }

        PUSH    EBX
        TEST    EAX,EAX
        JZ      @@Exception

        MOV     EBX,EAX
        ADD     EBX,[EAX-4]    // EBX = Last char of S (#0)
        CALL    _FormatNumber
        POP     EBX
        RET

  @@Exception:
        MOV     EAX,OFFSET Null
        MOV     EBX,EAX
        CALL    _FormatNumber
        POP     EBX
end;

function EFormatNumberPChar(Buffer: PChar; Len: Byte; Decimals: Byte): string;
asm
{     ->EAX     Pointer to Buffer
        EDX     Buffer length
        ECX     Decimals
        [ESP+4] Pointer to Result string }

        PUSH    EBX
        MOV     EBX,EAX
        MOVZX   EDX,DL
        ADD     EBX,EDX
        MOV     EDX,ECX
        MOV     ECX,[ESP+4+8]     // EBX, EBP already on stack
        CALL    _FormatNumber
        POP     EBX
end;

function EPosNum(Symbol: Char; const S: string; Occurence: Integer): Integer;
asm
{     ->AL      Symbol
        EDX     Pointer to S
        ECX     Occurence
      <-EAX     Occurence'th occurence of Symbol within S }

        PUSH    EBX
        PUSH    EDI
        TEST    EDX,EDX
        JE      @@failed
        TEST    ECX,ECX
        JLE     @@zero

        MOV     EDI,EDX
        MOV     EBX,ECX
        MOV     ECX,[EDX-4]            // Length(S)

  @@again:
        REPNE   SCASB
        JNE     @@failed
        DEC     EBX
        JNE     @@again

        MOV     EAX,EDI
        SUB     EAX,EDX
        JMP     @@exit

  @@failed:
        MOV     EAX,$7FFFFFFF
        JMP     @@exit

  @@zero:
        MOV     EAX,0

  @@exit:
        POP     EDI
        POP     EBX
end;

function EWordNum(Separator: Char; const S: string; Num: Integer): string;
var
  Start: Integer;
begin
  Start := EPosNum(Separator, S, Num - 1) + 1;
  Result := Copy(S, Start, EPosNum(Separator, S, Num) - Start);
end;

function EPosInMemory(Symbol: Char; P: Pointer; var LengthOfP: Integer): Pointer;
asm
{     ->AL      Symbol
        EDX     Value of pointer P
        ECX     LengthOfP
      <-EAX     Address of first found Symbol within P^, or Symbol + LengthOfP if failed }

        PUSH    EDI
        TEST    EDX,EDX
        MOV     EDI,EDX
        JZ      @@exit
        MOV     EDX,ECX
        MOV     ECX,[EDX]
        TEST    ECX,ECX
        JLE     @@failed

        REPNE   SCASB
        JNE     @@failed
        DEC     EDI
        INC     ECX
  @@failed:
        MOV     EAX,EDI
        MOV     [EDX],ECX

  @@exit:
        POP     EDI
end;

function EUnixDateToStr(Date: Integer): string;
var
  Year, Month, Day: Word;
begin
  DecodeDate(BaseUnixDate + Date div (60 * 60 * 24), Year, Month, Day);
  Result := Format(DateFmtStr, [Day, Month, Year]);
end;

function EUnixDateToDate(Date: Integer): TDateTime;
begin
  Result := BaseUnixDate + Date div (60 * 60 * 24)
end;

function EUnixTimeToTime(Time: Integer): TDateTime;
var
  Hours, Min: Integer;
begin
  Hours := EDivMod((Time - UnixBias) mod (60 * 60 * 24), 3600, Time);
  Min := EDivMod(Time, 60, Time);
  Result := EncodeTime(Hours, Min, Time, 0);
end;

function EUnixStampToDateTime(UnixStamp: Integer): TDateTime;
var
  TimeStamp: TTimeStamp;
begin
  TimeStamp.Date := EDivMod(UnixStamp, 60 * 60 * 24, TimeStamp.Time) + BaseUnixDays;
  TimeStamp.Time := (TimeStamp.Time - UnixBias) * 1000;
  Result := TimeStampToDateTime(TimeStamp);
end;

function EDateToUnixDate(Date: TDateTime): Integer;
begin
  Result := Trunc(Date - BaseUnixDate) * 60 * 60 *24;
end;

function EUnixStrToDate(const Date: string): Integer;
begin
  Result := EDateToUnixDate(StrToDate(Date));
end;

function ETimeToUnixTime(Time: TDateTime): Integer;
var
  T: TTimeStamp;
begin
  T := DateTimeToTimeStamp(Time);
  Result := T.Time div 1000 + UnixBias;
end;

procedure InitUnixBias;
var
  TZ: TTimeZoneInformation;
begin
  case GetTimeZoneInformation(TZ) of
    TIME_ZONE_ID_UNKNOWN: UnixBias := TZ.Bias;
    TIME_ZONE_ID_STANDARD: UnixBias := TZ.Bias + TZ.StandardBias;
    TIME_ZONE_ID_DAYLIGHT: UnixBias := TZ.Bias + TZ.DaylightBias;
  end;
  UnixBias := UnixBias * 60;
end;

function EExtractWord(const S: string; Num: Integer): string;
var
  Count, Index, Start, Finish: Integer;
begin
  Result := '';
  if (S = '') or (Num <= 0) then Exit;
  Index := 1;
  Start := 1;
  Finish := 1;
  for Count := 1 to Num do
  begin
    while S[Index] in [' ', #9] do Inc(Index);  // Find word start
    Start := Index;
    while not (S[Index] in [' ', #9, #0]) do Inc(Index);  // Find word end
    Finish := Index;
  end;
  Result := Copy(S, Start, Finish - Start);
end;

function EDuplicate(C: Char; Len: Integer): string;
begin
  ESetLength(Result, Len);
  FillChar(Pointer(Result)^, Len, C);
end;

function EGetTempFileName(const FilePrefix: string): string;
var
  Len: Integer;
  NameBuf: array [0..MAX_PATH] of Char;
begin
  ESetLength(Result, MAX_PATH);
  Len := GetTempPath(MAX_PATH, Pointer(Result));
  if Len = 0 then Result[1] := #0;
  if GetTempFileName(Pointer(Result), Pointer(FilePrefix), 0, @NameBuf) <> 0 then
    Result := NameBuf else Result := FilePrefix;
end;

function EGetShiftState: TShiftState;
begin
  Result := [];
  if GetKeyState(VK_SHIFT) < 0 then Include(Result, ssShift);
  if GetKeyState(VK_CONTROL) < 0 then Include(Result, ssCtrl);
  if GetKeyState(VK_MENU) < 0 then Include(Result, ssAlt);
end;

function ETrimSize(const S: string; Size: Integer): string;
begin
  if Length(S) > Size then SetString(Result, PChar(S), Size)
    else Result := S;
end;

function ESlashStr(const S: string): string;
begin
  if (S <> '') and (S[Length(S)] <> '\') then
    Result := S + '\'
  else
    Result := S;
end;

function ESetString(Buffer: PChar; Len: Integer): string;
begin
  SetString(Result, Buffer, Len);
end;

function ESetBitCount(Value: Integer): Integer;
asm
{    -> EAX     Value
     <- EAX     Result }

        MOV     CL,32
        XOR     EDX,EDX

  @@Test:
        SHR     EAX,1
        ADC     DL,0
        DEC     CL
        JNZ     @@Test
        MOV     EAX,EDX
end;

function EIsBitSet(Value, BitNo: Integer): Boolean;
asm
{    -> EAX     Value
        EDX     BitNo
     <- EAX     Result }

        BT      EAX,EDX
        SETC    AL
end;

procedure EToggleBit(var Value: Integer; BitNo: Integer);
asm
{    -> EAX     @Value
        EDX     BitNo }

        BTC     [EAX],EDX
end;

procedure ESetBit(var Value: Integer; BitNo: Integer);
asm
{    -> EAX     @Value
        EDX     BitNo }

        BTS     [EAX],EDX
end;

procedure EClearBit(var Value: Integer; BitNo: Integer);
asm
{    -> EAX     @Value
        EDX     BitNo }

        BTR     [EAX],EDX
end;

procedure EDeleteBit(var Value: Integer; BitNo: Integer);
asm
{    -> EAX     @Value
        EDX     BitNo }

        PUSH    EBX
        MOV     EBX,[EAX]
        MOV     CL,DL
        SHRD    EDX,EBX,CL
        SHR     EBX,CL
        SHR     EBX,1
        SHLD    EBX,EDX,CL
        MOV     [EAX],EBX
        POP     EBX
end;

procedure EInsertBit(var Value: Integer; BitNo: Integer);
asm
{    -> EAX     @Value
        EDX     BitNo }

        PUSH    EBX
        MOV     EBX,[EAX]
        MOV     CL,DL
        SHRD    EDX,EBX,CL
        SHR     EBX,CL
        SHL     EBX,1          // Clear bit being inserted
        SHLD    EBX,EDX,CL
        MOV     [EAX],EBX
        POP     EBX
end;

function ESign(Value: Integer): Integer;
asm
{    -> EAX     @Value
     <- EAX     -1, if Value < 0
                0, if Value = 0
                1, if Value > 0 }

        MOV     EDX,EAX
        SAR     EDX,31
        NEG     EAX
        SAR     EAX,31
        NEG     EAX
        ADD     EAX,EDX
end;

function EDisplayInt(const S: string): string;
var
  I, Digits: Integer;
begin
  Result := S;
  I := 1;
  while (I <= Length(Result)) and not (Result[I] in [',', '.']) do Inc(I);
  Digits := 0;
  Dec(I);

  while I > 1 do
  begin
    Inc(Digits);
    if Digits = 3 then
    begin
      if Result[I - 1] in ['0'..'9'] then
        Insert(ThousandSeparator, Result, I);
      Digits := 0;
    end;
    Dec(I);
  end;
end;

function EWinToDos(const S: string): string;
begin
  SetLength(Result, Length(S));
  CharToOem(Pointer(S), Pointer(Result));
end;

function MonthNum(const S: string): Integer;
const
  SMonthes: array [1..12] of string =
    ('Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec');
var
  I: Integer;
begin
  for I := Low(SMonthes) to High(SMonthes) do
    if SMonthes[I] = S then
    begin
      Result := I;
      Exit;
    end;
  Result := 0;
end;

// converts date from web-server into TDateTime
// e.g.: Tue, 29 Feb 2002 11:06:33 GMT
function EWebTimeToDateTime(const S: string): TDateTime;
var
  Year, Month, Day: Word;
  TimeStamp: TTimeStamp;
begin
  Day := EStrToInt(EWordNum(' ', S, 2), 0);
  Month := MonthNum(EWordNum(' ', S, 3));
  Year := EStrToInt(EWordNum(' ', S, 4), 0);
  Result := EncodeDate(Year, Month, Day) + StrToTime(EWordNum(' ', S, 5));
  // apply GMT time shift GetLocalTime
  TimeStamp := DateTimeToTimeStamp(Result);
  Dec(TimeStamp.Time, UnixBias * 1000);
  Result := TimeStampToDateTime(TimeStamp);
end;

initialization

  BaseUnixDate := EncodeDate(1970, 1, 1);
  BaseUnixDays := Round(BaseUnixDate) + DateDelta;
  DecimalSeparator := SysUtils.DecimalSeparator;
  DateFmtStr := '%2.2d' + DateSeparator + '%2.2d' + DateSeparator + '%d';
  InitUnixBias;

end.
