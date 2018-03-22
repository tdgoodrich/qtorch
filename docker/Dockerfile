FROM debian:stretch
ADD . .
RUN apt-get update && apt-get -y install build-essential
WORKDIR nlopt-2.4.2/
RUN ./configure && \
    make && \
    make install
WORKDIR /..
RUN mkdir -p /root/usr/local/bin && \
    make cut

FROM debian:stretch
COPY --from=0 /bin/maxcutQAOA /maxcutQAOA
ADD /scripts/run.sh run.sh
RUN chmod +x run.sh && \
    mkdir /angles/ && \
    mkdir /graphs/
