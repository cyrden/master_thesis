#!/usr/bin/env python

import matplotlib.pyplot as plt
import numpy as np
import json
from argparse import ArgumentParser as ap

parser = ap(description='Plot performances')
#parser.add_argument('-f', '--file', required=True, help="Filepath of the json file to be read")
args = parser.parse_args()

times1 = []
memory_usage1 = []
times2 = []
memory_usage2 = []
times3 = []
memory_usage3 = []
#with open(args.file) as f:
with open('memory_json/vanillafrr.json') as f:
    for line in f:
        if line != ("\n" or None or ""):
            times1.append(float(json.loads(line)['time']))
            memory_usage1.append(float(json.loads(line)['info']['memory_full_info'][8]))

with open('memory_json/myfrr_noplugins.json') as f:
    for line in f:
        if line != ("\n" or None or ""):
            times2.append(float(json.loads(line)['time']))
            memory_usage2.append(float(json.loads(line)['info']['memory_full_info'][8]))

#with open('mem_json/myfrr_5ins_7ets_newmalloc_2.json') as f:
with open('memory_json/myfrr_5plugins.json') as f:
    for line in f:
        if line != ("\n" or None or ""):
            times3.append(float(json.loads(line)['time']))
            memory_usage3.append(float(json.loads(line)['info']['memory_full_info'][8]))


times1[:] = [((x - times1[0])) for x in times1]
times1[:] = [(x/10**9) for x in times1]
memory_usage1[:] = [(x/1000000) for x in memory_usage1]

times2[:] = [((x - times2[0])) for x in times2]
times2[:] = [(x/10**9) for x in times2]
memory_usage2[:] = [(x/1000000) for x in memory_usage2]

times3[:] = [((x - times3[0])) for x in times3]
times3[:] = [(x/10**9) for x in times3]
memory_usage3[:] = [(x/1000000) for x in memory_usage3]

f, pltarr = plt.subplots(3, sharey=True)

pltarr[0].plot(times1, memory_usage1)
pltarr[0].set_title("vanilla ospf")    
pltarr[0].set_xlabel('time (s)')
pltarr[0].set_ylabel('memory usage (MB)')
pltarr[0].set_yticks([2, 4, 6])
pltarr[0].axhline(linewidth=0.4, color='k', linestyle='-.', y = 2)
pltarr[0].axhline(linewidth=0.4, color='k', linestyle='-.', y = 4)
pltarr[0].axhline(linewidth=0.4, color='k', linestyle='-.', y = 6)

pltarr[1].plot(times2, memory_usage2)
pltarr[1].set_title("flexible ospf without plugins")    
pltarr[1].set_xlabel('time (s)')
pltarr[1].set_ylabel('memory usage (MB)')
pltarr[1].axhline(linewidth=0.4, color='k', linestyle='-.', y = 2)
pltarr[1].axhline(linewidth=0.4, color='k', linestyle='-.', y = 4)
pltarr[1].axhline(linewidth=0.4, color='k', linestyle='-.', y = 6)

pltarr[2].plot(times3, memory_usage3)
pltarr[2].set_title("flexible ospf with plugins")    
pltarr[2].set_xlabel('time (s)')
pltarr[2].set_ylabel('memory usage (MB)')
pltarr[2].axhline(linewidth=0.4, color='k', linestyle='-.', y = 2)
pltarr[2].axhline(linewidth=0.4, color='k', linestyle='-.', y = 4)
pltarr[2].axhline(linewidth=0.4, color='k', linestyle='-.', y = 6)

f.tight_layout()

plt.show()
