FROM build-epuri

COPY data/redis-client.conf /etc/redis/redis.conf
COPY scripts/gen-redis-client-certs.sh \
    data/client.conf \
    data/client-secure.conf \
    tls/ca.crt \
    tls/ca.key \
    tls/ca.txt /tmp/
COPY tls/ca.crt /etc/redis

RUN cd /tmp \
    && chmod +x gen-redis-client-certs.sh \
    && ./gen-redis-client-certs.sh \
    && mv redis-client.key redis-client.crt /etc/redis \
    && mv redis-client-secure.key redis-client-secure.crt /etc/redis \
    && mv /tmp/apps/profile.sh /apps/ \
    && mv /tmp/apps/profile_sandboxed.sh /apps/