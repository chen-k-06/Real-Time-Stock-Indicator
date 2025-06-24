# Python bridge between FastAPI and C shared library using cffi
from cffi import FFI
ffi = FFI() # Foreign Function Interface
import numpy as np

ffi.cdef("""
void c_free(void *ptr);
double *compute_SMA(double *prices, int length, int window);
double *compute_EMA(double *prices, int length, int window);
double *compute_RSI(double *prices, int length, int window);
int compute_std_devs(double *prices, int length, int window, double *means, double *std_devs);
typedef struct
{
    double *middle_band;
    double *top_band;
    double *bottom_band;
    int length;
} BollingerBands;
void cleanup_bands(BollingerBands *band_values);
BollingerBands *compute_bollinger_bands(double *prices, int length, int window, double std_devs);
typedef struct
{
    int length;
    double *MACD_Values;
    double *signal_line_Values;
} MACD;
int cleanup_MACD(MACD *macd);
MACD *compute_MACD(double *prices, int length);
double *compute_OBV(const double *prices, const double *volumes, int length);
""")

# Load the shared library with ffi.dlopen(...)
lib = ffi.dlopen("../c_engine/indicators.so")

# wrap functions
def compute_SMA(prices, window):
    # converts prices to a numpy array of doubles -> data verification
    prices_arr = np.asarray(prices, dtype=np.double)
    length = len(prices_arr)
    
    # data verification
    if window <= 0 or window > length:
        raise ValueError("Invalid window size")
    
    # crate a C array using the values from the numpy array
    c_prices = ffi.new("double[]", prices_arr.tolist())  # cast to list for iterable format
    
    # run C function. store results in pointer
    result_ptr = lib.compute_SMA(c_prices, length, window) 
    if result_ptr == ffi.NULL:
        raise RuntimeError("C function returned NULL")

    # copy results into new numpy array 
    result_length = length - window + 1
    result = np.array([result_ptr[i] for i in range(result_length)])
    
    lib.c_free(result_ptr)

    return result

def compute_EMA(prices, window):
    # converts prices to a numpy array of doubles -> data verification
    prices_arr = np.asarray(prices, dtype=np.double)
    length = len(prices_arr)

    # data verification
    if window <= 0 or window > length:
        raise ValueError("Invalid window size")
    
    # crate a C array using the values from the numpy array
    c_prices = ffi.new("double[]", prices_arr.tolist()) 
    
    # run C function. store results in pointer
    result_ptr = lib.compute_EMA(c_prices, length, window)
    if result_ptr == ffi.NULL:
        raise RuntimeError("C function returned NULL")

    # copy results into new numpy array 
    result_length = length - window + 1
    result = np.array([result_ptr[i] for i in range(result_length)])
    
    lib.c_free(result_ptr)

    return result

def compute_RSI(prices, window):
    # converts prices to a numpy array of doubles -> data verification
    prices_arr = np.asarray(prices, dtype=np.double)
    length = len(prices_arr)

    # data verification
    if window <= 0 or window > length:
        raise ValueError("Invalid window size")
    
    # crate a C array using the values from the numpy array
    c_prices = ffi.new("double[]", prices_arr.tolist()) 

    # run C function. store results in pointer
    result_ptr = lib.compute_RSI(c_prices, length, window)
    if result_ptr == ffi.NULL:
        raise RuntimeError("C function returned NULL")
    
    # copy results into new numpy array 
    result_length = length - window + 1
    result = np.array([result_ptr[i] for i in range(result_length)])
    
    lib.c_free(result_ptr)

    return result
    
def compute_bollinger_bands(prices, window):
    # converts prices to a numpy array of doubles -> data verification
    prices_arr = np.asarray(prices, dtype=np.double)
    length = len(prices_arr)

    # data verification
    if window <= 0 or window > length:
        raise ValueError("Invalid window size")
    
    # malloc space for std_devs array 
    result_length = length - window + 1
    std_devs = lib.malloc_std_devs(result_length)
    if (std_devs == None):
        raise ValueError("Malloc failed")
    
    # crate a C array using the values from the numpy array
    c_prices = ffi.new("double[]", prices_arr.tolist()) 

    # run C function. store results in pointer
    result_ptr = lib.compute_bollinger_bands(c_prices, length, window, std_devs)
    if result_ptr == ffi.NULL:
        raise RuntimeError("C function returned NULL")
    
    # copy struct into new numpy array 
    result = result_ptr[0]

    if result.middle_band == ffi.NULL or result.top_band == ffi.NULL or result.bottom_band == ffi.NULL:
        lib.cleanup_bands(result_ptr)
        raise RuntimeError("Band arrays were not allocated properly")

    n = result.length

    middle = np.frombuffer(ffi.buffer(result.middle_band, n * 8), dtype=np.double).copy()
    top    = np.frombuffer(ffi.buffer(result.top_band,    n * 8), dtype=np.double).copy()
    bottom = np.frombuffer(ffi.buffer(result.bottom_band, n * 8), dtype=np.double).copy()

    lib.cleanup_bands(result_ptr)
    lib.c_free(std_devs)

    return np.stack([bottom, middle, top], axis=1) # 2-D numpy array of (result_length, 3)

def compute_MACD(prices):
    if (prices == None or len(prices) == 0):
        raise RuntimeError("Invalid prices array")
                           
    # converts prices to a numpy array of doubles -> data verification
    prices_arr = np.asarray(prices, dtype=np.double)
    length = len(prices_arr)

    # crate a C array using the values from the numpy array
    c_prices = ffi.new("double[]", prices_arr.tolist()) 

    # run C function. store results in pointer
    result_ptr = lib.compute_MACD(c_prices, length)
    if result_ptr == ffi.NULL:
        raise RuntimeError("C function returned NULL")
    
    # copy struct into new numpy array 
    result = result_ptr[0]

    if result.MACD_Values == ffi.NULL or result.signal_line_Values == ffi.NULL:
        lib.cleanup_MACD(result_ptr)
        raise RuntimeError("MACD arrays were not allocated properly")

    n = result.length

    MACD = np.frombuffer(ffi.buffer(result.MACD_Values, n * 8), dtype=np.double).copy()
    signal = np.frombuffer(ffi.buffer(result.signal_line_Values, n * 8), dtype=np.double).copy()

    lib.cleanup_MACD(result_ptr)

    return np.stack([MACD, signal], axis=1)

def compute_OBV(prices, volumes):
    if prices is None or volumes is None: 
        raise ValueError("Invalid arguments")

    # converts prices to a numpy array of doubles -> data verification
    prices_arr = np.asarray(prices, dtype=np.double)
    volume_arr = np.asarray(volumes, dtype=np.double)
    length = len(prices_arr)

    if (len(prices) != len(volumes)):
        raise ValueError("Prices and volumes array should be the same length")
    
    # crate a C array using the values from the numpy array
    c_prices = ffi.new("double[]", prices_arr.tolist()) 
    c_volumes = ffi.new("double[]", volume_arr.tolist()) 
    
    # run C function. store results in pointer
    result_ptr = lib.compute_OBV(c_prices, c_volumes, length)
    if result_ptr == ffi.NULL:
        raise RuntimeError("C function returned NULL")

    # copy results into new numpy array 
    result_length = length
    result = np.array([result_ptr[i] for i in range(result_length)])
    
    lib.c_free(result_ptr)

    return result