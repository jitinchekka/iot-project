import pickle
import os.path
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request


def gsheet_api_check(SCOPES):
    creds = None
    if os.path.exists("token.pickle"):
        with open("token.pickle", "rb") as token:
            creds = pickle.load(token)
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file("credentials.json", SCOPES)
            creds = flow.run_local_server(port=0)
        with open("token.pickle", "wb") as token:
            pickle.dump(creds, token)
    return creds


from googleapiclient.discovery import build


def pull_sheet_data(SCOPES, SPREADSHEET_ID, DATA_TO_PULL):
    creds = gsheet_api_check(SCOPES)
    service = build("sheets", "v4", credentials=creds)
    sheet = service.spreadsheets()
    result = (
        sheet.values().get(spreadsheetId=SPREADSHEET_ID, range=DATA_TO_PULL).execute()
    )
    values = result.get("values", [])

    if not values:
        print("No data found.")
    else:
        rows = (
            sheet.values()
            .get(spreadsheetId=SPREADSHEET_ID, range=DATA_TO_PULL)
            .execute()
        )
        data = rows.get("values")
        print("COMPLETE: Data copied")
        return data


import pandas as pd

SCOPES = ["https://www.googleapis.com/auth/spreadsheets"]
SPREADSHEET_ID = "1SO_75vaxuNKqEC3u1TRsboGCvhix74Fv3SDrmetMJ0k"
DATA_TO_PULL = "Sheet1"
data = pull_sheet_data(SCOPES, SPREADSHEET_ID, DATA_TO_PULL)
df = pd.DataFrame(data)
# Drop first two columns
df.drop(df.columns[[0, 1]], axis=1, inplace=True)
df.columns = ["temp", "current", "gas", "class"]
# For column 'class', replace None with '0'
df["class"] = df["class"].fillna(0)

# Generate random data for class 1 with temp > 60 and gas > 200
import random

for i in range(0, 30):
    df.loc[len(df.index)] = [
        random.randint(60, 100),
        random.randint(0, 1000),
        random.randint(200, 500),
        1,
    ]

# Drop first row
df.drop(df.index[0], inplace=True)


# Convert class to int
df["class"] = df["class"].astype(int)

# train the above data with bayes classifier
from sklearn.naive_bayes import GaussianNB
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score

gnb = GaussianNB()
X = df.drop("class", axis=1)
y = df["class"]
gnb.fit(X, y)
X_test = [[30, 5, 100]]
y_test = [0]
y_pred = gnb.predict(X_test)
print("Predicted class: ", y_pred[0])

import streamlit as st

# Print the predicted class
if y_pred == 0:
    st.write("Predicted class: safe")
else:
    st.write("Predicted class: unsafe")

