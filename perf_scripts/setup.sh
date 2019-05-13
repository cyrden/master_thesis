apt update
apt-get install    git autoconf automake libtool make libreadline-dev texinfo    pkg-config libpam0g-dev libjson-c-dev bison flex python-pytest    libc-ares-dev python3-dev libsystemd-dev python-ipaddress python3-sphinx    install-info build-essential libsystemd-dev libsnmp-dev perl
apt-get install devscripts
apt-get install cmake
apt-get install rpm
apt-get install debhelper
apt-get install libpcre3
apt-get install libpcre3-dev

git clone https://github.com/cyrden/master_thesis.git
cd master_thesis/

#git checkout dev2_new_type_lsa
git clone https://github.com/CESNET/libyang.git
cd libyang
mkdir build; cd build
cmake -DENABLE_LYD_PRIV=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr       -D CMAKE_BUILD_TYPE:String="Release" ..
make
make install

sudo groupadd -r -g 92 frr
sudo groupadd -r -g 85 frrvty
sudo adduser --system --ingroup frr --home /var/run/frr/ \
   --gecos "FRR suite" --shell /sbin/nologin frr
sudo usermod -a -G frrvty frr

./bootstrap.sh
./configure \
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
make
sudo make install

sudo install -m 775 -o frr -g frr -d /var/log/frr
sudo install -m 775 -o frr -g frrvty -d /etc/frr
sudo install -m 640 -o frr -g frrvty tools/etc/frr/vtysh.conf /etc/frr/vtysh.conf
sudo install -m 640 -o frr -g frr tools/etc/frr/frr.conf /etc/frr/frr.conf
sudo install -m 640 -o frr -g frr tools/etc/frr/daemons.conf /etc/frr/daemons.conf
sudo install -m 640 -o frr -g frr tools/etc/frr/daemons /etc/frr/daemons

sudo install -m 644 tools/frr.service /etc/systemd/system/frr.service
sudo systemctl enable frr

apt-get install clang-6.0
