// main C implementation file
// contains logic for SMA, EMA, RSI, Bollinger Bands
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

double *get_SMA(double *prices, int length, int window)
{
    /**
     * @brief Computes the Simple Moving Average (SMA) of a price series.
     *
     * This function calculates the SMA over a sliding window of the given size.
     * It returns a dynamically allocated array of SMA values, where each element
     * corresponds to the average of `window` consecutive prices.
     *
     * An SMA is a type of moving average (MA). Moving averages are calculated to identify the trend direction of a stock.
     * It is a trend-following or lagging indicator because it's based on past prices.
     * The longer the period for the moving average, the greater the lag.
     * Fifty-day and 200-day moving average figures are widely followed and considered to be important trading signals.
     * Shorter moving averages are typically used for short-term trading, while longer-term moving averages are more suited for long-term investors.
     *
     * @param prices Pointer to an array of double representing the price series.
     * @param length The total number of prices in the array.
     * @param window The size of the moving average window (number of periods).
     *
     * @return Pointer to a dynamically allocated array of doubles containing
     *         the SMA values. The length of this array is `length - window + 1`.
     *         Returns NULL if input parameters are invalid or if memory allocation fails.
     *
     * @note Caller is responsible for freeing the returned array.
     */

    if (!prices || !length || window <= 0)
    {
        return NULL;
    }
    if (window > length)
    {
        return NULL;
    }

    int result_length = length - window + 1;
    double *SMA_Values; // pointer to an array of doubles
    SMA_Values = malloc(sizeof(double) * result_length);
    if (!SMA_Values)
    {
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // populate array
    for (int i = 0; i < result_length; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < window; j++)
        {
            sum += prices[i + j];
        }
        SMA_Values[i] = sum / window;
    }
    return SMA_Values;
}

// exponential moving averages (EMAs)
double *get_EMA(double *prices, int length, int window)
{
    /**
     * @brief Computes the Exponential Moving Average (EMA) of a price series.
     *
     * This function calculates the EMA over a sliding window of the given size.
     * It returns a dynamically allocated array of EMA values, where each element
     * corresponds to the average of `window` consecutive prices.
     *
     * An EMA is a type of moving average (MA). Moving averages are calculated to identify the trend direction of a stock.
     * It is a trend-following or lagging indicator because it's based on past prices.
     * The longer the period for the moving average, the greater the lag.
     * Fifty-day and 200-day moving average figures are widely followed and considered to be important trading signals.
     * Shorter moving averages are typically used for short-term trading, while longer-term moving averages are more suited for long-term investors.
     *
     * The exponential moving average gives more weight to recent prices in an attempt to make them more responsive to new information.
     *
     * @param prices Pointer to an array of double representing the price series.
     * @param length The total number of prices in the array.
     * @param window The size of the moving average window (number of periods).
     *
     * @return Pointer to a dynamically allocated array of doubles containing
     *         the EMA values. The length of this array is `length - window + 1`.
     *         Returns NULL if input parameters are invalid or if memory allocation fails.
     *
     * @note Caller is responsible for freeing the returned array.
     */

    if (!prices || !length || !window)
    {
        return NULL;
    }
    if (window > length)
    {
        return NULL;
    }

    int result_length = length - window + 1;
    double alpha = 2.0 / ((double)window + 1.0);                 // smoothening multiplier
    double *EMA_Values = malloc(sizeof(double) * result_length); // pointer to an array of doubles
    double *SMA_Values = get_SMA(prices, length, window);        // EMA values are based on SMA values with a smoothener applied

    // data validation
    if (!EMA_Values)
    {
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (!SMA_Values)
    {
        fprintf(stderr, "Get SMA failed. %s.", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // populate array
    EMA_Values[0] = SMA_Values[0]; // first EMA value = seed EMA
    for (int i = 1; i < result_length; i++)
    {
        // EMA(current) = ( (Price(current) - EMA(prev) ) x Multiplier) + EMA(prev)
        EMA_Values[i] = ((prices[i + window - 1] - EMA_Values[i - 1]) * alpha) + EMA_Values[i - 1];
    }
    free(SMA_Values);
    return EMA_Values;
}

// RSI

// Bollinger Bands

// MACD

// On-Balance Volume (OBV)