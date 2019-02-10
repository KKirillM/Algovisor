#include "stdafx.h"
#include "Price.h"


Price::Price(double price, int precision)
{
    SetPrice(price, precision);
}

void Price::SetPrice(double price, int precision)
{
    _integer = (int)abs(price);

    if (price < 0)
        minus = true;
    else minus = false;

    if (price > 0)
        price -= _integer;
    else if (!_integer)
        price = abs(price);
    else price = abs(price) - _integer;

    for (int k = precision; k > 0; k--)
        price *= 10;

    _fraction = (int)(price+0.4);
    _precision = precision;
}

double Price::ToDouble() const
{
    double res = _fraction;

    for (int k = _precision; res != 0 && k > 0; k--)
        res /= 10;
    
    if (minus)
        res *= -1;
        
    res += _integer;

    return res;
}