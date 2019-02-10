#include "stdafx.h"
#include "Plaza2ReplayDialog.h"
#include "IniFileWorker.h"
#include "MainWindow.h"
#include "Plaza2Instruments.h"
#include "Plaza2Callbacks.h"
#include "..\Plaza2_Connector\IPlaza2_Connector.h"
#include <cstring>
#include <chrono>


using namespace std;
using namespace fo2base;
using namespace chrono;


Plaza2ReplayDialog::Plaza2ReplayDialog() 
	: startRevision(0)
	, lastRevision(0)
	, status(ReplayStatus::DBCLOSED)
	, replayFlag(false)
	, startReplayTime(0)
	, startLocalTime(0)
	, lastReplayTime(0)
	, ticksToStop(0)
{
}


Plaza2ReplayDialog::~Plaza2ReplayDialog()
{
	if (status != ReplayStatus::DBCLOSED)
	{
		CloseDB();
	}
	pMainWnd->plaza2ReplayDlg = NULL;
}


BEGIN_MESSAGE_MAP(Plaza2ReplayDialog, CDialogEx)
	ON_BN_CLICKED(BUTTON_PLAZA2_DB_OPEN, &Plaza2ReplayDialog::OnBnClickedPlaza2DbOpen)
	ON_BN_CLICKED(IDC_BUTTON_PLAZA2_REPLAY_START, &Plaza2ReplayDialog::OnBnClickedButtonPlaza2ReplayStart)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &Plaza2ReplayDialog::OnBnClickedButtonRestart)
	ON_BN_CLICKED(IDC_BUTTON_PLAZA2_REPLAY_NEXT, &Plaza2ReplayDialog::OnBnClickedButtonPlaza2ReplayNext)
END_MESSAGE_MAP()


BOOL Plaza2ReplayDialog::OnInitDialog()
{
	IniFileWorker ini;

	try
	{
		ini.Open(".\\ini\\Algovisor.ini");
		pathToDB = ini.ReadStr("Plaza2Replay", "DB");
	}
	catch (IniFileException&)
	{
		pMainWnd->UpdateActivityWindow(_T("ERROR: Algovisor.ini has no DB parameter in [Plaza2Replay] section"));
	}

	ini.Close();

	UpdateControls();

	return CDialogEx::OnInitDialog();
}


void Plaza2ReplayDialog::GetDate()
{
	CDateTimeCtrl* dateWidget = static_cast<CDateTimeCtrl*>(GetDlgItem(IDC_DATE_PLAZA2_REPLAY));
	COleDateTime date;
	dateWidget->GetTime(date);
	startTime.year = date.GetYear();
	startTime.month = date.GetMonth();
	startTime.day = date.GetDay();
}


void Plaza2ReplayDialog::GetTime()
{
	CDateTimeCtrl* timeWidget = static_cast<CDateTimeCtrl*>(GetDlgItem(IDC_TIME_PLAZA2_REPLAY2));
	COleDateTime date;
	timeWidget->GetTime(date);
	startTime.hour = date.GetHour();
	startTime.minute = date.GetMinute();
	startTime.second = date.GetSecond();
	startSessionDate.hour = date.GetHour();
	startSessionDate.minute = date.GetMinute();
	startSessionDate.second = date.GetSecond();
}


UnixNanosecondsTime Plaza2ReplayDialog::NewReplayTime()
{
	return (startTime.GetHour() == 18 && startTime.GetMinute() > 45) || startTime.GetHour() > 18
		? startSessionDate.ToUnixNanosecondsTime()
		: startTime.ToUnixNanosecondsTime();
}


void Plaza2ReplayDialog::StartWaitCursor()
{
	EnableWindow(0);
	BeginWaitCursor();
}


void Plaza2ReplayDialog::StopWaitCursor()
{
	EndWaitCursor();
	EnableWindow(1);
	Invalidate(0);
}


string GetFolderNameFromSystemtime(const fo2base::CP2Time& date)
{
	char buf[100];
	sprintf(buf, "%02d-%02d-%04d", date.day, date.month, date.year);
	return buf;
}


void Plaza2ReplayDialog::OnBnClickedPlaza2DbOpen()
{
	GetDate(), GetTime();
	string fullPath = pathToDB + GetFolderNameFromSystemtime(startTime) + "\\Plaza2_AllData.edb";
	CString buttonText;
	GetDlgItemText(BUTTON_PLAZA2_DB_OPEN, buttonText);
	StartWaitCursor();
	if (buttonText == "Open DB")
	{
		OpenDB(fullPath);
	}
	else
	{
		CloseDB();
	}
	pMainWnd->RedrawWindow();
	RedrawWindow();
	UpdateControls();
	StopWaitCursor();
}


void Plaza2ReplayDialog::OpenDB(const string& fullPath)
{
	if (edbReader.Open(fullPath))
	{
		pMainWnd->UpdateActivityWindow((_T("ERROR: Can't open EDB-file: ") + wstring(fullPath.begin(), fullPath.end())).c_str());
		return;
	}
	status = ReplayStatus::DBOPENED;
	pMainWnd->UpdateActivityWindow((_T("Start reading EDB-file: ") + wstring(fullPath.begin(), fullPath.end())).c_str());
	EDBStorabeCallback func = Plaza2ReplayDialog::OpenDBCallback;
	edbReader.StartRead(func, this);
	std::sort(isinList.begin(), isinList.end(),
		[](const FORTS_FUTINFO_REPL::fut_sess_contents& f1, const FORTS_FUTINFO_REPL::fut_sess_contents& f2) -> bool {
		return strcmp(f1.isin, f2.isin) < 0 ? true : false;
	});
	startReplayTime = NewReplayTime();
	replayThread = async(launch::async, [this]{
		EDBStorabeCallback func = Plaza2ReplayDialog::ReplayDBCallback;
		this->edbReader.StartRead(func, this, this->startRevision);
	});
	pMainWnd->AddNewDatafeedToComboBox(_T("Plaza2Replay"));
	pMainWnd->OnSelchangeDatafeedCombobox();
}


void Plaza2ReplayDialog::CloseDB()
{
	status = ReplayStatus::DBCLOSED;
	replayFlag = true;
	replayThread.wait();
	replayFlag = false;
	startRevision = 0;
	isinList.clear();
	listenerList.clear();
	edbReader.Close();
	startLocalTime = 0;
	startReplayTime = 0;
	pMainWnd->RemoveDatafeedFromComboBox(_T("Plaza2Replay"));
	pMainWnd->OnBnClickedUnsubscribeAll();
	pMainWnd->UpdateActivityWindow(_T("Plaza2Replay: DB file closed"));
}


void Plaza2ReplayDialog::OnBnClickedButtonPlaza2ReplayStart()
{
	if (status == ReplayStatus::DBCLOSED)
	{
		return;
	}

	if (pMainWnd->instrumentList.GetItemCount() == 0)
	{
		pMainWnd->UpdateActivityWindow(_T("Plaza2Replay: Please subscribe any instrument"));
		return;
	}

	if (status == ReplayStatus::DBPAUSE || status == ReplayStatus::DBOPENED || status == ReplayStatus::DBRESTART)
	{
		if (status != ReplayStatus::DBRESTART)
			status = ReplayStatus::DBPLAY;
		replayFlag = true;
		startLocalTime = 0;
		if (lastReplayTime != 0)
			startReplayTime = lastReplayTime;
	}
	else
	{
		status = ReplayStatus::DBPAUSE;
		replayFlag = false;
	}
	pMainWnd->RedrawWindow();
	RedrawWindow();
	UpdateControls();
}


void Plaza2ReplayDialog::OnBnClickedButtonRestart()
{
	GetTime();
	UnixNanosecondsTime newReplayTime = NewReplayTime();
	if (newReplayTime > lastReplayTime)
	{
		startReplayTime = newReplayTime;
		startLocalTime = 0;
		replayFlag = true;
		status = ReplayStatus::DBRESTART;
		RedrawWindow();
		UpdateControls();
		return;
	}

	{
		status = ReplayStatus::DBCLOSED;
		replayFlag = true;
		replayThread.wait();
		replayFlag = false;
	}

	{
		startReplayTime = newReplayTime;
		lastReplayTime = 0;
	}

	replayThread = async(launch::async, [this] {
		EDBStorabeCallback func = Plaza2ReplayDialog::ReplayDBCallback;
		this->edbReader.StartRead(func, this, this->startRevision);
	});
	{	// очищаем стаканы по всем инструментам
		Plaza2Callbacks::ClearAll();
		int selRow = pMainWnd->instrumentList.GetItemCount();
		for (int j = 0; j < selRow; ++j)
		{
			CString feed(pMainWnd->instrumentList.GetItemText(j, 1));
			if (feed == _T("Plaza2Replay"))
			{
				CStringA isin_id(pMainWnd->instrumentList.GetItemText(j, 3));
				auto instrumentsDB = Plaza2DB::GetInstance();
				Plaza2Instrument* instrumentPtr = instrumentsDB->GetInstrumentPtrByKey(atoi(isin_id));
				instrumentPtr->Clear();
			}
		}
	}
	status = ReplayStatus::DBRESTART;
	OnBnClickedButtonPlaza2ReplayStart();
}


P2ERR Plaza2ReplayDialog::OpenDBCallback(const TEReplRec* pRec, void* param)
{
	Plaza2ReplayDialog* replayDlg = static_cast<Plaza2ReplayDialog*>(param);

	int64_t* ptr = (int64_t*)((char*)pRec + sizeof(TEReplRec));
	char* beginOfData = (char*)ptr + sizeof(INT64);

	switch (pRec->eventType & ~(0xFF))
	{
	case P2StreamID::p2repl_FORTS_UNKNOWN:
		static_cast<MainWindow*>(replayDlg->GetParent())->
			UpdateActivityWindow((_T("ERROR: ReplayCallback message type: FORTS_UNKNOWN is wrong")));
		return P2ERR_NOT_OK;

	case P2StreamID::p2repl_FORTS_FUTINFO_REPL:
		if (pRec->eventType == P2StreamID::p2repl_FORTS_FUTINFO_REPL + FORTS_FUTINFO_REPL::fut_sess_contents_index)
		{
			FORTS_FUTINFO_REPL::fut_sess_contents& info = *reinterpret_cast<FORTS_FUTINFO_REPL::fut_sess_contents*>(beginOfData);
			replayDlg->AddIsin(info);	
		}
		break;

	case P2StreamID::p2repl_FORTS_ORDLOG_REPL:
		if (pRec->eventType == P2StreamID::p2repl_FORTS_ORDLOG_REPL + FORTS_ORDLOG_REPL::sys_events_index)
		{
			FORTS_ORDLOG_REPL::sys_events& info = *reinterpret_cast<FORTS_ORDLOG_REPL::sys_events*>(beginOfData);
			if (info.event_type == 8) // BROKER_RECALC_FINISHED, 10 13 8
			{
				replayDlg->SetStartRevision(pRec->rev);
				return P2ERR_COMMON_EXITCODE_TERMINATED;
			}
		}
		else if (pRec->eventType == P2StreamID::p2repl_FORTS_ORDLOG_REPL + FORTS_ORDLOG_REPL::heartbeat_index)
		{
			if (!replayDlg->startSessionDate.IsNull())
			{
				FORTS_ORDLOG_REPL::heartbeat& info = *reinterpret_cast<FORTS_ORDLOG_REPL::heartbeat*>(beginOfData);
				replayDlg->startSessionDate.day = info.server_time.day;
				replayDlg->startSessionDate.month = info.server_time.month;
				replayDlg->startSessionDate.year = info.server_time.year;
			}
		}
	}

	return P2ERR_OK;
}


P2ERR Plaza2ReplayDialog::ReplayDBCallback(const TEReplRec* pRec, void* param)
{
	Plaza2ReplayDialog* replayDlg = static_cast<Plaza2ReplayDialog*>(param);
	while (!replayDlg->replayFlag)
	{
		Sleep(1);
	}

	if (replayDlg->status == ReplayStatus::DBCLOSED)
	{
		replayDlg->SetLastRevision(pRec->rev);
		return P2ERR_COMMON_EXITCODE_TERMINATED;
	}

	int64_t* ptr = (int64_t*)((char*)pRec + sizeof(TEReplRec));
	char* beginOfData = (char*)ptr + sizeof(INT64);
	msg_streamdata_t data;
	data.data = (void*)beginOfData;
	data.msg_index = 2;

	switch (pRec->eventType & ~(0xFF))
	{
	case P2StreamID::p2repl_FORTS_UNKNOWN:
		static_cast<MainWindow*>(replayDlg->GetParent())->
			UpdateActivityWindow((_T("ERROR: ReplayCallback message type: FORTS_UNKNOWN is wrong")));
		return P2ERR_NOT_OK;

	case P2StreamID::p2repl_FORTS_DEALS_REPL:
		if (pRec->eventType == P2StreamID::p2repl_FORTS_DEALS_REPL + FORTS_DEALS_REPL::deal_index)
		{
			FORTS_DEALS_REPL::deal& info = *reinterpret_cast<FORTS_DEALS_REPL::deal*>(beginOfData);
			replayDlg->ProcessMessage(DEAL, &data, info.isin_id, info.moment_ns);
		}
		break;

	case P2StreamID::p2repl_FORTS_ORDLOG_REPL:
		if (pRec->eventType == P2StreamID::p2repl_FORTS_ORDLOG_REPL + FORTS_ORDLOG_REPL::orders_log_index)
		{
			FORTS_ORDLOG_REPL::orders_log& info = *reinterpret_cast<FORTS_ORDLOG_REPL::orders_log*>(beginOfData);
			replayDlg->ProcessMessage(ORDLOG, &data, info.isin_id, info.moment_ns);
		}
		break;
	}

	return P2ERR_OK;
}


void Plaza2ReplayDialog::ProcessMessage(const MessageType type, msg_streamdata_t* data, int isin_id, uint64_t moment_ns)
{
	const auto& it = listenerList.find(isin_id);
	if (it == listenerList.end())
	{
		return;
	}

	lastReplayTime = moment_ns;
	if (startReplayTime > moment_ns || ticksToStop != 0)
	{
		(type == DEAL) ? Plaza2Callbacks::Trade(data, &(it->second)) 
					   : Plaza2Callbacks::OrderBook(data, &(it->second));
		if (ticksToStop != 0)
		{
			if (--ticksToStop == 0)
			{
				Pause();
			}
		}
		return;
	}

	if (status == ReplayStatus::DBRESTART)
	{
		Pause();
	}

	{
		string timeString;
		CP2Time::FromUnixNanosecondsToString3(moment_ns + timezoneShift.get(), timeString);
		SetDlgItemText(IDC_STATIC_REPLAY_TIME, wstring(timeString.begin(), timeString.end()).c_str());
	}

	if (startLocalTime == 0)
	{
		startLocalTime = NowUnixNanosecondsTime();
	}
	uint64_t delta_local_time = NowUnixNanosecondsTime() - startLocalTime;
	uint64_t delta_replay_time = moment_ns - startReplayTime;
	while (delta_replay_time > delta_local_time && status == ReplayStatus::DBPLAY)
	{
		Sleep(1);
		delta_local_time = NowUnixNanosecondsTime() - startLocalTime;
	}
	(type == DEAL) ? Plaza2Callbacks::Trade(data, &(it->second)) 
				   : Plaza2Callbacks::OrderBook(data, &(it->second));
}


void Plaza2ReplayDialog::Pause()
{
	status = ReplayStatus::DBPAUSE;
	UpdateControls();
	replayFlag = false;
}


HBRUSH Plaza2ReplayDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == PLAZA2_REPLAY_STATUS)
	{
		pDC->SetBkMode(TRANSPARENT);
		switch (status)
		{
		case ReplayStatus::DBCLOSED:	hbr = ::CreateSolidBrush(RGB(192, 192, 192)); break;	//none серый
		case ReplayStatus::DBOPENED:	hbr = ::CreateSolidBrush(RGB(255, 155, 55)); break;		//wait коричневый
		case ReplayStatus::DBRESTART:
		case ReplayStatus::DBPLAY:		hbr = ::CreateSolidBrush(RGB(0, 255, 128));	break;		//connect ok зеленый
		case ReplayStatus::DBPAUSE:		hbr = ::CreateSolidBrush(RGB(0, 155, 255));	break;		//connect ok голубой
		}
	}
	return hbr;
}


void Plaza2ReplayDialog::OnClose()
{
	CDialogEx::OnClose();

	delete this;
}


void Plaza2ReplayDialog::UpdateControls()
{
	switch (status)
	{
	case ReplayStatus::DBCLOSED:
		GetDlgItem(IDC_BUTTON_PLAZA2_REPLAY_START)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_RESTART)->EnableWindow(0);
		GetDlgItem(IDC_DATE_PLAZA2_REPLAY)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_PLAZA2_REPLAY_NEXT)->EnableWindow(0);
		GetDlgItem(IDC_COMBO_NEXT)->EnableWindow(0);
		SetDlgItemText(BUTTON_PLAZA2_DB_OPEN, _T("Open DB"));
		SetDlgItemText(IDC_BUTTON_PLAZA2_REPLAY_START, _T("Play"));
		break;

	case ReplayStatus::DBOPENED:
		GetDlgItem(IDC_BUTTON_PLAZA2_REPLAY_START)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_RESTART)->EnableWindow(1);
		GetDlgItem(IDC_DATE_PLAZA2_REPLAY)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_PLAZA2_REPLAY_NEXT)->EnableWindow(0);
		GetDlgItem(IDC_COMBO_NEXT)->EnableWindow(0);
		SetDlgItemText(BUTTON_PLAZA2_DB_OPEN, _T("Close DB"));
		SetDlgItemText(IDC_BUTTON_PLAZA2_REPLAY_START, _T("Play"));
		break;

	case ReplayStatus::DBPLAY:
		GetDlgItem(IDC_BUTTON_PLAZA2_REPLAY_START)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_RESTART)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_PLAZA2_REPLAY_NEXT)->EnableWindow(0);
		GetDlgItem(IDC_COMBO_NEXT)->EnableWindow(0);
		SetDlgItemText(IDC_BUTTON_PLAZA2_REPLAY_START, _T("Pause"));
		break;

	case ReplayStatus::DBRESTART:
		GetDlgItem(IDC_BUTTON_PLAZA2_REPLAY_NEXT)->EnableWindow(0);
		GetDlgItem(IDC_COMBO_NEXT)->EnableWindow(0);
		break;

	case ReplayStatus::DBPAUSE:
		GetDlgItem(IDC_BUTTON_PLAZA2_REPLAY_START)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_RESTART)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_PLAZA2_REPLAY_NEXT)->EnableWindow(1);
		GetDlgItem(IDC_COMBO_NEXT)->EnableWindow(1);
		SetDlgItemText(IDC_BUTTON_PLAZA2_REPLAY_START, _T("Play"));
		break;
	}
}

void Plaza2ReplayDialog::OnBnClickedButtonPlaza2ReplayNext()
{
	CString nextStr;
	GetDlgItemText(IDC_COMBO_NEXT, nextStr);
	const fo2base::UnixNanosecondsTime second = 1000000000;
	if (nextStr == "1 Tick") {
		ticksToStop = 1;
	}
	else if (nextStr == "10 Tick") {
		ticksToStop = 10;
	}
	else if (nextStr == "100 Tick")	{
		ticksToStop = 100;
	}
	else if (nextStr == "1000 Tick") {
		ticksToStop = 1000;
	}
	else if (nextStr == "1 sec")	{
		startReplayTime = lastReplayTime + second;
	}
	else if (nextStr == "5 sec")	{
		startReplayTime = lastReplayTime + 5 * second;
	}
	else if (nextStr == "10 sec")	{
		startReplayTime = lastReplayTime + 10 * second;
	}
	else if (nextStr == "30 sec")	{
		startReplayTime = lastReplayTime + 30 * second;
	}
	else if (nextStr == "1 min")	{
		startReplayTime = lastReplayTime + 60 * second;
	}
	else if (nextStr == "5 min")	{
		startReplayTime = lastReplayTime + 5 * 60 * second;
	}
	else if (nextStr.IsEmpty())
	{
		return;
	}
	
	status = ReplayStatus::DBRESTART;
	replayFlag = true;
}
