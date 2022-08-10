#include <seccomp.h>
#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCESS
#include <signal.h>
#include <unistd.h> // getpid
#include <sys/types.h>
#include <sys/stat.h> // S_IRWXU
#include "common.h"
#include "sandbox.h"
#include "scm_functions.h"

#define SANDBOX_ALLOW_RULE(call) ({ \
	int allow_rule_syscall_number = seccomp_syscall_resolve_name (#call); \
	if (allow_rule_syscall_number == __NR_SCMP_ERROR || \
	    seccomp_rule_add (ctx, SCMP_ACT_ALLOW, allow_rule_syscall_number, 0) < 0) \
		goto Failed; \
})

extern simplelogger_t simplenoti_logger;

static void handler(int sig)
{
    printf("Sandbox: Caught signal\n");
    _exit(EXIT_SUCCESS);
}

static int configure_seccomp(int sockPair[2])
{
    int rc;
    scmp_filter_ctx ctx;
	
    ctx = seccomp_init(SCMP_ACT_ALLOW);
    rc = seccomp_rule_add(ctx, SCMP_ACT_NOTIFY,  SCMP_SYS(connect), 0);
    if(rc < 0)
        goto Failed;
    rc = seccomp_rule_add(ctx, SCMP_ACT_KILL_PROCESS,  SCMP_SYS(fork), 0);
    if(rc < 0)
        goto Failed;
    rc = seccomp_rule_add(ctx, SCMP_ACT_KILL_PROCESS,  SCMP_SYS(vfork), 0);
    if(rc < 0)
        goto Failed;
    rc = seccomp_rule_add(ctx, SCMP_ACT_KILL_PROCESS,  SCMP_SYS(clone), 0);
    if(rc < 0)
        goto Failed;
    if (seccomp_load (ctx) >= 0) {
        simplelogger_log(&simplenoti_logger, "Sandbox: Loadded seccomp rules\n");
        int notifyFd = seccomp_notify_fd(ctx);
        simplelogger_log(&simplenoti_logger, "Sandbox: NotifyFd: %d\n", notifyFd);
        if (sendfd(sockPair[0], notifyFd) == -1)
        {
            simplelogger_log_error(&simplenoti_logger, "sendfd: %s\n");
            seccomp_release (ctx);
            _exit(EXIT_FAILURE);
        }
        if (close(notifyFd) == -1)
        {
            simplelogger_log_error(&simplenoti_logger, "close-target-notify-fd: %s\n");
        }
        if(close_socket_pair(sockPair) == -1)
            goto Failed;
        simplelogger_log(&simplenoti_logger, "Sandbox: Loadded seccomp rules\n");
        
		seccomp_release (ctx);
		return 0;
	}
    
Failed:
    simplelogger_log(&simplenoti_logger, "Sandbox: Cannot load seccomp rules\n");
    seccomp_release (ctx);
    return -1;
}

pid_t sandbox_run(int sock_pair[2])
{
    simplelogger_init(&simplenoti_logger, getenv("SIMPLENOTI_SANDBOX_LOGGER_PATH"));
    simplelogger_log(&simplenoti_logger, "Sandbox: %d\n", getpid());
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        simplelogger_log(&simplenoti_logger, "Sandbox: sigaction SIGTERM\n");
    }
    if (sigaction(SIGSYS, &sa, NULL) == -1)
    {
        simplelogger_log(&simplenoti_logger, "Sandbox: sigaction SIGSYS\n");
    }
    if(configure_seccomp(sock_pair) == -1)
    {
        simplelogger_log(&simplenoti_logger, "Sandbox: failed to configure seccomp\n");
    }
    
}