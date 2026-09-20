#include "money.h"

// Function with no tax rate
money_t NO_TAX_FUNC(money_t amount)
{
    return 0.0;
}

// Difference between brackets for PAYE (nobody is earning a googol dollars p.a)
const money_t PAYE_BRACKETS[5] = {15600.0, 37900.0, 24600.0, 101900.0, 1e100};
// Tax rates paid at each bracket for PAYE
const double PAYE_RATES[5]     = {0.105, 0.175, 0.30, 0.33, 0.39};

// PAYE tax brackets
// $0 - $15,600 : 10.5%
// $15,601 - $53,500 : 17.5%
// $53,501 - $78,100 : 30.0%
// $78,101 - $180,000 : 33.0%
// $180,001+ : 39.0%
// Calculates the amount of tax the given income would pay, using the standard
// PAYE tax brackets (see above)
money_t PAYE(money_t amount)
{
    money_t tax = 0.0;
    for (int i = 0; i < 5; ++i)
    {
        if (amount >= PAYE_BRACKETS[i])
        {
            tax += PAYE_BRACKETS[i] * PAYE_RATES[i];
            amount -= PAYE_BRACKETS[i];
        }
        else
        {
            tax += amount * PAYE_RATES[i];
            return tax;
        }
    }

    return tax;
}

// Calculates the amount of tax the given income would pay, using the given tax
// brackets. Supply this the form as described below:
// $0 - $15,600, $15,601 - $53,500, $53,501 - $78,100, $78,101+
// would be: {15,600.0, 53,500.0, 78,100.0, 1,000,000,000,000,000.0}
// The rates would then be along the matching index, use a really high number
// as the largest tax bracket
money_t customPAYE(money_t amount, const money_t MAXES[], const double RATES[], int bracketCount)
{
    money_t BRACKETS[bracketCount];

    money_t tax = 0.0;
    for (int i = 0; i < bracketCount; ++i)
    {
        if (i == 0) BRACKETS[0] = MAXES[0];
        else BRACKETS[i] = MAXES[i] - MAXES[i - 1];

        if (amount >= BRACKETS[i])
        {
            tax += BRACKETS[i] * RATES[i];
            amount -= BRACKETS[i];
        }
        else
        {
            tax += amount * RATES[i];
            return tax;
        }
    }

    return tax;
}

money_t* COMPILED_PAYE_BRACKETS = (money_t *) -1;
double* COMPILED_PAYE_RATES = (double *) -1;
int COMPILED_PAYE_BRACKET_COUNT = -1;
// Compiles a new set of tax brackets for compiledPAYE, this is usually faster
// than customPAYE. This should only be run once before running many
// compiledPAYE calculations
void compileBrackets(const money_t MAXES[], const double RATES[], int bracketCount)
{
    if (COMPILED_PAYE_BRACKET_COUNT > 0)
    {
        free(COMPILED_PAYE_BRACKETS);
        free(COMPILED_PAYE_RATES);
    }
    COMPILED_PAYE_BRACKET_COUNT = bracketCount;
    COMPILED_PAYE_BRACKETS = malloc(sizeof(money_t) * COMPILED_PAYE_BRACKET_COUNT);
    COMPILED_PAYE_RATES = malloc(sizeof(double) * COMPILED_PAYE_BRACKET_COUNT);
    for (int i = 0; i < COMPILED_PAYE_BRACKET_COUNT; ++i)
    {
        if (i == 0) COMPILED_PAYE_BRACKETS[0] = MAXES[0];
        else COMPILED_PAYE_BRACKETS[i] = MAXES[i] - MAXES[i - 1];
        COMPILED_PAYE_RATES[i] = RATES[i];
    }
}

// Returns the tax amount with the compiled PAYE brackets, this unsafely
// assumes you have already set the compiled brackets, so don't be stupid! This
// is generally the fastest way of computing large amounts of taxes, just don't
// recompile before every calculation
money_t compiledPAYE(money_t amount)
{
    money_t tax = 0.0;
    for (int i = 0; i < COMPILED_PAYE_BRACKET_COUNT; ++i)
    {
        if (amount >= COMPILED_PAYE_BRACKETS[i])
        {
            tax += COMPILED_PAYE_BRACKETS[i] * COMPILED_PAYE_RATES[i];
            amount -= COMPILED_PAYE_BRACKETS[i];
        }
        else
        {
            tax += amount * COMPILED_PAYE_RATES[i];
            return tax;
        }
    }

    return tax;
}

// Runs a lot of tax calculations
void runTax()
{
}