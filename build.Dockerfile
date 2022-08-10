FROM ubuntu:22.04 AS intall-ubuntu

RUN apt-get update && \
    apt-get -y install\
    build-essential\
    pkg-config\
    libssl-dev\
    libseccomp-dev\
    wget

# RUN apt-get update && \
#     apt-get -y install bc

RUN apt install net-tools

FROM intall-ubuntu AS intall-redis

RUN cd /tmp\
    && wget https://download.redis.io/releases/redis-7.0.3.tar.gz \
    && tar -xzvf redis-7.0.3.tar.gz \
    && cd redis-7.0.3/ \
    && make MALLOC=libc BUILD_TLS=yes

RUN cd /tmp/redis-7.0.3/src \
    && cp redis-server redis-cli /usr/local/bin/

FROM intall-redis AS intall-cmake

RUN cd /tmp \
    && wget https://github.com/Kitware/CMake/releases/download/v3.23.2/cmake-3.23.2.tar.gz \
    && tar -zxvf cmake-3.23.2.tar.gz \
    && cd cmake-3.23.2 \
    && ./bootstrap \
    && make \
    && make install \
    && cmake --version | echo

FROM intall-cmake AS intall-app

COPY apps /tmp/apps
RUN cd /tmp/apps\
    && make build\
    && cp build/epuri /usr/local/bin\
    && cp build/fssl/libfssl.so /usr/lib\
    && cp build/simplenoti/src/libsimplenoti.so /usr/lib\
    && cp build/simplenet/src/libsimplenet.so /usr/lib \
    && mkdir /apps/ && cp /tmp/apps/Makefile /apps/