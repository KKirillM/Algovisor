unit acOBStats;

interface

uses
  SysUtils, Classes, Windows;

procedure ClearForSecurity(Data: PChar);
procedure UpdateOrderbook(ObList: TList);

implementation

const
  WantedSec: array [1..16] of Char = 'EQBREESR        ';

type
  TOrderBookRec = record
    SecBoard: array [1..4] of Char;
    SecCode: array [1..12] of Char;
    BuySell: Char;
    Price: array [1..18] of Char;
    Quanity: array [1..11] of Char;
  end;

  TOrderBook = array [1..20] of TOrderBookRec;
  POrderBook = ^TOrderBook;

const
  OB_MIDDLE = (High(TOrderBook) - Low(TOrderBook) + 1) div 2;

var
  PrevChangeCount: Cardinal = 0;
  ChangeCount: Cardinal = 0;
  TotalGotRows: Cardinal = 0;
  TotalChangedRows: Cardinal = 0;
  BuyChangeCount: Cardinal = 0;
  SellChangeCount: Cardinal = 0;
  PrevOb, Ob: TOrderBook;

procedure ClearForSecurity(Data: PChar);
begin
  if CompareMem(@WantedSec, Data, SizeOf(WantedSec)) then Inc(ChangeCount);
end;

procedure UpdateOrderbook(ObList: TList);
var
  I, ObCount: Integer;
  BuyChanged, SellChanged: Boolean;
begin
  if PrevChangeCount = ChangeCount then Exit;
  PrevChangeCount := ChangeCount;

  ObCount := Low(TOrderBook);
  for I := 0 to ObList.Count - 1 do
    if (ObCount <= High(TOrderBook)) and CompareMem(@WantedSec, ObList.Items[I], SizeOf(WantedSec)) then
    begin
      Move(ObList.Items[I]^, Ob[ObCount], SizeOf(TOrderBookRec));
      Inc(ObCount);
      Inc(TotalGotRows);
    end;

  // align buy/sell orderbook for 10 buys and 10 sells
  for I := Low(TOrderBook) to High(TOrderBook) do
    if Ob[I].BuySell = 'S' then
    begin
      if I < Low(TOrderBook) + 10 then
      begin
        Move(Ob[Low(TOrderBook)], Ob[Low(TOrderBook) + OB_MIDDLE - I + Low(TOrderBook)],
          (I - Low(TOrderBook) + 1) * SizeOf(TOrderBookRec));
        FillChar(Ob, (Low(TOrderBook) + OB_MIDDLE - I) * SizeOf(TOrderBookRec), #32);
      end;
      Break;
    end;

  // now compare ob and prevob
  BuyChanged := False;
  SellChanged := False;
  for I := Low(TOrderBook) to High(TOrderBook) do
    if not CompareMem(@Ob[I], @PrevOb[I], SizeOf(TOrderBookRec)) then
    begin
      Inc(TotalChangedRows);
      if I - Low(TOrderBook) < OB_MIDDLE then Inc(BuyChanged) else Inc(SellChanged); 
    end;
  if BuyChanged then Inc(BuyChangeCount);
  if SellChanged then Inc(SellChangeCount);

  PrevOb := Ob;

  // print results
  AllocConsole;
  Writeln(Format('[%s] all_ch: %d  buy_ch: %d  sell_ch: %d  all_rows: %d  ch_rows: %d',
    [TimeToStr(Time), ChangeCount, BuyChangeCount, SellChangeCount, TotalGotRows, TotalChangedRows]));  
end;

initialization
  FillChar(Ob, SizeOf(Ob), #32);
  FillChar(PrevOb, SizeOf(PrevOb), #32);

end.
