#include "money.h"
// For calculations of the Gini coefficient
//
// Gini = A / (A + B) = 2A = 1 - 2B
// Implicitly, a(x) = x, therefore:
//      ∫ 1
// A =  |   a(x)dx      = 0.5
//      ∫ 0
// Additionally, for some b(x):
//      ∫ 1
// B =  |   b(x)dx
//      ∫ 0

// Calculates B, see above comment, accurate to 3dp
double B(double b(double))
{
    const double DX = 1.0e-4;
    double total = 0.0;
    for (double x = 0.0; x <= 1.0; x += DX)
    {
        total += b(x);
    }
    return total * DX;
}

double totalIncomeWith(double (*taxFunc)(double))
{
    double total = 0.0;
    for (int i = 0; i < BAND_COUNT; ++i)
    {
        total += HOUSEHOLDS[i] * (BANDS[i] - taxFunc(BANDS[i]));
    }
    return total;
}

// Calculates A, which is 0.5 - B, accurate to 3dp
double A(double b(double))
{
    return 0.5 - B(b);
}

// Calculates the Gini coefficient for the given b function, accurate to 3dp
double Gini(double b(double))
{
    return 1.0 - 2.0 * B(b);
}