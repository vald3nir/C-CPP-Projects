#!/usr/bin/python3

import csv
import matplotlib.pyplot as plt

signal = []
with open('signal.csv') as csv_file:
    for row in csv.reader(csv_file, delimiter=',', quotechar='|'):
        signal.append(int(row[0]))

plt.plot(signal)
plt.grid(True)
plt.show()
