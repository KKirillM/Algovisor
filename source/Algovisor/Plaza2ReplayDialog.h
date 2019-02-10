#pragma once


#include "..\Plaza2Utils\EDBStorageReader.h"
#include "resource.h"
#include "..\Plaza2_Connector\Plaza2_Tables.h"
#include "IPlaza2_Connector.h"
#include "..\Plaza2Utils\p2time.h"
#include <atomic>
#include <future>


enum class ReplayStatus
{
	DBCLOSED,
	DBOPENED,
	DBPLAY,
	DBPAUSE,
	DBRESTART
};

class ReplayListener;

class Plaza2ReplayDialog : public CDialogEx
{
public:
	typedef std::vector<FORTS_FUTINFO_REPL::fut_sess_contents> IsinList;
	typedef std::unordered_map<int, ReplayListener> ListnerList;

	enum { IDD = DIALOG_PLAZA2_REPLAY };
	enum MessageType { DEAL, ORDLOG };

	Plaza2ReplayDialog();
	virtual ~Plaza2ReplayDialog();

	BOOL OnInitDialog();
	void SetStartRevision(INT64 rev) { startRevision = rev; }
	void SetLastRevision(INT64 rev) { lastRevision = rev; }
	IsinList& GetIsinList() { return isinList; }
	void AddIsin(const FORTS_FUTINFO_REPL::fut_sess_contents& isin) 
	{
		auto it = std::find_if(isinList.begin(), isinList.end(), [&isin](const IsinList::value_type& t)->bool {
			if (t.isin_id == isin.isin_id)
				return true;
			return false;
		});
		if (it == isinList.end())
			isinList.push_back(isin);	
	}

	static P2ERR OpenDBCallback(const TEReplRec* pRec, void* param);
	static P2ERR ReplayDBCallback(const TEReplRec* pRec, void* param);

	ReplayStatus GetStatus() const { return status; }

private:
	void OpenDB(const std::string& fullPath);
	void CloseDB();
	void Pause();
	void GetDate();
	void GetTime();
	fo2base::UnixNanosecondsTime NewReplayTime();
	void StartWaitCursor();
	void StopWaitCursor();
	void UpdateControls();
	void ProcessMessage(const MessageType type, msg_streamdata_t* data, int isin_id, uint64_t moment_ns);

private:
	std::future<void> replayThread;
	fo2base::CP2Time startTime;			// дата и время указанные в диалоговом окне при открытии базы
	fo2base::CP2Time startSessionDate;	// дата начала торговой сессии в реплее
	std::string pathToDB;
	EDBStorageReader edbReader;
	INT64 startRevision;
	INT64 lastRevision;
	ReplayStatus status;
	std::atomic<bool> replayFlag;
	IsinList isinList;
	ListnerList listenerList;
	fo2base::UnixNanosecondsTime lastReplayTime;
	fo2base::UnixNanosecondsTime startReplayTime;
	fo2base::UnixNanosecondsTime startLocalTime;
	INT64 ticksToStop;


private:
	afx_msg void OnBnClickedPlaza2DbOpen();
	afx_msg void OnBnClickedButtonPlaza2ReplayStart();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()

	friend class MainWindow;

public:
	afx_msg void OnBnClickedButtonRestart();
	afx_msg void OnBnClickedButtonPlaza2ReplayNext();
};	// Plaza2ReplayDialog


class ReplayListener : public IListener
{
public:
	ReplayListener(int isin, int prc) : isin_id(isin), precision(prc) {}
	int GetIsin() const override { return isin_id; }
	int GetPrecision() const override { return precision; }
	bool IsCurrentSession(int sessionID) override { return true; }
	const std::string& GetStreamName() const override { return ""; }
	P2StreamID GetStreamID() const override { return (P2StreamID)0; }
	const std::string& GetReplState() const override { return ""; }
	void SaveReplstateToFile(const std::string& replFilePath, const char* data, const size_t size) override {}
	void SetReplState(const std::string& _replstate) override {}

private:
	int isin_id;
	int precision;

};	// ReplayListener