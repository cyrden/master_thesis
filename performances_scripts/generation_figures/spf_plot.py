#!/usr/bin/env python

import matplotlib
matplotlib.use('Agg')
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

import re
from argparse import ArgumentParser as ap
import matplotlib.pyplot as plt
from statistics import mean,median

parser = ap(description='Plot performances')
#parser.add_argument('-f1', '--file1', required=True, help="Filepath of the json file to be read")
#parser.add_argument('-f2', '--file2', required=True, help="Filepath of the json file to be read")
#parser.add_argument('-f3', '--file3', required=True, help="Filepath of the json file to be read")
#parser.add_argument('-f4', '--file4', required=True, help="Filepath of the json file to be read")
#parser.add_argument('-f5', '--file5', required=True, help="Filepath of the json file to be read")
#args = parser.parse_args()

times_tot_1 = []
times_spf_1 = []
times_route_install_1 = []

times_tot_2 = []
times_spf_2 = []
times_route_install_2 = []

times_tot_3 = []
times_spf_3 = []
times_route_install_3 = []

times_tot_4 = []
times_spf_4 = []
times_route_install_4 = []

times_tot_5 = []
times_spf_5 = []
times_route_install_5 = []

regex_tot_time = r"SPF Processing Time\(usecs\): (?P<time_tot>\d+)"
regex_time_spf = r"SPF Time: (?P<time_spf>\d+)"
regex_mytime_spf = r"Time for SPF function: (?P<time_spf>\d+)"
regex_time_route_install = r"RouteInstall: (?P<time_route_install>\d+)"
regex_good_dijkstra = r"ospf_spf_calculate: Stop. 312 vertices"

good_dijkstra = False

#f1 = open(args.file1, "r")
f1 = open('spf_logs/vanillafrr.log', "r")
for line in f1:
  match = re.search(regex_good_dijkstra, line)
  if(match != None):
    good_dijkstra = True
  if(good_dijkstra):
    match1 = re.search(regex_tot_time, line)
    if(match1 != None):
      time_tot = match1.group('time_tot')
      #if(int(time_tot) > 1000 and int(time_tot) < 1000000):
      times_tot_1.append(float(time_tot))
   
    match2 = re.search(regex_time_spf, line)
    if(match2 != None):
      time_spf = match2.group('time_spf')
      #if(int(time_spf) > 1000 and int(time_spf) < 1000000):
      times_spf_1.append(float(time_spf))

    match3 = re.search(regex_time_route_install, line)
    if(match3 != None):
      time_route_install = match3.group('time_route_install')
      #if(int(time_route_install) > 500 and int(time_route_install) < 1000000):
      times_route_install_1.append(float(time_route_install))
      good_dijkstra = False

#f2 = open(args.file2, "r")
f2 = open('spf_logs/myfrr_noplugins.log', "r")
for line in f2:
  match = re.search(regex_good_dijkstra, line)
  if(match != None):
    good_dijkstra = True
  if(good_dijkstra):
    match1 = re.search(regex_tot_time, line)
    if(match1 != None):
      time_tot = match1.group('time_tot')
      #if(int(time_tot) > 1000 and int(time_tot) < 1000000):
      times_tot_2.append(float(time_tot))
   
    match2 = re.search(regex_time_spf, line)
    if(match2 != None):
      time_spf = match2.group('time_spf')
      #if(int(time_spf) > 1000 and int(time_spf) < 1000000):
      times_spf_2.append(float(time_spf))

    match3 = re.search(regex_time_route_install, line)
    if(match3 != None):
      time_route_install = match3.group('time_route_install')
      #if(int(time_route_install) > 500 and int(time_route_install) < 1000000):
      times_route_install_2.append(float(time_route_install))
      good_dijkstra = False

#f3 = open(args.file3, "r")
f3 = open('spf_logs/myfrr_plugin_mon.log', "r")
for line in f3:
  match = re.search(regex_good_dijkstra, line)
  if(match != None):
    good_dijkstra = True
  if(good_dijkstra):
    match1 = re.search(regex_tot_time, line)
    if(match1 != None):
      time_tot = match1.group('time_tot')
      #if(int(time_tot) > 1000 and int(time_tot) < 1000000):
      times_tot_3.append(float(time_tot))
   
    match2 = re.search(regex_time_spf, line)
    if(match2 != None):
      time_spf = match2.group('time_spf')
      #if(int(time_spf) > 1000 and int(time_spf) < 1000000):
      times_spf_3.append(float(time_spf))

    match3 = re.search(regex_time_route_install, line)
    if(match3 != None):
      time_route_install = match3.group('time_route_install')
      #if(int(time_route_install) > 500 and int(time_route_install) < 1000000):
      times_route_install_3.append(float(time_route_install))
      good_dijkstra = False

#f4 = open(args.file4, "r")
f4 = open('spf_logs/myfrr_plugin_dijkstra_interpreted.log', "r")
for line in f4:
  match = re.search(regex_good_dijkstra, line)
  if(match != None):
    good_dijkstra = True
  if(good_dijkstra):
    match1 = re.search(regex_tot_time, line)
    if(match1 != None):
      time_tot = match1.group('time_tot')
      #if(int(time_tot) > 1000 and int(time_tot) < 1000000):
      times_tot_4.append(float(time_tot))
   
    match2 = re.search(regex_time_spf, line)
    if(match2 != None):
      time_spf = match2.group('time_spf')
      #if(int(time_spf) > 1000 and int(time_spf) < 1000000):
      times_spf_4.append(float(time_spf))

    match3 = re.search(regex_time_route_install, line)
    if(match3 != None):
      time_route_install = match3.group('time_route_install')
      #if(int(time_route_install) > 500 and int(time_route_install) < 1000000):
      times_route_install_4.append(float(time_route_install))
      good_dijkstra = False

#f5 = open(args.file5, "r")
f5 = open('spf_logs/myfrr_plugin_dijkstra_jit.log', "r")
for line in f5:
  match = re.search(regex_good_dijkstra, line)
  if(match != None):
    good_dijkstra = True
  if(good_dijkstra):
    match1 = re.search(regex_tot_time, line)
    if(match1 != None):
      time_tot = match1.group('time_tot')
      #if(int(time_tot) > 1000 and int(time_tot) < 1000000):
      times_tot_5.append(float(time_tot))
   
    match2 = re.search(regex_time_spf, line)
    if(match2 != None):
      time_spf = match2.group('time_spf')
      #if(int(time_spf) > 1000 and int(time_spf) < 1000000):
      times_spf_5.append(float(time_spf))

    match3 = re.search(regex_time_route_install, line)
    if(match3 != None):
      time_route_install = match3.group('time_route_install')
      #if(int(time_route_install) > 500 and int(time_route_install) < 1000000):
      times_route_install_5.append(float(time_route_install))
      good_dijkstra = False

#print(len(times_spf_1))
#print(len(times_spf_2))
#print(len(times_spf_3))
#print(len(times_spf_4))
#print(len(times_spf_5))
#print(median(times_spf_1))
#print(median(times_spf_2))
#print(median(times_spf_3))
#print(median(times_route_install_1), mean(times_route_install_1))
#print(median(times_route_install_2), mean(times_route_install_2))
#print(median(times_route_install_3), mean(times_route_install_3))
#print(times_spf_1)
#print(times_route_install_1)

times_tot_1[:] = [x/1000 for x in times_tot_1]
times_spf_1[:] = [x/1000 for x in times_spf_1]
times_route_install_1[:] = [x/1000 for x in times_route_install_1]

times_tot_2[:] = [x/1000 for x in times_tot_2]
times_spf_2[:] = [x/1000 for x in times_spf_2]
times_route_install_2[:] = [x/1000 for x in times_route_install_2]

times_tot_3[:] = [x/1000 for x in times_tot_3]
times_spf_3[:] = [x/1000 for x in times_spf_3]
times_route_install_3[:] = [x/1000 for x in times_route_install_3]

times_tot_4[:] = [x/1000 for x in times_tot_4]
times_spf_4[:] = [x/1000 for x in times_spf_4]
times_route_install_4[:] = [x/1000 for x in times_route_install_4]

times_tot_5[:] = [x/1000 for x in times_tot_5]
times_spf_5[:] = [x/1000 for x in times_spf_5]
times_route_install_5[:] = [x/1000 for x in times_route_install_5]

medianprops = dict(linestyle='-', linewidth=1.5)

plt.figure(figsize=(5.2,3.6))
plt.boxplot([times_spf_1, times_spf_2, times_spf_3, times_spf_4, times_spf_5], patch_artist = True, showfliers=False, medianprops=medianprops)
plt.title("SPF time")
plt.ylabel("time (ms)")
plt.xticks([1, 2, 3, 4, 5], ['vanilla', 'pluginized\nno plugins', 'pluginized\nmonitor SPF\ninterpreted', 'pluginized\nmodify SPF\ninterpreted', 'pluginized\nmodify SPF\njitted'])
plt.ylim(bottom=0)
plt.axhline(linewidth=0.4, color='k', linestyle='-.', y = median(times_spf_1))
# plt.show()
plt.tight_layout()
plt.savefig("spf_boxplot.png")

f, pltarr = plt.subplots(3)

bp1 = pltarr[0].boxplot([times_tot_1, times_tot_2, times_tot_3, times_tot_4, times_tot_5], patch_artist = True, showfliers=False, medianprops=medianprops)
pltarr[0].set_title("(a) SPF total processing time")
pltarr[0].set_ylabel("time (ms)")
pltarr[0].set_xticklabels(['vanilla ospf', 'flexible ospf without \nplugins', 'flexible ospf with \nplugin monitor dijkstra \ninterpreted', 'flexible ospf with \nplugin modify dijkstra \ninterpreted', 'flexible ospf with \n plugin modify dijktra \njitted'])

bp2 = pltarr[1].boxplot([times_spf_1, times_spf_2, times_spf_3, times_spf_4, times_spf_5], patch_artist = True, showfliers=False, medianprops=medianprops)
pltarr[1].set_title("(b) SPF time")
pltarr[1].set_ylabel("time (ms)")
pltarr[1].set_xticklabels(['vanilla ospf', 'flexible ospf without \nplugins', 'flexible ospf with \nplugin monitor dijkstra \ninterpreted', 'flexible ospf with \nplugin modify dijkstra \ninterpreted', 'flexible ospf with \n plugin modify dijktra \njitted'])

bp3 = pltarr[2].boxplot([times_route_install_1, times_route_install_2, times_route_install_3, times_route_install_4, times_route_install_5], patch_artist = True, showfliers=False, medianprops=medianprops)
pltarr[2].set_title("(c) Route install time")
pltarr[2].set_ylabel("time (ms)")
pltarr[2].set_xticklabels(['vanilla ospf', 'flexible ospf without \nplugins', 'flexible ospf with \nplugin monitor dijkstra \ninterpreted', 'flexible ospf with \nplugin modify dijkstra \ninterpreted', 'flexible ospf with \n plugin modify dijktra \njitted'])

f.tight_layout()
plt.show()



