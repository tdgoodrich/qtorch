FROM debian:stretch
ADD . .
ADD scripts/run.sh /run.sh
RUN apt-get update && apt-get -y install build-essential vim gdb
WORKDIR nlopt-2.4.2/
RUN ./configure && \
    make && \
    make install
WORKDIR /..
RUN mkdir -p /root/usr/local/bin && \
    make cut && \
    chmod +x run.sh && \
    mkdir /angles/ && \
    mkdir /graphs/
