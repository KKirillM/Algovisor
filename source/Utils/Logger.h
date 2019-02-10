//----------------------------------------------------------------------------
// Copyright: (c) 2016 Algovisor, Russia
//----------------------------------------------------------------------------
// Created:	    23 Dec 2016   19:22
// File:        logger
// Author:		Kirill_Korobitsyn
//----------------------------------------------------------------------------
// Description: ������ ��� ����������� � ������� ��� ����
//---------------------------------------------------------------------------- 

#pragma once
#include <fstream>
#include <iostream>
#include <stdarg.h>
#include "lock.h"

#define LOGGER_PARAM_DATE       0x01    // ���������� � ������ ������ ������� ����
#define LOGGER_PARAM_TIME       0x02    // ���������� � ������ ������ ������� �����
#define LOGGER_PARAM_TYPE       0x04    // ���������� � ������ ������ ������� ��� ��������� (��. LoggerMessageType)
#define LOGGER_PARAM_MODULE     0x08    // ���������� � ������ ������ ��� ������ �� �������� ���� ����������� �����������
#define LOGGER_PARAM_TID        0x10    // ���������� � ������ ������ ID �������� ������
#define LOGGER_PARAM_ALL        0x1F

enum LoggerMessageType
{
    LOG_TRACE,  // ����� �������� ������
    LOG_DEBUG,  // ����� ������ ��� ���������� ������
    LOG_ERROR   // ����� �������������� �������� ERROR
};

template <typename StreamType>
class Logger
{
public:
    Logger(int params = 0) : stream(NULL)
    {
        InitParams("", params);
    }
    virtual ~Logger() {}
    
    void InitParams(const std::string& _moduleName, int _params = LOGGER_PARAM_ALL) { params = _params; moduleName = _moduleName; }
    virtual void print(LoggerMessageType type, const char* message, ...) = 0;

protected:
    void printString(LoggerMessageType type, const char* message, const va_list& args);

protected:
    StreamType* stream;
    int params;
    std::string moduleName;

};  // Logger

template <typename StreamType>
void Logger<StreamType>::printString(LoggerMessageType type, const char* message, const va_list& args)
{
    if (!stream)
    {
        throw std::exception("Logger stream is not initialized");
    }

    std::string outputString;

    ::SYSTEMTIME systemtime;
    ::GetLocalTime(&systemtime);

    if (params & LOGGER_PARAM_DATE)
    {
        char date[32];
        sprintf_s(date, "%02d-%02d-%d ", systemtime.wDay, systemtime.wMonth, systemtime.wYear);
        outputString += date;
    }

    if (params & LOGGER_PARAM_TIME)
    {
        char time[64];
        sprintf_s(time, "%02d:%02d:%02d.%03d;\t", systemtime.wHour, systemtime.wMinute, systemtime.wSecond, systemtime.wMilliseconds);
        outputString +=  time ;
    }

    if (params & LOGGER_PARAM_TYPE)
    {
        switch (type)
        {
        case LOG_TRACE: outputString += "; "; break;
        case LOG_DEBUG: outputString += "DEBUG; "; break;            
        case LOG_ERROR: outputString += "ERROR; "; break;
        }
    }

    if (params & LOGGER_PARAM_MODULE)
    {
        outputString += moduleName + "; ";
    }

    char buffer[1024*100]; //  10 ����� ���� � �������
    vsnprintf_s(buffer, sizeof(buffer), message, args);
    outputString += buffer;

    if (params & LOGGER_PARAM_TID)
    {
        outputString += "; TID " + std::to_string(::GetCurrentThreadId());
    }

    stream->write(outputString.c_str(), outputString.length());
    stream->operator<<(std::endl);
}

// ����� ��� ����������� � �������, ��������� ������������� �������������
class ConsoleLogger : public Logger<std::ostream>
{
public:
    ConsoleLogger(int params = 0) : Logger(params) { stream = &std::cout; }
    virtual ~ConsoleLogger() {}
    void print(LoggerMessageType type, const std::string& moduleName, const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        printString(type, message, args);
        va_end(args);
    }

};  // ConsoleLogger

// ����� ��� ������������ ������ ����� � ����
class FileLogger : public Logger<std::ofstream>
{
public:
    /*  std::ios_base::openmode values:
        in	    input	    File open for reading: the internal stream buffer supports input operations.
        out 	output	    File open for writing: the internal stream buffer supports output operations.
        binary	binary	    Operations are performed in binary mode rather than text.
        ate	    at end	    ��������� ������������ � ����� �����. ������ ������������ � ������� ������� (������������ �����) �����;
        app	    append	    ��� ������ ������ ����������� � ����� �����, ���� ���� ������� ������� ���� ����� ���� ��������  �������� ostream::seekp;
        trunc	truncate	���� ���� ��� ����������, ��� ���������� ������������ (����� ����� ����). ���� ����� ��������� �� ���������, ���� ios::out ����������, � ios::ate, ios::app ��� ios:in �� �����������;
    */
    FileLogger(const std::string& path, const std::string& name, const std::ios_base::openmode mode = std::ios_base::ate, int params = 0) : Logger(params)
    {
        assert(path != "" && name != "");
        std::string fullName = path+'\\'+name;

        std::string onlyPath(fullName.begin(), fullName.begin() + fullName.rfind('\\'));
        ::CreateDirectoryA(onlyPath.c_str(), NULL);

        stream = new std::ofstream(fullName, mode);

        if (!stream->is_open())
        {
            throw std::exception(("Can't open log-file: " + fullName).c_str());
        }
    }

    virtual ~FileLogger()
    {
        stream->close();
        delete stream;
    }

    void print(LoggerMessageType type, const char* message, ...)
    {
        va_list args;
        va_start(args, message);
        printString(type, message, args);
        va_end(args);
        stream->flush();
    }

};  // FileLogger

// ����� ��� ������������� ������ ����� � ����
class MT_FileLogger : public FileLogger
{
public:
    MT_FileLogger(const std::string& path, const std::string& name, const std::ios_base::openmode mode = std::ios_base::ate, int params = 0) : FileLogger(path, name, mode, params)
    {
    }

    virtual ~MT_FileLogger() {}

    void print(LoggerMessageType type, const char* message, ...)
    {
        locker.Lock();
        va_list args;
        va_start(args, message);
        printString(type, message, args);
        va_end(args);
        stream->flush();
        locker.Unlock();
    }    

private:
    CCriticalSectionLock locker;

};  // MT_FileLogger

#define PRINT_TO_LOG(logger, type, message, ...) logger.print(type, message, __VA_ARGS__);

// ���� ��� ���������� ����� ����������: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

