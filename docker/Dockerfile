#20200123 add iperf3 to measure network bandwidth as client
#20200110 remove qperf from apt and compile from source for ipv6 supporting
FROM ubuntu:18.04
ENV VERSION_QPERF=0.4.11

RUN apt-get update  

COPY . /root
WORKDIR /root

RUN apt-get install -y build-essential gnome-common gnome-icon-theme libnotify4 libunique-dev libglib2.0-dev libgtk2.0-dev libgconf2-dev libgstreamer1.0-dev libnotify-dev libxml2-dev 
