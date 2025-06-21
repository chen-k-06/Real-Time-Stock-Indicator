from dotenv import load_dotenv
import os
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from wrapper import compute_SMA, compute_EMA

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
