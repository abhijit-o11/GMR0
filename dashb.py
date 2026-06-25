import streamlit as st
import serial
import matplotlib.pyplot as plt

# 🔌 CHANGE THIS to your Arduino COM port
ser = serial.Serial('COM4', 115200)

st.title("EEG Monitoring System")

raw_data = []
filtered_data = []

chart = st.line_chart()

while True:
    try:
        line = ser.readline().decode().strip()
        raw, clean, state = line.split(',')

        raw = float(raw)
        clean = float(clean)
        state = int(state)

        raw_data.append(raw)
        filtered_data.append(clean)

        raw_data = raw_data[-50:]
        filtered_data = filtered_data[-50:]

        chart.add_rows({
            "Raw": raw,
            "Filtered": clean
        })

        if state == 0:
            st.success("Normal State")
        elif state == 1:
            st.warning("Pre-ictal State")
        else:
            st.error("Seizure Detected")

    except:
        pass