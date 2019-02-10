#include "stdafx.h"
#include "Plaza2_Tables.h"
#include "IniFileWorker.h"
#include "HelpTools.h"
#include "Plaza2_defines.h"
#include "Listener.h"
#include "Plaza2_Connector.h"


Listener::Listener(conn_t** conn, const P2ListnerParameters& streamParams) :
	session_id(0),
	plaza2Listener(NULL), 
	plaza2ConnectionObject(conn),
    stream_id(P2StreamID::p2repl_FORTS_UNKNOWN)
{
    prefix = streamParams.prefix;
    stream_name = streamParams.name;
    if (!streamParams.postfixParams.empty())
    {
        posfix = streamParams.postfixParams;
    }
    if (!streamParams.tables.empty())
    {
        tables = "tables=" + streamParams.tables;
    }
    if (!streamParams.mode.empty())
    {
        mode = "mode=" + streamParams.mode;
    }

    isin = streamParams.isin;
    precision = streamParams.precision;
	session_id = streamParams.session_id;

    assert(streamParams.func1 != NULL && streamParams.func2 != NULL);
    func1 = streamParams.func1; func2 = streamParams.func2; 
}

Listener::~Listener()
{
	DestroyListener();
}

void Listener::CloseListener()
{
	if (plaza2Listener)
		WARN_FAIL(lsn_close(plaza2Listener));
}

void Listener::DestroyListener()
{
    if (plaza2Listener)
    {
        lsn_destroy(plaza2Listener);
        plaza2Listener = NULL;
    }
}

bool Listener::CheckListenerState()
{
	try
	{
		uint32_t state;
		if (plaza2Listener)
		{
			CHECK_PLAZA2_FAIL(lsn_getstate(plaza2Listener, &state));
		}
		else
		{
			state = CG_STATE_ERROR;
		}

		switch (state)
		{
		case CG_STATE_CLOSED:		//=0
			{
				log_info("Plaza2 listener %s state: CG_STATE_CLOSED, try to call cg_lsn_open()...", stream_name.c_str());
				CHECK_PLAZA2_FAIL(lsn_open(plaza2Listener, (mode + ';' + replstate).c_str()));
			}
			break;
		case CG_STATE_ERROR:		//=1
			{
				log_error("Plaza2 listener %s state: CG_STATE_ERROR, try to call cg_lsn_close()...", stream_name.c_str());
				CloseListener();
			}
			break;
		//case CG_STATE_OPENING:	//=2
		//case CG_STATE_ACTIVE:		//=3
		}
	}
	catch(std::exception& e)
	{
		char err_str[100];
		sprintf_s(err_str, 100, "Disconnect from repl stream: %s, %s", stream_name.c_str(), e.what());
		log_error(err_str);
		return false;
	}

	return true;
}

void Listener::SaveReplstateToFile(const std::string& replFilePath, const char* data, const size_t size)
{ 
	FILE* state_file;

	TCHAR currDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currDir);
	std::string fullPath = currDir + replFilePath + '\\' + stream_name + REPLSTATEFILE;

	state_file = std::fopen(fullPath.c_str(), "wb");
	if (state_file)
	{
		fwrite(data, 1, size, state_file);
		fclose(state_file);
		log_info("Replica state stored to file %s", stream_name.c_str());
	} 
	else
	{
		log_error("FAILED to store replica state to %s", stream_name.c_str());
	}
}

CG_RESULT Listener::MessageCallback(conn_t* conn, listener_t* listener, struct msg_t* message, void* data)
{
	Listener* _this = (Listener*)data;

	switch (message->type)
	{
	/* ��������� �������� � ������ ��������� ������ ������. ��� ������� ��������������
	/* ��������� �� ������� ����� ���� ������ �� ������ ��������. ��� ������� ����������
	/* ������ ��������� ��������, ��� ����� ������ ����������� � ������ ��� �������������
	/* (��������� ��. ����� ������) ������ ��������� �� �������� �������������� ������
	/* � ��� ���� data � data_size �� ������������. */
	case CG_MSG_OPEN:
		{
			return _this->func1(CG_MSG_OPEN, NULL, (IListener*)data);

			/*scheme_desc_t* scheme_desc = 0;
			CHECK_PLAZA2_FAIL(cg_lsn_getscheme(listener, &scheme_desc));
			assert(scheme_desc);

			message_desc_t* message_desc = scheme_desc->messages;
			for (size_t index = 0; message_desc; index ++, message_desc = message_desc->next)
			{
				field_desc_t* field_desc = message_desc->fields;
				while (field_desc)
				{

					field_desc = field_desc->next;
				}
			}*/			
		}
		//break;

	/* 	��������� �������� � ������ �������� ������ ������. ������ ��������� ��������, ���
	/*	����� ��� ������ ������������� ��� ��������. � ���� data ���������� ��������� �� int,
	/*	�� ���������� ������ �������� ���������� � ������� �������� ����������. */
	case CG_MSG_CLOSE:
		{
			return _this->func1(CG_MSG_CLOSE, message, (IListener*)data);

			// ������� �������� ������:
			// CG_REASON_UNDEFINED - �� ����������;
			// CG_REASON_USER - ������������ ������ ������ � callback ����������;
			// CG_REASON_ERROR - ���������� ������;
			// CG_REASON_DONE - ������ ����� cg_lsn_destroy;
			// CG_REASON_SNAPSHOT_DONE - ������� �������
		}
		//break;

	/* �������� ������ ������ ��������� ���������� ����� ������. � ���� �� ��������� ��-
	/* �������� ����� ���� ������������ ������� �� ��� �������� ����������� ������. ���-
	/* ��� ��������� �� �������� �������������� ������ � ��� ���� data � data_size �� ��-
	/* ����������. */
	case CG_MSG_TN_BEGIN:
		{
			return _this->func1(CG_MSG_TN_BEGIN, NULL, (IListener*)data);
		}
		//break;

	/* �������� ������ ���������� ��������� ���������� ����� ������. � ������� �������
	/* ����� ��������� ����� �������, ��� ������ ���������� �� ������ ��������, �������-
	/* �� � ���������������� ��������� � �������� ������� � ������������������ ����� ��-
	/* ��� ���������. ������ ��������� �� �������� �������������� ������ � ��� ���� data
	/* � data_size �� ������������. */
	case CG_MSG_TN_COMMIT:
		{
			return _this->func1(CG_MSG_TN_COMMIT, NULL, (IListener*)data);
		}
		//break;
	
	/* ��������� ������� ��������� ������. ���� data_size �������� ������ ���������� ���-
	/* ���, data ��������� �� ���� ������. ���� ��������� �������� �������������� ����,
	/* ������� ����������� ���������� cg_msg_streamdata_t. ��������� � ��������� ������
	/* ����� ���������� ���� � ������ �������. */
	case CG_MSG_STREAM_DATA:
		{
			//struct msg_streamdata_t* stream_msg = (struct msg_streamdata_t* )message;
			return _this->func2((msg_streamdata_t*)message, (IListener*)data);
		}
		//break;
		
	/* ������� ������ � ��������� online - ��� ��������, ��� ��������� ���������� ����� ����
	/* ��������� � ��������� ��������� CG_MSG_STREAM_DATA ����� ����� ������ ��-
	/* ����. ������ ��������� �� �������� �������������� ������ � ��� ���� data � data_size
	/* �� ������������. */
	case CG_MSG_P2REPL_ONLINE:
		{
			//msg_streamdata_t* info = (struct msg_streamdata_t* )data;
            log_info("%s_%d become to ONLINE sate. Message CG_MSG_P2REPL_ONLINE recieved", ((IListener*)data)->GetStreamName().c_str(), ((IListener*)data)->GetIsin());
			return _this->func1(CG_MSG_P2REPL_ONLINE, NULL, (IListener*)data);
		}
		//break;
		
	/* ������� ����� ����� �����. ����� ��������� ��������, ��� ���������� ������, ����-
	/* ������ �� ������, �� ��������� � ������ ���� �������. ��� ���� ��������� ���������
	/* ���������� ������ �� ������ ������ ����� ����� ������. ���� data ��������� �����-
	/* ���� �� ������������� ��������, ���������� ����� ����� ����� �����; ���� data_size
	/* �������� ������ �������������� ����. */
	case CG_MSG_P2REPL_LIFENUM:
		{
			return _this->func1(CG_MSG_P2REPL_LIFENUM, (struct msg_t*)message, (IListener*)data);
			//log_info("Life number is changed.\n");			
		}
		//break;

	/* ��������� �������� ��������� �������� ���������� ������. ���� data ��������� ���-
	/* ������ �� ��������� cg_data_cleardeleted_t, � ������� ������ ����� ������� � ����� ��-
	/* �����, �� ������� ������ � ��������� ������� ��������� ����������. ���� ������� �
	/* cg_data_cleardeleted_t == CG_MAX_REVISON, �� ����������� ������� ����������� � 1. */
	case CG_MSG_P2REPL_CLEARDELETED:
		{
			return _this->func1(CG_MSG_P2REPL_CLEARDELETED, (struct msg_t*)message, (IListener*)data);
		}
		//break;
		
	/* ��������� �������� ��������� ������ ������; ����������� ����� ��������� ������.
	/* ���� data ��������� ��������� �� ������, ������� � �������������� ���� �������� ��-
	/* ������� ������ ������ �� ������ ������� ��������� - ����������� ����� ������, ��-
	/* ���� ������� ������ � ����� ����� ����� �� ������ ���������� CG_MSG_TN_COMMIT
	/* (��������: ��� ������������ ������ � replstate �������, ���������� ����� ����������
	/* CG_MSG_TN_COMMIT, ����� �������� ��������!) ��� ������ ����� ���� �������� � ��-
	/* ��� cg_lsn_open � �������� ��������� "replstate" �� ����� �� ������ � ��������� ���, ���
	/* ��������� ����������� ��������� ������ � ������� ��������� ������. */
	case CG_MSG_P2REPL_REPLSTATE: 
		{
			return _this->func1(CG_MSG_P2REPL_REPLSTATE, (struct msg_t*)message, (IListener*)data);
		}
		//break;
	}

	return CG_ERR_OK;
}

P2Listener::P2Listener(conn_t** conn, const P2ListnerParameters& streamParams) : Listener(conn, streamParams)
{
}

P2Listener::~P2Listener()
{
}

void P2Listener::OpenListener()
{
    if (!plaza2Listener)
    {
        std::string url;
        switch(prefix)
        {
        case P2REPL:    url = "p2repl://"; break;
        case P2ORDBOOK: url = "p2ordbook://"; break;
        case P2MQREPLY: url = "p2mqreply://"; break;
        case P2SYS:     url = "p2sys://"; break;
        case P2EMB:     url = "p2emb://"; break;
        }
        url += stream_name + ';';
        if (!posfix.empty())
        {
            url += posfix + ';';
        }
        url += "name=" + stream_name + '_' + std::to_string(isin) + ';';
        if (!tables.empty())
        {
            url += tables;
        }

        log_info("Try to open listener for %s, call cg_lsn_new() ...", stream_name.c_str());
        CHECK_PLAZA2_FAIL(lsn_new(*plaza2ConnectionObject, url.c_str(), &MessageCallback, (Listener*)this, &plaza2Listener))
    }

    uint32_t state;
    CHECK_PLAZA2_FAIL(lsn_getstate(plaza2Listener, &state));
}
