from dotenv import load_dotenv
import os
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from wrapper import compute_SMA, compute_EMA, compute_RSI, compute_bollinger_bands, compute_MACD, compute_OBV

# load environment variables (API key)
load_dotenv()
api_key = os.getenv("ALPHA_VANTAGE_API_KEY")

if not api_key:
    raise ValueError("Missing ALPHA_VANTAGE_API_KEY in environment variables.")

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["https://chen-k-06.github.io"],  # frontend URL
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.get("/")
def read_root():
    return {"Hello": "World"}

#------------------------------------------------
# SMA Retrival 
#------------------------------------------------
class GetSMA(BaseModel):
    prices: list[float]
    window: int

@app.post("/get_sma", response_model=list[float])
def get_sma(request: GetSMA) -> list[float]: 
    result = compute_SMA(request.prices, request.window)
    return result.tolist()

#------------------------------------------------
# EMA Retrival 
#------------------------------------------------
class GetEMA(BaseModel):
    prices: list[float]
    window: int

@app.post("/get_ema", response_model=list[float])
def get_ema(request: GetEMA) -> list[float]: 
    result = compute_EMA(request.prices, request.window)
    return result.tolist()

#------------------------------------------------
# RSI Retrival 
#------------------------------------------------
class GetRSI(BaseModel):
    prices: list[float]
    window: int

@app.post("/get_rsi", response_model=list[float])
def get_rsi(request: GetRSI) -> list[float]: 
    result = compute_RSI(request.prices, request.window)
    return result.tolist()

#------------------------------------------------
# Bollinger Bands Retrival
#------------------------------------------------
class GetBB(BaseModel):
    prices: list[float]
    window: int

@app.post("/get_bollinger_bands", response_model=list[float])
def get_bollinger_bands(request: GetBB) -> list[float]:
    result = compute_bollinger_bands(request.prices, request.window)
    return result.tolist()

#------------------------------------------------
# MCAD Retrival
#------------------------------------------------
class GetMACD(BaseModel):
    prices: list[float]

@app.post("/get_macd", response_model=list[float])
def get_MCAD(request: GetMACD)-> list[float]:
    result = compute_MACD(request.prices)
    return result.tolist()


#------------------------------------------------
# OBV Retrival
#------------------------------------------------
class GetOBV(BaseModel):
    prices: list[float]
    volumes: list[float]

@app.post("/get_obv", response_model=list[float])
def get_OBV(request: GetOBV)-> list[float]:
    result = compute_OBV(request.prices, request.volumes)
    return result.tolist()