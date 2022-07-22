#include <stdlib.h> // EXIT_FAILED
#include <dlfcn.h>
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <unistd.h> // getpid
#include <assert.h>
#include "common.h"
#include "sandbox.h"
#include "tracer.h"

static pid_t pid;

static int (*real_main)(int, char **, char **);
static int sock_pair[2];

__attribute__((constructor)) static void setup_simplenoti()
{
	setbuf(stdout, NULL);
    /* Create a UNIX domain socket that is used to pass the seccomp
       notification file descriptor from the target process to the tracer
       process. */

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sock_pair) == -1)
    {
        print_error("Main: socketpair: %d\n");
    }
	pid = fork();
	if(pid == 0)
	{
		sandbox_run(sock_pair);
	}
}

static int wrapper_main(int argc, char **argv, char **envp)
{
	assert(argc >= 2);
	char* expected_host = argv[argc-2];
	char* expected_port = argv[argc-1];
	argc -= 2;
	argv[argc] = NULL;
	argv[argc+1] = NULL; // prevent buffer overflow attack
    if(pid)
	{
		tracer_run(sock_pair, expected_host, expected_port);
	}
	else
	{
		int status;
		status = real_main(argc, argv, envp);
		printf("Sanbox: Done!: %d, %d\n", getpid(), getppid());
		_exit(status);
	}
    return EXIT_FAILURE;
}

int __libc_start_main(
	int (*main)(int, char **, char **),
	int argc,
	char ** ubp_av,
	void (*init)(void),
	void (*fini)(void),
	void (*rtld_fini)(void),
	void (* stack_end))
{
	void *libc_handle;
	const char *heapenv;

	int (*real_libc_start_main)(
		int (*main) (int, char **, char **),
		int argc,
		char ** ubp_av,
		void (*init)(void),
		void (*fini)(void),
		void (*rtld_fini)(void),
		void (* stack_end));

	/* Save pointers to the real init, main, destructor, and runtime loader destructor functions */
	real_main = main;

	/* explicitly open the glibc shared library */
	libc_handle = dlopen("libc.so.6", RTLD_LOCAL | RTLD_LAZY);
	if (libc_handle == 0) {
        print_error("Main: dlopen: %s\n");
		_exit(EXIT_FAILURE);
	}

	/* get a pointer to the real __libc_start_main function */
	*(void **) (&real_libc_start_main) = dlsym(libc_handle, "__libc_start_main");

	/* Delegate to the real __libc_start_main, but provide our
	 * wrapper init, main, destructor, and runtime loader destructor functions */
	return real_libc_start_main(wrapper_main, argc, ubp_av,
		init, fini, rtld_fini, stack_end);
}