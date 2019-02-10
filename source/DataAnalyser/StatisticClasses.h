#pragma once
#include "TradeSessionData.h"

// разница значений элементов начала и конца окна
template <typename T>
class DeltaValues
{
	std::deque<T> values;
	size_t windowRange;

public:
	DeltaValues(const size_t range) : windowRange(range) {}
	void AddValue(const T value)
	{
		values.push_back(value);
		if (values.size() > windowRange)
		{
			values.pop_front();
		}
	}
	T GetDelta() 
	{ 
		return (values.size() > 1) ? (values.back() - values.front()) : 0;
	}
};

template <typename T>
class TimeRangeDeltaValues
{
	std::deque<T> values;
	const fo2base::UnixNanosecondsTime delay;			// временной диапазон между первым и последним трейдом в пакете
	std::deque<fo2base::UnixNanosecondsTime> times;		// все значения времени трейдов в пакете

public:
	TimeRangeDeltaValues(const fo2base::UnixNanosecondsTime timeDelay) : delay(timeDelay) {}
	void AddValue(const T value, const fo2base::UnixNanosecondsTime time)
	{
		values.push_back(value);
		times.push_back(time);
		while (times.back() - times.front() > delay)
		{
			values.pop_front();
			times.pop_front();
		}
	}
	T GetDelta() const
	{
		return (values.size() > 1) ? (values.back() - values.front()) : 0;
	}
	T GetLogRatio() const
	{
		return (values.size() > 1) ? std::log(values.back() / values.front()) : 0;
	}

};

// индикатор расчёта волатильности для заданного промежутка времени
// на вход принимает 2 параметра: количество временных интервалов и длительность временного интервала
template <typename T>
class TimeVolatility
{
	size_t rangeCount;
	std::deque<T> values;
	const fo2base::UnixNanosecondsTime delay;			// временной диапазон между первым и последним трейдом в пакете
	std::deque<fo2base::UnixNanosecondsTime> times;		// все значения времени трейдов в пакете

	T sum;
	std::deque<T> volatValues;

public:
	TimeVolatility(const int count, const fo2base::UnixNanosecondsTime timeDelay) : rangeCount(count), delay(timeDelay), sum(0) {}
	void AddValue(const T value, const fo2base::UnixNanosecondsTime time)
	{
		if (!times.empty() && time - times.front() > delay)
		{
			T val = std::abs(values.back() - values.front());
			sum += val;
			volatValues.push_back(val);
			if (volatValues.size() > rangeCount)
			{
				sum -= volatValues.front();
				volatValues.pop_front();
			}
			values.clear();
			times.clear();
		}
		values.push_back(value);
		times.push_back(time);
	}
	T GetVolat() const
	{
		return (!volatValues.empty()) ? (sum / volatValues.size()) : 0;
	}
	bool isTimeRangeFull() const { return volatValues.size() == rangeCount; }

};	// TimeVolatility

// простая скользящая средняя
// максимальное кол-во элементов задаётся в конструкторе
template <typename T>
class SimpleMovingAvarage
{
	T sum;
	std::deque<T> values;
	size_t rangeCount;

public:
	SimpleMovingAvarage(const size_t range) : rangeCount(range), sum(0) {}
	void AddValue(const T value)
	{
		sum += value;
		if (std::isnan(sum))
		{
			sum = 0;
		}
		values.push_back(value); 
		if (values.size() > rangeCount)
		{
			sum -= values.front();
			values.pop_front();
		}
	}
	T CalcAverage() const 
	{ 
		return (values.size() > 0) ? sum / (T)values.size() : 0;
	}
	bool isRangeFull() const { return values.size() == rangeCount; }

};	// MovingAvarage

// временная скользящая средняя
// максимальный интервал времени задаётся в конструкторе
// предполагается, что вместе с каждым элементом последовательности передётся значение времени, по которому происходит отсев
template <typename T>
class TimeMovingAvarage
{
	T sum;
	std::deque<T> values;
	const fo2base::UnixNanosecondsTime delay;					// временной диапазон между первым и последним трейдом в пакете
	std::deque<fo2base::UnixNanosecondsTime> times;		// все значения времени трейдов в пакете

public:
	TimeMovingAvarage(const fo2base::UnixNanosecondsTime timeDelay) : delay(timeDelay), sum(0) {}
	void AddValue(const T value, const fo2base::UnixNanosecondsTime time)
	{
		sum += value;
		if (std::isnan(sum))
		{
			sum = 0;
		}
		values.push_back(value);
		times.push_back(time);
		while (times.back() - times.front() > delay)
		{
			sum -= values.front();
			values.pop_front();
			times.pop_front();
		}
	}
	T CalcAverage() const
	{
		return (values.size() > 0) ? sum / (T)values.size() : 0;
	}

};	// TimeMovingAvarage

// класс для вычисляения экспоненциальной скользящей средней
template <typename T>
class ExpMovingAvarage
{
	T expMean;
	size_t rangeCount;
	SimpleMovingAvarage<T> mean;

public:
	ExpMovingAvarage(const size_t range) : rangeCount(range), expMean(0), mean(range) {}
	void AddValue(const T value)
	{
		if (mean.isRangeFull())
		{
			const double alfa = (2.0 / (rangeCount + 1));
			expMean = alfa * value + (1 - alfa) * expMean;
			if (std::isnan(expMean))
			{
				expMean = 0;
			}
		}
		else
		{
			mean.AddValue(value);
			expMean = mean.CalcAverage();
		}
	}
	T GetAvg() const
	{
		return expMean;
	}
	
};	// ExpMovingAvarage

// класс для вычисления величины стандартного отклонения 
// количество значений в ряде ограничено максимальным значением maxCount
template <typename T>
class Sigma
{
	T sum;
	T squareSum;
	std::deque<double> values;
	std::deque<double> squareValues;
	size_t maxCount;

public:
	Sigma(const size_t max) : maxCount(max), squareSum(0), sum(0) {}
	void AddValue(const T value)
	{
		sum += value;
		const double squareVal = std::pow(value, 2);
		squareSum += (T)squareVal;
		values.push_back(value);
		squareValues.push_back(squareVal);
		if (values.size() > maxCount)
		{
			squareSum -= (T)squareValues.front();
			squareValues.pop_front();
			sum -= (T)values.front();
			values.pop_front();
		}
	}
	T Calc() const 
	{ 
		return (T)std::sqrt(squareSum / values.size() - std::pow(sum / values.size(), 2));
	}

	bool isMaxElementsCount() const { return values.size() == maxCount; }

};	// StdDev

// класс для вычисления величины стандартного отклонения 
// количество значений в ряде ограничено максимальным значением maxCount и временным диапазоном между первым и последним значениями в ряде
template <typename T>
class TimeRangeSigma
{
	T sum;
	T squareSum;
	std::deque<double> values;
	std::deque<double> squareValues;
	size_t maxCount;
	const fo2base::UnixNanosecondsTime delay;					// временной диапазон между первым и последним трейдом в пакете
	std::deque<fo2base::UnixNanosecondsTime> times;		// все значения времени трейдов в пакете

public:
	TimeRangeSigma(const size_t max, const fo2base::UnixNanosecondsTime timeDelay) : maxCount(max), delay(timeDelay), squareSum(0), sum(0) {}
	void AddValue(const T value, const fo2base::UnixNanosecondsTime time)
	{
		sum += value;
		const double squareVal = std::pow(value, 2);
		squareSum += (T)squareVal;
		values.push_back(value);
		squareValues.push_back(squareVal);
		times.push_back(time);
		while (times.back() - times.front() > delay || values.size() > maxCount)
		{
			squareSum -= (T)squareValues.front();
			squareValues.pop_front();
			sum -= (T)values.front();
			values.pop_front();
			times.pop_front();
		}
	}
	T Calc() const
	{
		return (T)std::sqrt(squareSum / values.size() - std::pow(sum / values.size(), 2));
	}

	bool isMaxElementsCount() const { return values.size() == maxCount; }

};	// StdDev

// классический VPIN индикатор, строится на базе объёмов ордеров
class VPIN_Indicator
{
	const unsigned int VMax;				// максимальное значение объёма в пакете
	unsigned int V;							// текущий объём пакета
	unsigned int Vs;						// текущий объем на продажу в пакете
	unsigned int Vb;						// текущий объем на покупку в пакете
	std::deque<int> volumeValues;			// все значения объёмов в пакете

public:
	VPIN_Indicator(const unsigned int maxVol) : VMax(maxVol), V(0), Vs(0), Vb(0) {}

	void AddValue(const int vol)
	{
		V += std::abs(vol);

		if (vol > 0) // buy
		{
			Vb += std::abs(vol);
		}
		else		// sell
		{
			Vs += std::abs(vol);
		}

		volumeValues.push_back(vol);

		while (V > VMax)
		{
			int& vol = volumeValues.front();
			const unsigned int V0 = V - std::abs(vol);
			if (V0 <= VMax)
			{
				const int delta = V - VMax;
				if (vol > 0) // buy
				{
					vol -= delta;
					Vb -= delta;
				}
				else		// sell
				{
					vol += (V - VMax);
					Vs -= delta;
				}
				V -= delta;
			}
			else
			{
				if (vol > 0) // buy
				{
					Vb -= vol;
				}
				else		// sell
				{
					Vs += vol;
				}
				V = V0;
				volumeValues.pop_front();
			}
		}			
	}

	double CalcVPIN() const 
	{ 
		return (V == VMax) ? ((double(Vb) - Vs) / V) : 0.0; 
	}

};	// VPIN_Indicator

// VPIN на базе количества ордеров поглощённых трейдом
class VPIN_Indicator_v1
{
	const unsigned int maxCount;		// кол-во трейдов, которое содержится в окне вычисления
	unsigned int Vs;					// текущее кол-во ордеров на продажу в пакете
	unsigned int Vb;					// текущее кол-во ордеров на покупку в пакете
	unsigned int V;						// общее кол-во поглощённых ордеров в пакете
	std::deque<int> values;				// все значения количеств поглощённых ордеров со знаком, также может содержать нули, 
										// если трейд не смог поглотить ни одного ордера.
	fo2base::UnixNanosecondsTime timeWindow;	// временной диапазон между первым и последним трейдом в пакете
	std::deque<fo2base::UnixNanosecondsTime> times;	// все значения времени трейдов в пакете

public:
	VPIN_Indicator_v1(const unsigned int count, const fo2base::UnixNanosecondsTime& delay = UINT64_MAX) : maxCount(count), timeWindow(delay), Vs(0), Vb(0), V(0) {}

	void AddValue(const int vol, const fo2base::UnixNanosecondsTime time)
	{
		const unsigned int module_vol = std::abs(vol);
		V += module_vol;

		if (vol > 0)		// buy
		{
			Vb += module_vol;
		}
		else if (vol < 0)	// sell
		{
			Vs += module_vol;
		}

		values.push_back(vol);
		times.push_back(time);

		while (times.back() - times.front() > timeWindow || values.size() > maxCount)
		{
			const int sign_vol = values.front();
			const unsigned int unsigned_vol = std::abs(sign_vol);
			if (sign_vol > 0)
			{
				Vb -= unsigned_vol;
			}
			else if (sign_vol < 0)
			{
				Vs -= unsigned_vol;
			}
			V -= unsigned_vol;
			values.pop_front();
			times.pop_front();
		}
	}

	double CalcVPIN() const 
	{ 
		return (!values.empty()) ? ((double(Vb) - Vs) / V) : 0.0; 
	}

	bool isMaxElementsCount() const { return values.size() == maxCount; }

};	// VPIN_Indicator_v1

// VPIN на базе количества поглощённых ордеров трейдами с учётом открытого интереса
class VPIN_Indicator_v2
{
	const unsigned int maxCount;		// кол-во трейдов
	const InterestDirection dir;		// направление открытого интереса, которое отслеживает текущий индикатор
	unsigned int Vs;					// текущее количество поглащённых ордеров на продажу в пакете с открытым интересом в покупку/продажу
	unsigned int Vb;					// текущее количество поглащённых ордеров на покупку в пакете с открытым интересом в покупку/продажу
	unsigned int Vu;					// текущее количество поглащённых ордеров на покупку и на продажу с неопределённым открытым интересом UNKNOWN_POSITION 
	unsigned int V;						// общее кол-во поглощённых ордеров в пакете
	std::deque<std::pair<int, InterestDirection>> values;				// все значения поглощённых ордеров со знаком и направление изменения ОИ

public:
	VPIN_Indicator_v2(const unsigned int count, const InterestDirection OI_DIR) : maxCount(count), dir(OI_DIR), Vs(0), Vb(0), Vu(0), V(0) {}

	void AddValue(const int vol, const InterestDirection OI_DIR)
	{
		const unsigned int module_vol = std::abs(vol);
		V += module_vol;

		if (vol > 0 && dir == OI_DIR)		// buy
		{
			Vb += module_vol;
		}
		else if (vol < 0 && dir == OI_DIR)	// sell
		{
			Vs += module_vol;
		}
		else if (OI_DIR == UNKNOWN_POSITION)
		{
			Vu += module_vol;
		}

		values.push_back(std::make_pair(vol, OI_DIR));

		if (values.size() > maxCount)
		{
			const std::pair<int, InterestDirection> sign_vol = values.front();
			const unsigned int unsigned_vol = std::abs(sign_vol.first);
			if (sign_vol.first > 0 && dir == sign_vol.second)
			{
				Vb -= unsigned_vol;
			}
			else if (sign_vol.first < 0 && dir == sign_vol.second)
			{
				Vs -= unsigned_vol;
			}
			else if (sign_vol.second == UNKNOWN_POSITION)
			{
				Vu -= unsigned_vol;
			}
			V -= unsigned_vol;
			values.pop_front();
		}
	}

	double CalcVPIN() const 
	{ 
		return (values.size() == maxCount) ? ((double(Vb) - Vs) / (V - Vu)) : 0.0;
	}

};	// VPIN_Indicator_v2

// Индикатор измеряющий направление движения спрэда или силу добавления/снятия ордеров на спрэде в определённом окне.
// Каждому добавленномн/снятому ордеру присваиваются свои веса по принципу:
//	- добавление ордера на бид +1
//	- снятие ордера с асков +1
//	- добавление ордера на аск -1
//  - снятие оредра с бида -1
class SpreadEventRatio
{
	const unsigned int maxCount;					// кол-во событий, которое содержится в окне вычисления
	unsigned int V_long;							// суммарное кол-во ордеров добавленных на первый уровень бидов или убранных с первого уровня асков
	unsigned int V_short;							// суммарное кол-во ордеров добавленных на первый уровень асков или убранных с первого уровня бидов
	std::deque<char> values;						// содержит общее кол-во добавленных или убранных ордеров на первом уровне
	fo2base::UnixNanosecondsTime timeWindow;		// временной диапазон между первым и последним событием в пакете
	std::deque<fo2base::UnixNanosecondsTime> times;	// все значения времени событий с ордерами в пакете

public:
	SpreadEventRatio(const unsigned int count, const fo2base::UnixNanosecondsTime& delay = UINT64_MAX) : maxCount(count), timeWindow(delay), V_long(0), V_short(0) {}

	void AddValue(const char value, const fo2base::UnixNanosecondsTime time)
	{
		const unsigned char unsig_value = std::abs(value);

		if (value > 0)		// buy
		{
			V_long += unsig_value;
		}
		else /*if (value < 0)*/	// sell
		{
			V_short += unsig_value;
		}

		values.push_back(value);
		times.push_back(time);

		while (times.back() - times.front() > timeWindow || values.size() > maxCount)
		{
			const char sign_value = values.front();
			const unsigned int unsigned_value = std::abs(sign_value);
			if (sign_value > 0)
			{
				V_long -= unsigned_value;
			}
			else /*if (sign_value < 0)*/
			{
				V_short -= unsigned_value;
			}
			values.pop_front();
			times.pop_front();
		}
	}

	double Calc() const
	{
		return (!values.empty()) ? ((double(V_long) - V_short) / values.size()) : 0.0;
	}

	bool isMaxElementsCount() const { return values.size() == maxCount; }

};	// VPIN_Indicator_v1

// индикатор расчёта корреляции в движущемся окне значений по методу Пирсона
// ограничением количества элементов в окне явлется величина maxCount
template <typename T>
class Correlation
{
	T sum1;
	T sum2;
	std::deque<T> values1;
	std::deque<T> values2;
	size_t maxCount;

public:
	Correlation(const size_t range) : maxCount(range), sum1(0), sum2(0) {}
	void AddValues(const T value1, const T value2)
	{
		sum1 += value1;
		sum2 += value2;
		if (std::isnan(sum1))
		{
			sum1 = 0;
		}
		if (std::isnan(sum2))
		{
			sum2 = 0;
		}
		values1.push_back(value1);
		values2.push_back(value2);
		if (values1.size() > maxCount)
		{
			sum1 -= values1.front();
			values1.pop_front();
			sum2 -= values2.front();
			values2.pop_front();
		}
	}
	T Calc() const
	{
		if (!isFullRange())
		{
			return 0;
		}

		T avg1 = sum1 / (T)values1.size();
		T avg2 = sum2 / (T)values2.size();

		T sumMul = 0;
		T sumPowX = 0;
		T sumPowY = 0;
		for (int i = 0; i < maxCount; ++i)
		{
			T X = avg1 - values1[i];
			T Y = avg2 - values2[i];
			sumPowX += std::pow(X, 2.0);
			sumPowY += std::pow(Y, 2.0);
			sumMul += X * Y;
		}

		return sumMul / std::sqrt(sumPowX * sumPowY);
	}
	bool isFullRange() const { return values1.size() == maxCount; }

};	// Correlation

// индикатор расчёта корреляции в движущемся окне значений по методу Пирсона
// ограничением количества элементов в окне явлется величина maxCount и временным диапазоном между первым и последним значениями в ряде
template <typename T>
class TimeRangeCorrelation
{
	T sum1;
	T sum2;
	std::deque<T> values1;
	std::deque<T> values2;
	size_t maxCount;
	const fo2base::UnixNanosecondsTime delay;					// временной диапазон между первым и последним трейдом в пакете
	std::deque<fo2base::UnixNanosecondsTime> times;		// все значения времени трейдов в пакете

public:
	TimeRangeCorrelation(const size_t range, const fo2base::UnixNanosecondsTime timeDelay) : maxCount(range), delay(timeDelay), sum1(0), sum2(0) {}
	void AddValues(const T value1, const T value2, const fo2base::UnixNanosecondsTime time)
	{
		sum1 += value1;
		sum2 += value2;
		if (std::isnan(sum1))
		{
			sum1 = 0;
		}
		if (std::isnan(sum2))
		{
			sum2 = 0;
		}
		values1.push_back(value1);
		values2.push_back(value2);
		times.push_back(time);
		while (times.back() - times.front() > delay || values1.size() > maxCount)
		{
			sum1 -= values1.front();
			values1.pop_front();
			sum2 -= values2.front();
			values2.pop_front();
			times.pop_front();
		}
	}
	T Calc() const
	{
		if (!isFullRange())
		{
			return 0;
		}

		T avg1 = sum1 / (T)values1.size();
		T avg2 = sum2 / (T)values2.size();

		T sumMul = 0;
		T sumPowX = 0;
		T sumPowY = 0;
		for (int i = 0; i < values1.size(); ++i)
		{
			T X = avg1 - values1[i];
			T Y = avg2 - values2[i];
			sumPowX += std::pow(X, 2.0);
			sumPowY += std::pow(Y, 2.0);
			sumMul += X * Y;
		}

		return sumMul / std::sqrt(sumPowX * sumPowY);
	}
	bool isFullRange() const { return values1.size() == maxCount; }

};	// Correlation
