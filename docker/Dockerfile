#20200123 add iperf3 to measure network bandwidth as client
#20200110 remove qperf from apt and compile from source for ipv6 supporting
FROM ubuntu:18.04
ENV VERSION_QPERF=0.4.11

RUN apt-get update  \
    && apt-get install -y busybox wget curl net-tools tcpdump jq ioping iperf fio sysbench stress-ng libmoosex-role-timer-perl make gcc unzip iperf3 vim \
    && cd root && wget https://github.com/kdlucas/byte-unixbench/archive/v5.1.3.tar.gz \
    && tar xvfz v5.1.3.tar.gz && rm v5.1.3.tar.gz && cd byte-unixbench-5.1.3/UnixBench \
    && make all && cd /root && mv byte-unixbench-5.1.3/UnixBench unixbench && rm -rf byte-unixbench-5.1.3 \  
    && echo "ioping . -c 10" > ioping.sh && chmod a+x ioping.sh \
    && apt-get install -y automake autoconf m4 perl libtool && cd /root && wget https://github.com/linux-rdma/qperf/archive/v$VERSION_QPERF.tar.gz && tar xvfz v$VERSION_QPERF.tar.gz && rm v$VERSION_QPERF.tar.gz \
    && cd qperf-$VERSION_QPERF && ./autogen.sh && ./configure && make && make install && cd .. && rm -rf qperf-$VERSION_QPERF

RUN mkdir -p /usr/local/bin && wget -O /usr/local/bin/busybox https://busybox.net/downloads/binaries/1.28.1-defconfig-multiarch/busybox-x86_64; chmod a+x /usr/local/bin/busybox && mkdir /tool; cp /usr/local/bin/busybox /tool/

COPY . /root
WORKDIR /root

RUN apt-get install -y build-essential gnome-common gnome-icon-theme libnotify4 libunique-dev libglib2.0-dev libgtk2.0-dev libgconf2-dev libgstreamer1.0-dev libnotify-dev libxml2-dev 
