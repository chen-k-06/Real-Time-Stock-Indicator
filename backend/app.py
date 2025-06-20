# FastAPI app serverfrom dotenv import load_dotenv
from dotenv import load_dotenv
import os

load_dotenv()  
api_key = os.getenv("ALPHA_VANTAGE_API_KEY")

if not api_key:
    raise ValueError("Missing ALPHA_VANTAGE_API_KEY in environment variables.")