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
    double *MCAD_Values;
    double *signal_line_Values;
} MCAD;
int cleanup_MCAD(MCAD *mcad);
MCAD *compute_MCAD(double *prices, int length);
double *compute_OBV(const double *prices, const double *volumes, int length);
""")

# Load the shared library with ffi.dlopen(...)
lib = ffi.dlopen("../c_engine/indicators.so")

# wrap functions
def compute_SMA(prices, window):
    # converts prices to a numpy array of doubles
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
    # converts prices to a numpy array of doubles
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
