�
 TLOGONFORM 0�  TPF0�
TLogonForm	LogonFormLeft�Top=ActiveControlParamsBorderStylebsDialogCaption;   Подключение к торговому серверуClientHeight#ClientWidth�OldCreateOrderPositionpoScreenCenterOnCreate
FormCreate	OnDestroyFormDestroyPixelsPerInch`
TextHeight TLabelLabel3LeftTop$Width� HeightCaption*   Параметры подключения:  TLabel
lblSpecDllLefttTop� WidthQHeight5AutoSizeCaption'   Требуется спец. MTESRL.DLLLayouttlBottomWordWrap	  TButtonOkBtnLeft� Top WidthKHeightCaptionOKDefault	ModalResultTabOrderOnClick
OkBtnClick  TButton	btnCancelLeftTop WidthKHeightCancel	Caption   B<5=0ModalResultTabOrder  	TRichEditParamsLeftTop4WidthYHeight� 
ScrollBarsssBothTabOrderWordWrap	OnKeyDownParamsKeyDownOnKeyUpParamsKeyUp	OnMouseUpParamsMouseUp  TButtonbtnComLefttTopWidthKHeightCaptionRS-232TabOrderOnClickbtnComClick  TButtonbtnPipeLefttTopHWidthKHeightCaptionNetBEUITabOrderOnClickbtnPipeClick  TButtonbtnTCPLefttTophWidthKHeightCaptionTCP/IPTabOrderOnClickbtnTCPClick  TButtonbtnLocalLefttTop� WidthKHeightCaptionLocalTabOrder	OnClickbtnLocalClick  TButtonbtnEPTSLefttTop� WidthKHeightCaptionEPTSTabOrder
OnClickbtnEPTSClick  	TComboBox
cbProfilesLeftTopWidthYHeightStylecsOwnerDrawFixed
ItemHeightTabOrder OnChangecbProfilesChange
OnDrawItemcbProfilesDrawItem  TButtonbtnTEAPLefttTop(WidthKHeightCaptionTEAPTabOrderOnClickbtnTEAPClick  	TCheckBoxcbSoftConnectLeftTopWidth� HeightHint�   Использовать функцию MTESoftConnect() - просто подключиться к TEServer, но не проверять наличие ТС ММВБCaption$   Не входить в ТС ММВБTabOrder   