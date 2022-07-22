#include <errno.h>
#include <stdio.h>
#include <sys/uio.h> /* process_vm_readv */
#include <seccomp.h> /* seccomp_notif */
#include <linux/limits.h> /* PATH_MAX */
#include <stdlib.h> /* EXIT_SUCCESS */
#include <sys/stat.h>
#include <unistd.h> /* getpid */
#include <netdb.h> /* getaddrinfo, NI_MAXHOST, NI_MAXSERV */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h> /* O_RDONLY, O_CLOEXEC */
#include <sys/syscall.h> /* Definition of SYS_* constants */
#include <signal.h>
#include <arpa/inet.h>
#include "scm_functions.h"
#include "common.h"
#include "tracer.h"

#define TRACER_ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
extern simplelogger_t simplenoti_logger;

static void handler(int sig)
{
    simplelogger_log(&simplenoti_logger, "Tracer: Caught signal: %d\n", sig);
    simplelogger_close(&simplenoti_logger);
    _exit(EXIT_SUCCESS);
}

static void cleanup(struct seccomp_notif* req, struct seccomp_notif_resp* resp)
{
    seccomp_notify_free(req, resp);
    simplelogger_close(&simplenoti_logger);
}

static int check_noti_fd(int notifyFd, struct seccomp_notif *req)
{
    int result = seccomp_notify_id_valid(notifyFd, req->id);
    if(result != 0)
    {
        simplelogger_log(&simplenoti_logger, "Tracer: My parent seems to be dead, bye!: %d\n", result);
    }
    return result;
}

static int get_remote_buf(pid_t remote_pid, void* buf, size_t buf_len, __u64 remote_buf_addr)
{
    struct iovec local[1];
    struct iovec remote[1];
    ssize_t nread;

    local[0].iov_base = buf;
    local[0].iov_len = buf_len;
    remote[0].iov_base = (void *) remote_buf_addr;
    remote[0].iov_len = buf_len;

    nread = process_vm_readv(remote_pid, local, 1, remote, 1, 0);
    if (nread == -1)
    {
        simplelogger_log_error(&simplenoti_logger, "read: %s\n");
        return -1;
    }
    else if (nread == 0) {
        simplelogger_log(&simplenoti_logger, "Tracer: read returned EOF\n");
        return -1;
    }
    return nread;
}

static void monitor(int notifyFd, char* expected_host, char* expected_port)
{
    int result;
    char path[PATH_MAX];
    char buf[BUF_SIZE];
    struct seccomp_notif *req;
    struct seccomp_notif_resp *resp;
    struct sockaddr saddr;
    struct sockaddr_in *sin;
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
    static struct seccomp_notif_sizes sizes = { 0, 0, 0 };
    syscall(__NR_seccomp, SECCOMP_GET_NOTIF_SIZES, 0, &sizes);

    seccomp_notify_alloc(&req, &resp);

    while(1)
    {
        simplelogger_log(&simplenoti_logger, "Tracer: Waiting...\n");
        memset(req, 0, sizes.seccomp_notif);
        result = seccomp_notify_receive(notifyFd, req);
        if(result < 0)
        {
            result = check_noti_fd(notifyFd, req);
            if(result < 0) goto DeadSandbox;
            goto FailedMonitor;
        }

        if(strcmp(seccomp_syscall_resolve_num_arch(req->data.arch, req->data.nr), "connect") == 0)
        {
            simplelogger_log(&simplenoti_logger, "Tracer: connect is called: %lld, %lld, %lld\n", req->data.args[0], req->data.args[1], req->data.args[2]);
            result = get_remote_buf(req->pid, (void*) &saddr, sizeof(saddr), req->data.args[1]);
            if(result < 0)
                goto FailedMonitor;
            sin = (struct sockaddr_in *)&saddr;
            inet_ntop(AF_INET, &sin->sin_addr, ip, sizeof (ip));
            port = htons (sin->sin_port);
            simplelogger_log(&simplenoti_logger, "Tracer: Connection to: %s:%d\n", ip, port);
            resp->id = req->id;
            if(strlen(ip) == strlen(expected_host) && strcmp(ip, expected_host) == 0
                && port == atoi(expected_port))
            {
                resp->val = 0;
                resp->error = 0;
                resp->flags = SECCOMP_USER_NOTIF_FLAG_CONTINUE;
                simplelogger_log(&simplenoti_logger, "Tracer: Accepting\n");
            }
            else
            {
                resp->val = -1;
                resp->id = req->id;
                resp->error = -EPERM;
                resp->flags = 0;
                simplelogger_log(&simplenoti_logger, "Tracer: Rejecting\n");
            }
            result = seccomp_notify_respond(notifyFd, resp);
            if(result < 0)
                goto FailedMonitor;
        }
        else
        {
            simplelogger_log(&simplenoti_logger, "Tracer: Unexpected notifier for system call: %s\n", seccomp_syscall_resolve_num_arch(req->data.arch, req->data.nr));
            goto FailedMonitor;
        }
    }
FailedMonitor:
    simplelogger_log(&simplenoti_logger, "Tracer: Failed monitor\n");
    cleanup(req, resp);
    exit(EXIT_FAILURE);
DeadSandbox:
    cleanup(req, resp);
    exit(EXIT_SUCCESS);
}

void tracer_run(int sock_pair[2], char* expected_host, char* expected_port)
{
    simplelogger_init(&simplenoti_logger, getenv("SIMPLENOTI_TRACER_LOGGER_PATH"));
    simplelogger_log(&simplenoti_logger, "Tracer: %d\n", getpid());
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        simplelogger_log(&simplenoti_logger, "Tracer: sigaction SIGCHLD\n");
    }
    int notifyFd = recvfd(sock_pair[1]);
    if (notifyFd == -1)
    {
        simplelogger_log_error(&simplenoti_logger, "recvfd: %s\n");
        exit(EXIT_FAILURE);
    }
    close_socket_pair(sock_pair);
    monitor(notifyFd, expected_host, expected_port);
}