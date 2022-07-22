FROM build-epuri

COPY data/redis.service /etc/systemd/system/redis.service
COPY data/redis-server.conf /etc/redis/redis.conf
COPY scripts/gen-redis-server-certs.sh \
    data/server.conf \
    tls/ca.crt \
    tls/ca.key \
    tls/ca.txt /tmp/
COPY tls/ca.crt /etc/redis

RUN cd /tmp/ \
    && chmod +x gen-redis-server-certs.sh \
    && ./gen-redis-server-certs.sh \
    && mv redis-server.key redis-server.crt /etc/redis