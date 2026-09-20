#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "csvreader.c"
#include "money.h"
#include "gini.c"
#include "tax.c"

double b(double x, double (*taxFunc)(double))
{
    int households = TOTAL_HOUSEHOLDS * x;
    double totalCash = 0.0;

    for (int i = 0; i < BAND_COUNT; ++i)
    {
        if (households >= HOUSEHOLDS[i])
        {
            totalCash += (BANDS[i] - taxFunc(BANDS[i])) * HOUSEHOLDS[i];
            households -= HOUSEHOLDS[i];
        }
        else
        {
            totalCash += (BANDS[i] - taxFunc(BANDS[i])) * households;
            return totalCash / totalIncomeWith(taxFunc);
        }
    }

    return totalCash / totalIncomeWith(taxFunc);
}

double (*COMPILED_B_TAX_FUNC)(double);
double COMPILED_B_TOTAL_INCOME = 0.0;
double COMPILED_B_TAXED_BANDS[BAND_COUNT];

void compileb(double taxFunc(double))
{
    COMPILED_B_TAX_FUNC = taxFunc;
    COMPILED_B_TOTAL_INCOME = totalIncomeWith(COMPILED_B_TAX_FUNC);
    for (int i = 0; i < BAND_COUNT; ++i)
    {
        COMPILED_B_TAXED_BANDS[i] = BANDS[i] - COMPILED_B_TAX_FUNC(BANDS[i]);
    }
}

double bCompiled(double x)
{
    int households = TOTAL_HOUSEHOLDS * x;
    double totalCash = 0.0;
    for (int i = 0; i < BAND_COUNT; ++i)
    {
        if (households >= HOUSEHOLDS[i])
        {
            totalCash += COMPILED_B_TAXED_BANDS[i] * HOUSEHOLDS[i];
            households -= HOUSEHOLDS[i];
        }
        else
        {
            totalCash += COMPILED_B_TAXED_BANDS[i] * households;
            return totalCash / COMPILED_B_TOTAL_INCOME;
        }
    }

    return totalCash / COMPILED_B_TOTAL_INCOME;
}

double consumptionAt(double (*taxFunc)(double))
{
    return totalIncomeWith(taxFunc) * 0.96;
}
// "Spent not saved"

int main()
{
    clock_t time = clock();

    // TODO: Factor in the increase / decrease in consumption spending due to
    // people being taxed more/less. Then factor this in to your optimisation
    // calculation. Probably use some type of weighted distance formula.

    // To be fair, what my machine would optimally spit out (if I didn't weight
    // it towards any given system) is simply just tax everyone at 100%. Then
    // let the government decide what to do with 100% of its country's capital.
    // Essentially, my program turns communist.

    money_t MAXES[5] = {15600.0, 53500.0, 78100.0, 180000.0, 1e100};
    double RATES[5] = {0.105, 0.175, 0.30, 0.33, 0.39};
    compileBrackets(MAXES, RATES, 5);
    compileb(compiledPAYE);

    const double BASE_CONSUMPTION = consumptionAt(PAYE);
    double bestGini = Gini(bCompiled);
    double bestConsumption = consumptionAt(compiledPAYE);
    double bestRelativeConsumption = 1.0;
    double bestRates[5];
    double bestScore = -1.0;
    RATES[0] = 0.0;
    for (int a = 0; a <= 100; ++a)
    {
        RATES[1] = a * 0.01;
        for (int b = 0; b <= 100; ++b)
        {
            RATES[2] = b * 0.01;

            compileBrackets(MAXES, RATES, 5);
            compileb(compiledPAYE);

            double gini = Gini(bCompiled);
            double relativeConsumption = consumptionAt(compiledPAYE) / BASE_CONSUMPTION;

            double giniW = 1.0 - gini; // Weighted gini coefficient

            double score = giniW * giniW + (relativeConsumption - 0.5);
            if (relativeConsumption < 1.05)
            {
                score = 0.0;
            }
            if (gini > 0.38)
            {
                score = 0.0;
            }
            if (score > bestScore)
            {
                bestGini = gini;
                bestConsumption = consumptionAt(compiledPAYE);
                bestRelativeConsumption = relativeConsumption;
                bestScore = score;
                bestRates[0] = RATES[0];
                bestRates[1] = RATES[1];
                bestRates[2] = RATES[2];
                bestRates[3] = RATES[3];
                bestRates[4] = RATES[4];
            }
        }
    }


    printf(
        "Optimal Gini found as: %.1f\nConsumption spending of: $%.2f (%.1f%% of base)\nTax rates: {%.3f, %.3f, %.3f, %.3f, %.3f} (Score: %.3f)\n",
        bestGini * 100.0, bestConsumption, bestRelativeConsumption * 100.0, bestRates[0], bestRates[1], bestRates[2], bestRates[3], bestRates[4], bestScore
        );

    time = clock() - time;

    double elapsed = time;
    elapsed /= CLOCKS_PER_SEC;
    printf("Op Time: %.3fs\n", elapsed);


    return 0;
}