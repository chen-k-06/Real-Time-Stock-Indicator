/**
 * indicators.c
 * ------------
 * Implements financial technical indicators in C for use in a high-performance stock analysis API.
 *
 * Indicators included:
 *  - Simple Moving Average (SMA
 *  - Exponential Moving Average (EMA)
 *  - Relative Strength Index (RSI)
 *  - Bollinger Bands
 *
 * These functions are optimized for speed and are intended to be called from a Python FastAPI server
 * using ctypes or cffi.
 *
 * Author: Kexin Chen
 * Date: June 20th 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

double *compute_SMA(double *prices, int length, int window)
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
    if (window >= length)
    {
        return NULL;
    }

    int result_length = length - window + 1;
    double *SMA_Values = malloc(sizeof(double) * result_length); // pointer to an array of doubles
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

double *compute_EMA(double *prices, int length, int window)
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
    if (window >= length)
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

double *compute_RSI(double *prices, int length, int window)
{
    /**
     * @brief Computes the Relative Strength Index (RSI) of a price series.
     *
     * This function calculates the RSI over a sliding window of the given size.
     * It returns a dynamically allocated array of RSI values, where each element
     * corresponds to the average of `window` consecutive prices.
     *
     * RSI is a momentum indicator, used to determine the strength or weakness of a stock's price trend.
     * Momentum is the rate at which a stock's price changes.
     * RSI measures the speed and magnitude of a security's recent price changes to detect overbought or oversold conditions in the price of that security.
     * The RSI is displayed as an oscillator (a line graph) on a scale of zero to 100.
     * Traditionally, an RSI reading of 70 or above indicates an overbought condition. A reading of 30 or below indicates an oversold condition.
     *
     * @param prices Pointer to an array of double representing the price series.
     * @param length The total number of prices in the array.
     * @param window The size of the moving average window (number of periods).
     *
     * @return Pointer to a dynamically allocated array of doubles containing
     *         the RSI values. The length of this array is `length - window`.
     *         Returns NULL if input parameters are invalid or if memory allocation fails.
     *
     * @note Caller is responsible for freeing the returned array.
     */
    if (!prices || window >= length || window <= 0)
    {
        return NULL;
    }
    int result_length = length - window;
    double *RSI_Values = malloc(sizeof(double) * result_length); // pointer to an array of doubles
    if (!RSI_Values)
    {
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // compute price changes
    double *changes = malloc((length - 1) * sizeof(double)); // changes[0] = prices[1] - prices[0] → change from Day 0 to Day 1
    double *gains = malloc((length - 1) * sizeof(double));
    double *losses = malloc((length - 1) * sizeof(double));
    if (!changes || !gains || !losses)
    {
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < length; i++)
    {
        changes[i - 1] = prices[i] - prices[i - 1];
    }

    // seperate out gains and losses for first window
    double gain_sum = 0;
    double loss_sum = 0;
    for (int i = 0; i < window; i++)
    {
        if (changes[i] > 0)
        {
            gains[i] = changes[i];
            losses[i] = 0;
            gain_sum += gains[i];
        }
        else if (changes[i] < 0)
        {
            losses[i] = -1 * changes[i];
            gains[i] = 0;
            loss_sum += losses[i];
        }
        else
        {
            gains[i] = 0;
            losses[i] = 0;
        }
    }

    // compute average gains / losses over first [window] changes
    double avg_gain = gain_sum / window;
    double avg_loss = loss_sum / window;

    // calculate first RSI value
    // this corresponds to the price at prices[window]
    double RS;
    if (avg_loss == 0)
    {
        RSI_Values[0] = 100;
    }
    else
    {
        RS = avg_gain / avg_loss;
        RSI_Values[0] = 100 - (100 / (1 + RS));
    }

    // calculate subsquent RSI values
    double gain;
    double loss;
    for (int i = window; i < length - 1; i++)
    {
        gain = (changes[i] > 0) ? changes[i] : 0.0;
        loss = (changes[i] < 0) ? -1 * changes[i] : 0.0;

        // Wilder's smoothening formula: updates weighted average
        avg_gain = (avg_gain * (window - 1) + gain) / window;
        avg_loss = (avg_loss * (window - 1) + loss) / window;

        if (avg_loss == 0)
        {
            RSI_Values[i - window + 1] = 100;
        }
        else
        {
            RS = avg_gain / avg_loss;
            RSI_Values[i - window + 1] = 100 - (100 / (1 + RS));
        }
    }

    free(changes);
    free(gains);
    free(losses);
    return RSI_Values;
}

// Bollinger Bands

// MACD

// On-Balance Volume (OBV)