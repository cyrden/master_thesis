Flexible OSPF implementation
============================

This repository contains a flexible implementation of OSPF. The starting point of this work was the FRRouting suite (https://github.com/FRRouting/frr) commit e6ee7eb9e34449fa2064b8541007b64b21c7bb81. This commit correponds to version 6.0.2 of FRR. 

The following of this README contains instruction to compile and run the flexible OSPF implementation. /!\ I advise to do all this in a virtual machine.

If you encounter troubles to run the implementation feel free to send me an e-mail: cyril.denos@student.uclouvain.be

In folder 'performances\_scripts' you can find the scripts and logs file to reproduce the graphs in the manuscript. There are also script tu run some IPMininet topologies to reproduce the emulated networks. See associated README for more information.

# Compiling the implementation

### Installing needed packages

```
sudo apt-get update
sudo apt-get install    git autoconf automake libtool make libreadline-dev texinfo    pkg-config libpam0g-dev libjson-c-dev bison flex python-pytest    libc-ares-dev python3-dev libsystemd-dev python-ipaddress python3-sphinx    install-info build-essential libsystemd-dev libsnmp-dev perl
sudo apt-get install devscripts
sudo apt-get install cmake
sudo apt-get install rpm
sudo apt-get install debhelper
sudo apt-get install libpcre3
sudo apt-get install libpcre3-dev
sudo apt-get install clang-6.0
sudo apt-get install cmake
```

### Install libyang

```
git clone https://github.com/CESNET/libyang.git
cd libyang
mkdir build; cd build
cmake -DENABLE_LYD_PRIV=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr       -D CMAKE_BUILD_TYPE:String="Release" ..
sudo make
sudo make install
```

### Add FRR and user group

```
sudo groupadd -r -g 92 frr
sudo groupadd -r -g 85 frrvty
sudo adduser --system --ingroup frr --home /var/run/frr/ \
   --gecos "FRR suite" --shell /sbin/nologin frr
sudo usermod -a -G frrvty frr
```

### Clone repo and compile implementation

```
git clone https://github.com/cyrden/master_thesis.git
cd master_thesis/
sudo ./bootstrap.sh
sudo ./configure \
    --prefix=/usr \
    --includedir=\${prefix}/include \
    --enable-exampledir=\${prefix}/share/doc/frr/examples \
    --bindir=\${prefix}/bin \
    --sbindir=\${prefix}/lib/frr \
    --libdir=\${prefix}/lib/frr \
    --libexecdir=\${prefix}/lib/frr \
    --localstatedir=/var/run/frr \
    --sysconfdir=/etc/frr \
    --with-moduledir=\${prefix}/lib/frr/modules \
    --with-libyang-pluginsdir=\${prefix}/lib/frr/libyang_plugins \
    --enable-configfile-mask=0640 \
    --enable-logfile-mask=0640 \
    --enable-snmp=agentx \
    --enable-multipath=64 \
    --enable-user=frr \
    --enable-group=frr \
    --enable-vty-group=frrvty \
    --with-pkg-git-version \
    --with-pkg-extra-version=-MyOwnFRRVersion
sudo make
sudo make install
```

### Create FRR config files

```
sudo install -m 775 -o frr -g frr -d /var/log/frr
sudo install -m 775 -o frr -g frrvty -d /etc/frr
sudo install -m 640 -o frr -g frrvty tools/etc/frr/vtysh.conf /etc/frr/vtysh.conf
sudo install -m 640 -o frr -g frr tools/etc/frr/frr.conf /etc/frr/frr.conf
sudo install -m 640 -o frr -g frr tools/etc/frr/ospfd.conf /etc/frr/ospfd.conf
sudo install -m 640 -o frr -g frr tools/etc/frr/zebra.conf /etc/frr/zebra.conf
sudo install -m 640 -o frr -g frr tools/etc/frr/daemons.conf /etc/frr/daemons.conf
sudo install -m 640 -o frr -g frr tools/etc/frr/daemons /etc/frr/daemons

sudo install -m 644 tools/frr.service /etc/systemd/system/frr.service
```


### Compile plugins
```
cd ospfd/plugins
sudo make
```
If you want to use the provided manifest example to inject plugins, you must copy the plugins/ directory at /
```
sudo cp -r ospfd/plugins /
``` 

# Router configuration
To run OSPF, ZEBRA (IP routing manager) must also be running. Both daemons must be configured using configurations files. The following are sample configuration files for both daemons. Of course you must replace the addresses with addresses of the machine.

/etc/frr/ospfd.conf
```
hostname test-router
!
router ospf
 ospf router-id 1.1.1.1
 network 130.104.229.24/25 area 0
!
line vty
!
log file /var/log/frr/ospfd.log debugging
```

/etc/frr/zebra.conf
```
hostname test-router
log syslog informational

!
interface eth-0
 ip address 130.104.229.24/25
!
```

# Running the OSPF implementation
It is possible to run FRRouting using systemd. Here, we propose to launch all the daemons manually for more control on what happens.

### Copy the executables in PATH
```
sudo cp /usr/lib/frr/zebra /usr/bin/zebra
sudo cp /usr/lib/frr/ospfd /usr/bin/ospfd
```

### Run zebra and ospf daemons
```
sudo /usr/bin/zebra -d -f /etc/frr/zebra.conf -i /tmp/zebra.pid -z /tmp/zebra.api -u root -k
sudo /usr/bin/ospfd -d -f /etc/frr/ospfd.conf -i /tmp/ospfd.pid -z /tmp/zebra.api -u root
```
If you encounter problems with group membership add your user to frrvty group
```
sudo usermod -a -G frrvty username
```

You can use 'sudo vtysh' to enter the CLI of the router.
Some basic commands:
```
show ip ospf database // see the LSDB of the router
show ip ospf route // see the routes
show ip ospf interface // see the OSPF enabled interfaces
...
```

### Injecting plugins in OSPF
To inject plugin in OSPF, you must run the user CLI
```
./ospfd/plugins_manager/plugins_ui
```
Then you can input the path to your manifesto file. A sample file is available in ospfd/plugins/manifesto\_example.txt

If the monitoring server is launched, you should see some message that arrive when the bytecodes from the manifest file are injected. 

### Run the monitoring server
To run the monitoring server (monitoring are sending some datas to it) use the following command
```
./ospfd/monitoring_server/monitoring_server
```
