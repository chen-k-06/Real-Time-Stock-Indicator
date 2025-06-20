/**
 * indicators.c
 * ------------
 * Implements financial technical indicators in C for use in a high-performance stock analysis API.
 *
 * Financial indicators are metrics used to assess the financial health and performance of an entity, such as a company or an economy.
 * They provide insights into various aspects of financial stability, profitability, and growth potential.
 *
 * Indicators included:
 *  - Simple Moving Average (SMA)
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

#include "indicators.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

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
double *compute_SMA(double *prices, int length, int window)
{

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
        return NULL;
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
double *compute_EMA(double *prices, int length, int window)
{

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
        free(SMA_Values);
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        return NULL;
    }
    if (!SMA_Values)
    {
        free(EMA_Values);
        free(SMA_Values);
        fprintf(stderr, "Get SMA failed. %s.", strerror(errno));
        return NULL;
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

/**
 * @brief Computes the Relative Strength Index (RSI) of a price series.
 *
 * The RSI is a momentum oscillator that measures the speed and magnitude of
 * recent price changes to identify overbought or oversold conditions.
 * It is calculated using Wilder's smoothing method over a given window size.
 *
 * @param prices Pointer to an array of double-precision prices.
 * @param length Total number of price entries in the array.
 * @param window Lookback period over which RSI is calculated (typically 14).
 *
 * @return Pointer to a dynamically allocated array of RSI values.
 *         The array has length (length - window), corresponding to
 *         RSI values for prices[window] to prices[length - 1].
 *         Returns NULL if input is invalid or memory allocation fails.
 *
 * @note Caller is responsible for freeing the returned array.
 */
double *compute_RSI(double *prices, int length, int window)
{
    if (!prices || window >= length || window <= 0)
    {
        return NULL;
    }
    int result_length = length - window;
    double *RSI_Values = malloc(sizeof(double) * result_length); // pointer to an array of doubles
    if (!RSI_Values)
    {
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        return NULL;
    }

    // compute price changes
    double *changes = malloc((length - 1) * sizeof(double)); // changes[0] = prices[1] - prices[0] → change from Day 0 to Day 1
    double *gains = malloc((length - 1) * sizeof(double));
    double *losses = malloc((length - 1) * sizeof(double));
    if (!changes || !gains || !losses)
    {
        free(RSI_Values);
        free(changes);
        free(gains);
        free(losses);

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

/**
 * @brief Computes the standard deviation of a price series over a sliding window.
 *
 * This function calculates the standard deviation of the price values within each
 * window of size `window`. It stores the results in the pre-allocated `std_devs` array.
 *
 * The length of the `std_devs` array should be `length - window + 1`.
 *
 * @param prices    Pointer to an array of double representing the price series.
 * @param length    The total number of prices in the `prices` array.
 * @param window    The size of the moving window (number of periods).
 * @param means     Pointer to a precomputed array of mean (SMA) values for each window.
 *                  Length is `length - window + 1`.
 * @param std_devs  Pointer to a pre-allocated array where computed standard deviations
 *                  will be stored. Length is `length - window + 1`.
 *
 * @return EXIT_SUCCESS/0 on success. EXIT_FAILURE/1 on failure/if arguments not valid.
 *
 * @note The caller is responsible for allocating and freeing the `std_devs` array.
 */
int compute_std_devs(double *prices, int length, int window, double *means, double *std_devs)
{
    if (!prices || !means || !std_devs || length <= 0 || window <= 0 || window > length)
        return EXIT_FAILURE;

    int result_length = length - window + 1;

    // compute std dev
    for (int i = 0; i < result_length; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < window; j++)
        {
            double difference = prices[i + j] - means[i];
            sum += difference * difference;
        }
        double variance = sum / window;
        std_devs[i] = sqrt(variance);
    }
    return EXIT_SUCCESS;
}

typedef struct
{
    double *middle_band; // SMA values
    double *top_band;    // SMA + stddevs*stddev
    double *bottom_band; // SMA - stddevs*stddev
    int length;
} BollingerBands;

void cleanup_bands(BollingerBands *band_values)
{
    free(band_values->middle_band);
    free(band_values->top_band);
    free(band_values->bottom_band);
    free(band_values);
}

/**
 * @brief Computes the Bollinger Bands of a price series.
 *
 * Bollinger Bands are a technical analysis tool used to determine where prices are high and low relative to each other.
 * These bands are composed of three lines: a simple moving average (the middle band) and an upper and lower band.
 * The upper and lower bands are typically two standard deviations above or below a 20-period simple moving average (SMA).
 * The bands widen and narrow as the volatility of the underlying asset changes.
 *
 * @param prices Pointer to an array of double-precision prices.
 * @param length Total number of price entries in the array.
 * @param window Lookback period over which bands are calculated (typically 20).
 * @param std_devs Scalar multiplier for the number of standard deviations
 *                 the upper and lower bands are from the center line
 *                 (typically 2).
 *
 * @return Pointer to a dynamically allocated struct (BollingerBands) of band values.
 *         The struct contains 3 fields, corresponding to
 *         top, middle, and bottom band values for prices[window] to prices[length - 1].
 *         Each field stores an array of length = length - window + 1.
 *         Returns NULL if input is invalid or memory allocation fails.
 *
 * @note Caller is responsible for freeing the returned struct and struct fields.
 */
BollingerBands *compute_bollinger_bands(double *prices, int length, int window, double std_devs)
{
    if (!prices || window >= length || window <= 0 || std_devs <= 0)
    {
        return NULL;
    }
    BollingerBands *band_values = malloc(sizeof(BollingerBands)); // band_values is a pointer to a struct, not a struct variable.
                                                                  // therefore access struct values through ->, not .
    if (!band_values)
    {
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        return NULL;
    }

    // malloc struct fields
    int result_length = length - window + 1;
    double *SMA_Values = compute_SMA(prices, length, window); // middle band
    band_values->length = result_length;
    band_values->middle_band = SMA_Values; // middle_band points to externally-managed memory; do NOT free both band_values->middle_band AND SMA_Values.
                                           // WILL cause a double free
    band_values->top_band = malloc(sizeof(double) * result_length);
    band_values->bottom_band = malloc(sizeof(double) * result_length);
    double *stddev_values = malloc(sizeof(double) * result_length);
    if (!band_values->top_band || !band_values->bottom_band || !stddev_values)
    {
        free(stddev_values);
        cleanup_bands(band_values);
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        return NULL;
    }
    if (!SMA_Values)
    {
        free(stddev_values);
        cleanup_bands(band_values);
        fprintf(stderr, "Get SMA failed. %s.", strerror(errno));
        return NULL;
    }

    // calculate standard deviation
    compute_std_devs(prices, length, window, band_values->middle_band, stddev_values);

    for (int i = 0; i < result_length; i++)
    {
        band_values->top_band[i] = band_values->middle_band[i] + std_devs * stddev_values[i];
        band_values->bottom_band[i] = band_values->middle_band[i] - std_devs * stddev_values[i];
    }

    free(stddev_values);
    return band_values; // pointer to a BollingerBands struct
}

// MACD

// On-Balance Volume (OBV)