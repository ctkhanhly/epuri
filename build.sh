mkdir -p tls
chmod +x scripts/gen-ca-certs.sh
./scripts/gen-ca-certs.sh
sudo docker build -f build.Dockerfile . -t build-epuri
sudo docker build -f server.Dockerfile . -t server-epuri
sudo docker build -f client.Dockerfile . -t client-epuri
sudo docker build -f attacker.Dockerfile . -t attacker-epuri