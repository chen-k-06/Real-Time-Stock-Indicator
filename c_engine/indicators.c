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
    double *SMA_Values = compute_SMA(prices, length, window);    // EMA values are based on SMA values with a smoothener applied

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

int compute_std_devs(double *prices, int length, int window, double *means, double *std_devs)
{
    if (!prices || !means || !std_devs || length <= 0 || window <= 0 || window > length)
    {
        return NULL;
    }

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

void cleanup_bands(BollingerBands *band_values)
{
    free(band_values->middle_band);
    free(band_values->top_band);
    free(band_values->bottom_band);
    free(band_values);
}

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

void cleanup_MCAD(MACD *mcad)
{
    free(mcad->MCAD_Values);
    free(mcad->signal_line_Values);
    free(mcad);
}

MACD *compute_MACD(double *prices, int length)
{
    // data verification
    if (!prices || length - 26 - 9 + 1 <= 0)
    {
        return NULL;
    }

    // mermory management
    MACD *mcad = malloc(sizeof(MACD));
    if (!mcad)
    {
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        return NULL;
    }

    // allocate struct memory
    int result_length = length - 26 - 9 + 1; // also the length of EMA_26
    mcad->length = result_length;
    mcad->MCAD_Values = malloc(sizeof(double) * result_length);
    mcad->signal_line_Values = malloc(sizeof(double) * result_length);
    if (!mcad->MCAD_Values || !mcad->signal_line_Values)
    {
        cleanup_MCAD(mcad);
        fprintf(stderr, "Malloc failed. %s.", strerror(errno));
        return NULL;
    }

    // calculate EMA of 12 day and 26 day periods
    double *EMA_12 = compute_EMA(prices, length, 12); // must be freed
    double *EMA_26 = compute_EMA(prices, length, 26); // must be freed
    if (!EMA_12 || !EMA_26)
    {
        cleanup_MCAD(mcad);
        fprintf(stderr, "Compute EMA failed. %s.", strerror(errno));
        return NULL;
    }

    // calculate first 8 temp MCAD values
    double *temp_MCADs = malloc(sizeof(double) * 8);
    for (int i = 0; i < 8; i++)
    {
        temp_MCADs[i] = EMA_12[i + 14] - EMA_26[i]; // because the windows are different, EMA_12[0] corresponds with
                                                    // prices[12 -1], whereas EMA_26[0] -> prices[26-1]
    }

    // compute remaining MCAD values
    for (int i = 0; i < result_length; i++)
    {
        mcad->MCAD_Values[i] = EMA_12[i + 14] - EMA_26[i];
    }

    // compute signal values

    free(temp_MCADs);
    free(EMA_12);
    free(EMA_26);
    return mcad;
}

// On-Balance Volume (OBV)