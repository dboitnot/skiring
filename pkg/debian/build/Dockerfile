FROM ubuntu:22.04

RUN apt-get update \
 && apt-get install -y build-essential automake autoconf

COPY pkg/debian/build/deb/ /root/deb/
COPY pkg/debian/build/run.sh /

COPY skiring-*.tar.gz /

CMD /run.sh
