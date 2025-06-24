import json
import numpy as np
import sys
import os

# ensure project root is on PYTHONPATH
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from backend.wrapper import (
    compute_SMA,
    compute_EMA,
    compute_RSI,
    compute_OBV,
    compute_bollinger_bands
)

def load_json(name):
    path = os.path.join(os.path.dirname(__file__), name)
    with open(path, 'r') as f:
        return json.load(f)

def test_sma():
    data = load_json("test_sma.json")
    prices, window = data["prices"], data["window"]
    expected = np.array(data["expected"], dtype=float)

    result = compute_SMA(prices, window)
    # result is numpy array
    if not np.allclose(result, expected, atol=1e-6):
        print("❌ SMA test failed")
        print("Expected:", expected)
        print("Got     :", result)
    else:
        print("✅ SMA test passed")

def test_ema():
    data = load_json("test_ema.json")
    prices, window = data["prices"], data["window"]
    expected = np.array(data["expected"], dtype=float)

    result = compute_EMA(prices, window)
    if not np.allclose(result, expected, atol=1e-6):
        print("❌ EMA test failed")
        print("Expected:", expected)
        print("Got     :", result)
    else:
        print("✅ EMA test passed")

def test_rsi():
    data = load_json("test_rsi.json")
    prices, window = data["prices"], data["window"]
    expected = np.array(data["expected"], dtype=float)

    result = compute_RSI(prices, window)
    if not np.allclose(result[-len(expected):], expected, atol=0.01):
        print("❌ RSI test failed")
        print("Expected:", expected)
        print("Got     :", result[-len(expected):])
    else:
        print("✅ RSI test passed")

def test_obv():
    data = load_json("test_obv.json")
    prices, volumes = data["prices"], data["volumes"]
    expected = np.array(data["expected"], dtype=float)

    result = compute_OBV(prices, volumes)
    if not np.allclose(result, expected, atol=1e-6):
        print("❌ OBV test failed")
        print("Expected:", expected)
        print("Got     :", result)
    else:
        print("✅ OBV test passed")
def test_bollinger():
    data = load_json("test_bollinger.json")
    prices = data["prices"]
    window = data["window"]
    std_devs = data["std_devs"]

    # expected arrays from JSON
    exp_mid = np.array(data["expected_middle"], dtype=float)
    exp_top = np.array(data["expected_top"], dtype=float)
    exp_bot = np.array(data["expected_bottom"], dtype=float)

    # call your wrapper (returns Nx3 array)
    bb = compute_bollinger_bands(prices, window, std_devs)

    # unpack columns: 0=bottom, 1=middle, 2=top
    bot = bb[:, 0]
    mid = bb[:, 1]
    top = bb[:, 2]

    ok_mid = np.allclose(mid, exp_mid, atol=1e-6)
    ok_top = np.allclose(top, exp_top, atol=1e-6)
    ok_bot = np.allclose(bot, exp_bot, atol=1e-6)

    if not (ok_mid and ok_top and ok_bot):
        print("❌ Bollinger Bands test failed")
        print("Expected middle:", exp_mid, "Got middle:", mid)
        print("Expected top   :", exp_top, "Got top   :", top)
        print("Expected bot   :", exp_bot, "Got bot   :", bot)
    else:
        print("✅ Bollinger Bands test passed")


if __name__ == "__main__":
    test_sma()
    test_ema()
    test_rsi()
    test_obv()
    test_bollinger()
