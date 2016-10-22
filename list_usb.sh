#!/bin/bash

FILE="$1"

if [ "$1" = "" ]
then
    echo "$0 dest" &1>2
    exit -1
fi

source /usr/bin/usb-devices > /dev/null

printf "Choose your usb devices for auhentification\n"

print_device() {
	local devpath=$1
	local parent=$2
	local level=$3
	local count=$4

	[ -d $devpath ] || return
	cd $devpath

	local busnum=`cat busnum`
	local devnum=`cat devnum`

	if [ $level -gt 0 ]; then
		port=$((${devpath##*[-.]} - 1))
	else
		port=0
	fi
	speed=`cat speed`
	maxchild=`cat maxchild`
	printf "\nT:  Bus=%02i Lev=%02i Prnt=%02i Port=%02i Cnt=%02i Dev#=%3i Spd=%-3s MxCh=%2i\n" \
		$busnum $level $parent $port $count $devnum $speed $maxchild

	ver=`cat version`
	devclass=`cat bDeviceClass`
	devsubclass=`cat bDeviceSubClass`
	devprotocol=`cat bDeviceProtocol`
	maxps0=`cat bMaxPacketSize0`
	numconfigs=`cat bNumConfigurations`
	classname=`class_decode $devclass`
	printf "D:  Ver=%5s Cls=%s(%s) Sub=%s Prot=%s MxPS=%2i #Cfgs=%3i\n" \
		$ver $devclass "$classname" $devsubclass $devprotocol \
		$maxps0 $numconfigs

	vendid=`cat idVendor`
	prodid=`cat idProduct`
	revmajor=`cat bcdDevice | cut -c 1-2`
	revminor=`cat bcdDevice | cut -c 3-4`
	printf "P:  Vendor=%s ProdID=>%s< Rev=%s.%s\n" \
		$vendid $prodid $revmajor $revminor

	print_string manufacturer "Manufacturer"
	print_string product Product
	print_string serial SerialNumber

	numifs=`cat bNumInterfaces`
	cfgnum=`cat bConfigurationValue`
	attr=`cat bmAttributes`
	maxpower=`cat bMaxPower`
	printf "C:  #Ifs=%2i Cfg#=%2i Atr=%s MxPwr=%s\n" \
		$numifs $cfgnum $attr $maxpower

	# There's not really any useful info in endpoint 00
	#print_endpoint $devpath/ep_00

	printf "Use this device ? (y/n)\n"
	read answer

	if [ "$answer" = "Y" ] || [ "$answer" = "y" ]
	then
	    echo "$vendid:$prodid" > $FILE
	    exit 0;
	fi

	
	local devcount=0
	for subdev in $busnum-*
	do
		echo "$subdev" | grep -Eq "^$busnum-[0-9]+(\.[0-9]+)*$" \
			|| continue

		devcount=$(($devcount + 1))
		if [ -d $devpath/$subdev ]; then
			print_device $devpath/$subdev \
				$devnum $(($level +1)) $devcount
		fi
	done
}


for device in /sys/bus/usb/devices/usb*/
do

    print_device $device 0 0 0
done
