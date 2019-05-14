sudo cp /usr/lib/frr/zebra /usr/bin/zebra
sudo cp /usr/lib/frr/ospfd /usr/bin/ospfd

cd ospfd/plugins
sudo make
cd ..
sudo cp -r plugins /
cd ..

sudo /usr/bin/zebra -d -f /etc/frr/zebra.conf -i /tmp/zebra.pid -z /tmp/zebra.api -u root -k
sleep 1
sudo /usr/bin/ospfd -d -f /etc/frr/ospfd.conf -i /tmp/ospfd.pid -z /tmp/zebra.api -u root
