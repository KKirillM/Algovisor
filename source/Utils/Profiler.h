#pragma once

// Stop watch class из книги Рихтера для оценки времени выполнения участка кода
// QueryPerformanceFrequency возвращает количество тактов от высокочастотного таймера в секунду
// QueryPerformanceCounter возвращает текущее значение счетчика тактов с момента включения компа
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
        return (liPerfNow.QuadPart - m_liPerfStart.QuadPart);	//время в тактах
        //(((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);	//время в миллискундах
    }

    __int64 Now(char* s) const; 

private:
    //LARGE_INTEGER m_liPerfFreq;   // Counts per second
    LARGE_INTEGER m_liPerfStart;  // Starting count
};
