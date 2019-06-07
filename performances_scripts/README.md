PERFORMANCES SCRIPTS
====================

This folder contains script and files that have been used to test the performances of our solution.

# Reproduce performances graphs
To reproduce the performances graphs showed in the master thesis manuscript, you can use the scripts in the generation\_figures folder. These two python script will use the provided logs file to produce the graphs.

# Useful scripts

### Generating a IPMIninet topology file
To generate an IPMininet script for a given topology you just need to provide the graphML\_to\_IPMininet.py script with a .graphml file that describes your topology. Use it as follows:
```
python graphML\_to\_IPmininet.py -f 'your\_file'
``` 
GtsCe.graphml.xml is an example of a graphml topology file.
GTS_CE-IPMininet.py is the file produced by the script for this topology.

### Simulate a topology of OSPF routers with IPMininet
To simulate a topology you just need to run an IPMininet script.
You need to install Mininet and IPMininet:
```
sudo apt-get install mininet
sudo pip install ipmininet
```
IPMininet is looking in PATH to find the daemons executable file. You thus must not forget to copy them (zebra and ospfd) in /usr/bin before running the IPMininet script.
