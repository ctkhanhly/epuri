server = '10.9.0.3'
attacker = '10.9.0.2'
build: simplenet simplelogger epuri.c fssl simplenoti
	mkdir -p build \
	&& cmake -S . -B build \
	&& cd build \
	&& make
open_logger:
	mkdir -p logs
run_simplenet_client: open_logger
	SIMPLENET_LOGGER_PATH='./logs/simplenet.txt' \
	epuri 'client' $(attacker) '8346'
run_simplenet_server: open_logger
	SIMPLENET_LOGGER_PATH='./logs/simplenet.txt' \
	epuri server $(attacker) 8346
run_sandboxed_client: open_logger
	SIMPLENET_LOGGER_PATH='./logs/simplenet.txt' \
	SIMPLENOTI_TRACER_LOGGER_PATH='./logs/simplenoti_tracer.txt' \
	SIMPLENOTI_SANDBOX_LOGGER_PATH='./logs/simplenoti_sandbox.txt' \
	LD_PRELOAD='libsimplenoti.so' \
	epuri 'client' $(attacker) '8346' $(attacker) '8346'
run_secure_redis_client: open_logger
	FSSL_LOGGER_PATH='./logs/fssl.txt' \
	SIMPLENET_LOGGER_PATH='./logs/simplenet.txt' \
	SIMPLENOTI_TRACER_LOGGER_PATH='./logs/simplenoti_tracer.txt' \
	SIMPLENOTI_SANDBOX_LOGGER_PATH='./logs/simplenoti_sandbox.txt' \
	LD_PRELOAD="libfssl.so libsimplenoti.so" \
	redis-cli --tls \
        --cert /etc/redis/redis-client.crt \
        --key /etc/redis/redis-client.key \
        --cacert /etc/redis/ca.crt \
        -h $(server) -p 8345 \
        $(server) 8345
run_unsecure_redis_client: open_logger
	FSSL_LOGGER_PATH='./logs/fssl.txt' \
	SIMPLENET_LOGGER_PATH='./logs/simplenet.txt' \
	LD_PRELOAD=libfssl.so \
	redis-cli --tls \
        --cert /etc/redis/redis-client.crt \
        --key /etc/redis/redis-client.key \
        --cacert /etc/redis/ca.crt \
        -h $(server) -p 8345
run_unsandboxed_secure_redis_client: open_logger
	FSSL_LOGGER_PATH='./logs/fssl.txt' \
	SIMPLENET_LOGGER_PATH='./logs/simplenet.txt' \
	LD_PRELOAD=libfssl.so \
	redis-cli --tls \
        --cert /etc/redis/redis-client-secure.crt \
        --key /etc/redis/redis-client-secure.key \
        --cacert /etc/redis/ca.crt \
        -h $(server) -p 8345
run_redis_client: open_logger
	SIMPLENET_LOGGER_PATH='./logs/simplenet.txt' \
	redis-cli --tls \
        --cert /etc/redis/redis-client.crt \
        --key /etc/redis/redis-client.key \
        --cacert /etc/redis/ca.crt \
        -h $(server) -p 8345
run_redis_server: open_logger
	SIMPLENET_LOGGER_PATH='./logs/simplenet.txt' \
	redis-server --tls-port 8345 --port 0 \
        --tls-cert-file /etc/redis/redis-server.crt \
        --tls-key-file /etc/redis/redis-server.key \
        --tls-ca-cert-file /etc/redis/ca.crt
clean:
	rm -rf build logs