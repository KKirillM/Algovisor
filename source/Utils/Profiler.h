#pragma once

// Stop watch class �� ����� ������� ��� ������ ������� ���������� ������� ����
// QueryPerformanceFrequency ���������� ���������� ������ �� ���������������� ������� � �������
// QueryPerformanceCounter ���������� ������� �������� �������� ������ � ������� ��������� �����
class CStopwatch 
{
public:
    CStopwatch() { /*QueryPerformanceFrequency(&m_liPerfFreq);*/ Start(); }

    void Start() { QueryPerformanceCounter(&m_liPerfStart); }

    // Returns # of milliseconds since Start was called
    __int64 Now() const 
    {   
        LARGE_INTEGER liPerfNow;
        QueryPerformanceCounter(&liPerfNow);
        return (liPerfNow.QuadPart - m_liPerfStart.QuadPart);	//����� � ������
        //(((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);	//����� � ������������
    }

    __int64 Now(char* s) const; 

private:
    //LARGE_INTEGER m_liPerfFreq;   // Counts per second
    LARGE_INTEGER m_liPerfStart;  // Starting count
};
