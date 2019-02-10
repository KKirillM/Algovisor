#pragma once

#include "stdafx.h"

#include "resource.h"
#include "Instrument.h"
#include "LoginDlg.h"
#include "DEFINE.h"

class BaseChart;
class QuantChart;
class HeatMap;
class MainWindow;
class IInstrument;
struct Price;

extern MainWindow* pMainWnd;

extern DWORD iNumberOfProcessors;			//���������� �����������(����) � �������
extern CString login;						//��� ������������ (���������� ������ ��� ��� ����������� �����)

enum FeedType { EMPTY, RITHMIC, CQG, NINJATRADER, PLAZA2, PLAZA2REPLAY, ASTS, ANY };

extern std::list<HANDLE> hPipeServer, hPipeThreads;
extern HANDLE hGlobalMutex, hPipeThread;	//������ ��� ����-������� ��� ����� � ������ � �������� ��� �������������
extern SRWLOCK srwLockNinja;				//��� ��� ��������� �� �����
extern SRWLOCK SRWLockRithmic;				//��� ��� �������

static UINT indicators[] =					// �������������� ��� ������ ���������
{
	//ID_SEPARATOR,
	IDS_ALGOVISOR_COPYRIGHT,
	IDS_STRING_FOR_RIMICLOGO,
	IDS_STRING_FOR_OMNELOGO
};

template <class T>
class ninjaList
{
public:
	ninjaList() { ::InitializeSRWLock(&srwListLock); }

	void push_back(T element)
	{
		::AcquireSRWLockExclusive(&srwListLock);
		data.push_back(element);
		::ReleaseSRWLockExclusive(&srwListLock);
	}

	T front()
	{
		T e;
		::AcquireSRWLockExclusive(&srwListLock);
		e = data.front();
		::ReleaseSRWLockExclusive(&srwListLock);
		return e;
	}

	void pop_front()
	{
		::AcquireSRWLockExclusive(&srwListLock);
		data.pop_front();
		::ReleaseSRWLockExclusive(&srwListLock);
	}

	void clear()
	{
		::AcquireSRWLockExclusive(&srwListLock);
		data.clear();
		::ReleaseSRWLockExclusive(&srwListLock);
	}

	std::list<T> data;
	SRWLOCK srwListLock;
};

//class StatusBar : public CStatusBar
//{
//	#if _MFC_VER < 0x400
//       virtual void DoPaint(CDC* pDC);
//   #endif
//
//   #if _MFC_VER >= 0x400
//       virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
//   #endif
//};

using namespace rapidjson;

struct MyHandler 
{
    bool Null() { return true; }
    bool Bool(bool ) { 
		return true;
	}
    bool Int(int ) { 
		return true; 
	}
    bool Uint(unsigned ) { 
		return true; 
	}
    bool Int64(int64_t ) { 
		return true; 
	}
    bool Uint64(uint64_t ) { 
		return true; 
	}
    bool Double(double ) { 
		return true; 
	}
    bool RawNumber(const char* str, SizeType length, bool copy) { 
        strings.push_back(std::string(str, length));
        return true;
    }
    bool String(const char* str, SizeType length, bool ) { 
		strings.push_back(std::string(str, length));
        return true;
    }
    bool StartObject() { 
		return true; 
	}
    bool Key(const char* , SizeType , bool ) 
	{
        return true;
    }
    bool EndObject(SizeType ) {  
		return true;
	}
    bool StartArray() { return true; }
    bool EndArray(SizeType ) { return true; }

	std::vector<std::string> strings;
};

struct computerInfo
{
	computerInfo() 
	{
		userName[0] = 0;
		computerName[0] = 0;
		discMask = 0;
		driveSerial = 0;
		HDD_HardwareSerial = NULL;
		memorySize = 0;
		processorArch = 0;	
		numbersOfProc = 0;
		processorRevision = 0;
		majorVerOs = 0;	
		minorVerOs = 0;	
		bildNumberOs = 0;
		machineGUID = NULL;
		cxScreen = 0;
		cyScreen = 0;
		biosVer = NULL;
		biosDate = NULL;
		multiBiosVer = NULL;
	}

	wchar_t userName[UNLEN];				//��� ������������
	wchar_t computerName[UNLEN];			//��� ����������
	
	DWORD discMask;							//�����-������� �����, � ������� �������� ��� ��������� �����(���� ��� 0 ����� 1, �� ���� "�:\" ������������ � �.�.)
	DWORD driveSerial;						//�������� ����� ����� � �������� �������� ��������� (���� ����� ����������� windows ����� ��������������, �.�. ����� ��������)
	wchar_t* HDD_HardwareSerial;			//���������� �������� ����� �������� �����, � �������� �������� ���������
	
	size_t memorySize;						//������ ����������(�����������) ������ � ������
	
	DWORD processorArch;					//����������� ����������: 0 - x86, 9 - x64 (Intel or AMD), 0xffff - unknown, 6 - Intel Itanium-based
	DWORD numbersOfProc;					//���������� ����������� (���������� ����)
	DWORD processorRevision;				//������� ����������

	DWORD majorVerOs;						//�������� ������ ��
	DWORD minorVerOs;						//�������������� ������ ��
	DWORD bildNumberOs;						//����� ������

	wchar_t* machineGUID;					//guid ������� (�������� ����� ������ ���������)

	int cxScreen;							//������ ������ � ��������
	int cyScreen;							//������ ������ � ��������

	std::vector<wchar_t*> macs;				//��� ������ ���� ���������
	wchar_t* biosVer;						//������ ������ �����
	wchar_t* biosDate;						//������ � ����� �����
	BYTE* multiBiosVer;
};

//������ ��� ���� ������� �� ������ BaseChart
struct BaseChartData
{
	void SetData(BaseChart* base, const size_t _start, const size_t _end, std::vector<std::vector<std::pair<size_t,size_t>>>* pTimeLabelsVector, 
				 std::vector<Price>* maxPoolPrice,std::vector<Price>* minPoolPrice,std::vector<Price>* lastBidPoolPrice,std::vector<Price>* lastAskPoolPrice, volatile LONG* numOfPulThread);

	volatile LONG* numberOfPoolThread;
	IInstrument* _i;
	size_t start;
	size_t end;
	int scale_type;
	std::vector<Price>* maxPrice;
	std::vector<Price>* minPrice;
	std::vector<Price>* lastBidPrice;
	std::vector<Price>* lastAskPrice;
	std::vector<std::vector<std::pair<size_t,size_t>>>* timeLabelsVector;
};

struct Coordinate
{
	std::pair<int,int> From;
	std::pair<int,int> To;
	COLORREF color;
};

struct LabelsCoordinate
{
	UINT size; 
	COLORREF color1;
	COLORREF color2; 
	int Ycoord;
};

//������ ��� ���� ������� �� ������ BaseChart
struct QuantChartData
{
	void SetData(QuantChart* _base, const size_t _start, const size_t _end, Price* _scaleMaxPrice, Price* _p1, Price* _p2, const double _maxDomValue, const double _count,
				 std::vector<std::vector<Coordinate>>* _drawVector, std::vector<LabelsCoordinate>* _labelsVector, volatile LONG* numOfPulThread);

	volatile LONG* numberOfPoolThread;
	QuantChart* base;
	size_t start;
	size_t end;
	Price* scaleMaxPrice;
	Price* p1;
	Price* p2; 
	double maxDomValue;
	double count;
	std::vector<std::vector<Coordinate>>* drawVector;
	std::vector<LabelsCoordinate>* labelsVector;
};

//���������� ��������� ��� ���� �������
VOID CALLBACK FindMaxMinPrices(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);
VOID CALLBACK CalculateCoordinates(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work);

//����� ��� ���� ������� ��� ������������ Quant Chart � Quote Mapper
class ThreadPool
{
public:

	ThreadPool() : numberOfPoolThread(-1), gWorkItem1(NULL), gWorkItem2(NULL), gWorkItem3(NULL)
	{
	}

	~ThreadPool()
	{
		//��������� ������� ������� ���� �������
		if (gWorkItem1)
			CloseThreadpoolWork(gWorkItem1);
		if (gWorkItem2)
			CloseThreadpoolWork(gWorkItem2);
		if (gWorkItem3)
			CloseThreadpoolWork(gWorkItem3);
	}

	void CreatePool(int n) 
	{
		switch (n)
		{
		case 1:
			if (!gWorkItem1)
				gWorkItem1 = CreateThreadpoolWork(FindMaxMinPrices, (PVOID)&baseData, NULL);
			break;
		case 2:
			if (!gWorkItem2)
				gWorkItem2 = CreateThreadpoolWork(CalculateCoordinates, (PVOID)&quantData, NULL);
			break;
		case 3:
			if (!gWorkItem3)
				gWorkItem3 = CreateThreadpoolWork(FindMaxMinPrices, (PVOID)&baseData, NULL);
			break;
		}
	}

	void SetBaseData(BaseChart* base, const size_t _start, const size_t _end, std::vector<std::vector<std::pair<size_t,size_t>>>* pTimeLabelsVector, 
					 std::vector<Price>* maxPoolPrice, std::vector<Price>* minPoolPrice,std::vector<Price>* lastBidPoolPrice,std::vector<Price>* lastAskPoolPrice)
	{
		baseData.SetData(base, _start, _end, pTimeLabelsVector, maxPoolPrice, minPoolPrice, lastBidPoolPrice, lastAskPoolPrice, &numberOfPoolThread);
	}

	void SetQuantData(QuantChart* _base, const size_t _start, const size_t _end, Price* _scaleMaxPrice, Price* _p1, Price* _p2, const double _maxDomValue, const double _count,
					  std::vector<std::vector<Coordinate>>* _drawVector, std::vector<LabelsCoordinate>* _labelsVector)
	{
		quantData.SetData(_base, _start, _end, _scaleMaxPrice, _p1, _p2, _maxDomValue, _count, _drawVector, _labelsVector, &numberOfPoolThread);
	}

	void StartNewThread(int n)
	{
		switch (n)
		{
		case 1: SubmitThreadpoolWork(gWorkItem1); break;
		case 2: SubmitThreadpoolWork(gWorkItem2); break;
		case 3: SubmitThreadpoolWork(gWorkItem3); break;
		}		
	}

	void WaitForThread(int n)
	{
		switch (n)
		{
		case 1: WaitForThreadpoolWorkCallbacks(gWorkItem1, FALSE); numberOfPoolThread = -1; break;
		case 2: WaitForThreadpoolWorkCallbacks(gWorkItem2, FALSE); numberOfPoolThread = -1; break;
		case 3: WaitForThreadpoolWorkCallbacks(gWorkItem3, FALSE); numberOfPoolThread = -1; break;
		}	
	}

private:
	
	PTP_WORK gWorkItem1;				//������� ������� ��� ���� �������
	PTP_WORK gWorkItem2;				//������� ������� ��� ���� �������
	PTP_WORK gWorkItem3;				//������� ������� ��� ���� �������
	BaseChartData baseData;				//������ ��� ������ ����
	QuantChartData quantData;
	volatile LONG numberOfPoolThread;	//��� ������������� ���� �������
};

std::string MD5_String(const char* str);
std::string UrlDecode(const std::string & sSrc);
std::string UrlEncode(const std::string & sSrc);
void GetHDDSerial(wchar_t** str);
/*PROTECT_FUNC*/ computerInfo SFLB_0_GetComputerInfo();
/*PROTECT_FUNC*/ CString SFLB_0_CryptString(const CString& str);
/*PROTECT_FUNC*/ std::string SFLB_0_CryptString(const std::string& str);
/*PROTECT_FUNC*/ tstring& SFLB_0_CryptStringW(tstring& str);
PROTECT_FUNC wchar_t* SFLB_0_encryptRSA(char* sourceString, std::string& encryptString);
PROTECT_FUNC const int SFLB_0_decryptRSA(std::string& answ);
PROTECT_FUNC tstring SFLB_0_SendCurlRequest(std::string& req, std::string& answer);
/*PROTECT_FUNC*/ VOID SFLB_0_PushOk(LoginDlg* dlg, PsProtUInt32_1 i, std::string& result);
void GetTradesOnlyInVector(IInstrument*& source, IInstrument*& dest, size_t& dest_index);
void DrawUnsubscribeString(HDC dc, const RECT& rt, const FeedType ft, const char* name);
void DrawButton(HDC dc, RECT& rt, const RECT& rt2, const RECT& rt3, const wchar_t* name, const COLORREF color1, const COLORREF color2);
void DrawHistoryLoad(HDC dc, const RECT& rt);
UINT GetRangeIndexes(HWND dc, IInstrument* _i, size_t& start, size_t& end, RECT& rt);
void DrawCopyright(HDC dc, const RECT& rt);
void DrawRithmicCopyright(HDC dc, const RECT& rt);
void Disconnect_Algovisor_Server();
int CheckProcessInMemory(const wchar_t* process_name);
void FindMaxMinPriceInRange(size_t _start, size_t _end, size_t x, IInstrument* _i, Price& maxP, Price& minP, 
							int scale_type, Price& lastBidP, Price& lastAskP, Price* lastP, std::vector<std::vector<std::pair<size_t,size_t>>>* pTimeLabelsVector, size_t indx);
void GetDOMCoordinates(const QuantChart* base, const size_t _start, const size_t _end, IInstrument* _i, const Price& scaleMaxPrice, Price& p1, Price& p2, const double maxDomValue, 
					   const double count, const int indx, const size_t distance, std::vector<std::vector<Coordinate>>* drawVector, std::vector<LabelsCoordinate>* labelsVector);

int StartPipeServerForNinja();
static unsigned int WINAPI PipeThreadFunction(void* arg);
UINT TypeTextToActivMon(LPVOID p);
std::wstring wide_string(std::string const &s, std::locale const &loc);
std::string narrow_string(std::wstring const &s, std::locale const &loc, char default_char = '?');