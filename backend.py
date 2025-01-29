import requests
import pandas as pd
import time
import numpy as np
from astral import Location
import astral

import datetime

# Function to get the sunrise, sunset, and other times
def get_day_times():
    a=astral.Astral()
    city = a['Jerusalem']
    #astral(name='Rehovot',region= "Israel", timezone="Asia/Jerusalem", latitude=31.89,longitude= 34.81)
    s = city.sun(date=datetime.date.today(), local=True)
    return s['sunrise']


# Configuration
channel1ID = 2766376
channel2ID = 2820739
channel3ID=2804506
apiReadKey = "AA08C1N0PHZGSSS8"
apiWriteKey = "0E2ETUHX1YEB5H7P"
controlApiReadKey="PXZI6OG13O5QMZW5"
controlApiWriteKey="B8FJ36JXSB115B8K"
vpdStationReadApiKey='K97I6D8WEXKL0SXQ'
outputFile = "field_data.csv"
flowRate=1
# Parameters for communication with ThingSpeak
readParams = {"api_key": apiReadKey, "results": 2}

# ThingSpeak API endpoint
dataServer = f"https://api.thingspeak.com/channels/{channel1ID}/feeds.json"
controlServer = f"https://api.thingspeak.com/channels/{channel2ID}/bulk_update.json"
# Get data from ThingSpeak

#set the control channel
fertFlag=requests.get(f"https://api.thingspeak.com/channels/{channel2ID}/feeds.json?api_key={controlApiReadKey}&results=1")

#(get_day_times().hour)

def turnOn1():
    
    try:
        fertFlag = requests.get(f"https://api.thingspeak.com/channels/{channel2ID}/feeds.json?api_key={controlApiReadKey}&results=1")
        states = fertFlag.json()['feeds']
        lastState = [states[0]['field2'],states[0]['field4'],states[0]['field5']]
        
        fertFlag = requests.post(
            f"https://api.thingspeak.com/update.json?api_key={controlApiWriteKey}",
            json={"field1": 1, "field2": lastState[0], "field3": 1, "field4": lastState[1], "field5": lastState[2]}
        )
        print(fertFlag)
        if fertFlag.status_code == 200:
            print(f"Fertigation turned on")
        else:
            print(f"Failed to turn on fertigation (Status Code: {fertFlag.status_code})")
    except Exception as e:
        print(f"Error turning on fertigation: {e}")
def turnOn2():
   
    try:
        fertFlag = requests.get(f"https://api.thingspeak.com/channels/{channel2ID}/feeds.json?api_key={controlApiReadKey}&results=1")
        states = fertFlag.json()['feeds']
        lastState = [states[0]['field1'],states[0]['field3'],states[0]['field4'],states[0]['field5']]
        fertFlag = requests.post(
            f"https://api.thingspeak.com/update.json?api_key={controlApiWriteKey}",
            json={"field1": lastState[0], "field2": 1, "field3": lastState[1], "field4": lastState[2], "field5": lastState[3]}
        )
        print(fertFlag)
        if fertFlag.status_code == 200:
            print(f"Fertigation turned on")
        else:
            print(f"Failed to turn on fertigation (Status Code: {fertFlag.status_code})")
    except Exception as e:
        print(f"Error turning on fertigation: {e}")
def turnOff1():
    
    try:
        fertFlag = requests.get(f"https://api.thingspeak.com/channels/{channel2ID}/feeds.json?api_key={controlApiReadKey}&results=1")
        states = fertFlag.json()['feeds']
        lastState = [states[0]['field2'],states[0]['field3'],states[0]['field4'],states[0]['field5']]
        fertFlag = requests.post(
            f"https://api.thingspeak.com/update.json?api_key={controlApiWriteKey}",
            json={"field1": 0, "field2": lastState[0], "field3": lastState[1], "field4": lastState[2], "field5": lastState[3]}
        )
        print(fertFlag)
        if fertFlag.status_code == 200:
            print(f"Fertigation turned off")
        else:
            print(f"Failed to turn on fertigation (Status Code: {fertFlag.status_code})")
    except Exception as e:
        print(f"Error turning on fertigation: {e}")
def turnOff2():

    
    try:
        fertFlag = requests.get(f"https://api.thingspeak.com/channels/{channel2ID}/feeds.json?api_key={controlApiReadKey}&results=1")
        states = fertFlag.json()['feeds']
        lastState = [states[0]['field1'],states[0]['field3'],states[0]['field4'],states[0]['field5']]
        fertFlag = requests.post(
            f"https://api.thingspeak.com/update.json?api_key={controlApiWriteKey}",
            json={"field1": lastState[0], "field2": 0, "field3": lastState[1], "field4": lastState[2], "field5": lastState[3]}
        )
        print(fertFlag)
        if fertFlag.status_code == 200:
            print(f"Fertigation turned on")
        else:
            print(f"Failed to turn on fertigation (Status Code: {fertFlag.status_code})")
    except Exception as e:
        print(f"Error turning on fertigation: {e}")
# returns vpd value
def getVpd():
    response = requests.get(f"https://api.thingspeak.com/channels/{channel3ID}/feeds.json?api_key={vpdStationReadApiKey}&results=1")
    data = response.json()
    df = pd.DataFrame(data['feeds'])
    temp=float(df['field1'][0])
    relativeHumidity=float(df['field2'][0])
    # calculate vpd
    vpd=0.611*10**((7.5*temp)/(237.7+temp))*(1-relativeHumidity/100)
    print(df)
    return 0.5

def fertedToday():
    response = requests.get(f"https://api.thingspeak.com/channels/{channel2ID}/feeds.json?api_key={controlApiReadKey}&results=20")
    data = response.json()
    df = pd.DataFrame(data['feeds'])
    # Ensure the column is numeric (convert None to NaN if necessary)
    df['field3'] = pd.to_numeric(df['field3'], errors='coerce')

    # Find the last non-null value and its index
    last_valid_index = df['field3'].last_valid_index()
    last_valid_value = df.loc[last_valid_index, 'field3'] if last_valid_index is not None else None
    return last_valid_value==1

def startWatering():        
    try:
        # Fetch the current mass of the plant from Channel 1
        response = requests.get(f"https://api.thingspeak.com/channels/{channel1ID}/feeds.json?api_key={apiReadKey}&results=1")
        response.raise_for_status()  # Check if the request was successful
        data = response.json()
        df = pd.DataFrame(data['feeds'])
        currentMass = float(df['field1'][0])  # Ensure conversion to float
        
        # Fetch the fertilization control data from Channel 2
        fertFlag = requests.get(f"https://api.thingspeak.com/channels/{channel2ID}/feeds.json?api_key={controlApiReadKey}&results=10")
        fertFlag.raise_for_status()  # Check if the request was successful
        data = fertFlag.json()
        df = pd.DataFrame(data['feeds'])
        
        # Ensure 'field4' is numeric (convert invalid entries to NaN)
        df['field4'] = pd.to_numeric(df['field4'], errors='coerce')
        
        # Find the last non-null value in 'field4'
        last_valid_index = df['field4'].last_valid_index()
        last_valid_value = df.loc[last_valid_index, 'field4'] if last_valid_index is not None else None
        
        if last_valid_value is None:
            print("No valid value found in 'field4'. Cannot calculate total watering.")
            return

        # Calculate the total amount of water needed
        totalWatering = float(last_valid_value) - currentMass
        print(f"Total watering needed: {totalWatering} ml")
        # Post the watering command to Channel 2
        watering_response = requests.post(
            f"https://api.thingspeak.com/update.json?api_key={controlApiWriteKey}",
            json={"field1": 0, "field2": 0, "field3": 1, "field5": totalWatering}
        )

        watering_response.raise_for_status()  # Check if the POST request was successful
        print("Watering data sent successfully.")
        
    except requests.exceptions.RequestException as e:
        print(f"Error with the API request: {e}")
    except KeyError as e:
        print(f"Missing expected key in the response data: {e}")
    except ValueError as e:
        print(f"Error processing numeric values: {e}")
fertFlag = requests.post(
            f"https://api.thingspeak.com/update.json?api_key={controlApiWriteKey}",
            json={"field1": 0, "field2": 0, "field3": 1, "field4": 100, "field5": 100}
        )
# Main loop
while True:
    getVpd() 
# start analysis hour after sunrise if not already fertigated
    if time.localtime()[3] >= (get_day_times().hour + 1) and not fertedToday():
        response = requests.get(dataServer, params=readParams)
        data = response.json()
        df = pd.DataFrame(data['feeds'])
        value=(float(df['field1'][1])-float(df['field1'][0]))/getVpd()
        count=0
        while (count<15)and(value<0.5):
            response = requests.get(dataServer, params=readParams)
            data = response.json()
            df = pd.DataFrame(data['feeds'])
            value=(float(df['field1'][1])-float(df['field1'][0]))/getVpd()
            count+=1
            time.sleep(30)
        #turn on fertigation+set fertFlag of toady true
        turnOn1()
        #fert for 15 minutes
        time.sleep(900)
        turnOff1()

    # # Ensure 'created_at' is a datetime column
    # df['created_at'] = pd.to_datetime(df['created_at'])

    # # Filter rows based on the hour
    # filtered_df = df[df['created_at'] > get_day_times()]  # Customize minute range if needed
    # # Ensure 'field1' and 'field2' columns are numeric
    # filtered_df['field1'] = pd.to_numeric(filtered_df['field1'], errors='coerce')
    


#if 11:59 PM set fertFlag of today to false
    if time.localtime()[3]==23 and time.localtime()[4]==59:
        fertFlag = requests.post(
            f"https://api.thingspeak.com/update.json?api_key={controlApiWriteKey}",
            json={"field1": 0, "field2": 0, "field3": 0}
        )
    
    print("fertFlag set to false")

# starts fertigation at 11:00 AM to 11:15 AM on fertPump2
    if time.localtime()[3] == 11 and time.localtime()[4] == 0:
        turnOn2()
        time.sleep(900)
        turnOff2()
        print("Fertigation on pump 2 started")
    
# starts watering at 9:00 PM 
    if time.localtime()[3] == 21 and time.localtime()[4] == 0:
        startWatering()
time.sleep(60)        
 

