
#!/bin/python

import os
import numpy
import argparse
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser()
parser.add_argument('results_path', metavar='results-path')
parser.add_argument('plots_path', metavar='plots-path')

args = parser.parse_args()
os.chdir(args.results_path)
files = os.listdir('.')
files.sort(key=lambda x: int(x[:-4]))

datarate_array = range(20, 89)
all_means = []

labels = ['Authentication start', 'MAG receiving and forwarding authentication request', 'AuS receiving authentication request',
          'AuS sending authentication responses', 'MAG receiving and forwarding authentication response', 'MN receiving authentication response',
          'MN authenticating MAG', 'MAG authenticating MN', 'Authentication done']

for file in files:
    print(file)
    variables_values = numpy.loadtxt(file)
    current_datarate_means = numpy.mean(variables_values, axis=0)  # average over each column
    all_means.append(current_datarate_means)

all_means = numpy.delete(all_means, [1, 4, 5, 9, 11, 13, 15], 1)

plt.figure(figsize=(8, 8))
lineObjects = plt.plot(datarate_array, all_means)
plt.legend(iter(lineObjects), labels)
plt.title('Variation of time variables with respect to datarates')
plt.xlabel('Datarate (kbps)')
plt.ylabel('Time (s)')
plt.grid(linestyle = '--', linewidth = 0.5)

plt.savefig(args.plots_path + 'Figure.pdf', dpi=1000)
#plt.show()