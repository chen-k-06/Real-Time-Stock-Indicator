/**
 * indicators.h
 * ------------
 * Declarations of financial technical indicator functions and types
 * for use in a high-performance stock analysis API.
 */

#define DEFAULT_WINDOW_SIZE 20
#define SUCCESS 0
#define FAILURE 1

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
double *compute_SMA(double *prices, int length, int window);

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
double *compute_EMA(double *prices, int length, int window);

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
double *compute_RSI(double *prices, int length, int window);

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
 * @note The caller is responsible for allocating and freeing the `std_devs` array.
 */
int compute_std_devs(double *prices, int length, int window, double *means, double *std_devs);

typedef struct
{
    double *middle_band;
    double *top_band;
    double *bottom_band;
    int length;
} BollingerBands;

/**
 * @brief Frees memory allocated for a BollingerBands struct and its fields.
 *
 * This function frees the dynamically allocated arrays for the middle,
 * top, and bottom bands inside the given `BollingerBands` struct,
 * and then frees the struct itself.
 *
 * @param band_values Pointer to the dynamically allocated BollingerBands struct to clean up.
 *
 * @note After calling this function, the pointer `band_values` becomes invalid.
 *       Do not use it after cleanup.
 */
void cleanup_bands(BollingerBands *band_values);

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
BollingerBands *compute_bollinger_bands(double *prices, int length, int window, double std_devs);