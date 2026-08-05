/* revsh - reverse shell with a REAL pty.
 *
 * AMIX (guest) connects OUT to a listener on the Mac (host), allocates a STREAMS
 * pseudo-terminal via /dev/ptmx, runs bash on the slave side (its controlling
 * tty), and shuttles bytes between the socket and the pty master. bash then sees
 * a genuine terminal: job control, history, arrow keys, vi and curses all work.
 *
 * This is the canonical revsh. It does NOT use script(1) -- over the socket on
 * this system script fails to keep a shell alive. If /dev/ptmx is ever
 * unavailable, revsh-nopty.c is the dumb-but-reliable `bash -i` fallback (a
 * prompt and command execution, but no job control or line editing).
 *
 * For a clean session the Mac side should be raw too, so keystrokes pass through
 * byte-for-byte and the guest pty owns the echo:
 *     while : ; do stty raw -echo; nc -l 4444; stty sane; done
 *
 * Build on the guest:  cc -o revsh revsh.c -lsocket -lnsl
 * Install:             cp revsh /usr/local/sbin/revsh
 * Verify:              connect, then `tty` should print /dev/pts/N
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stropts.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HOST "10.0.2.2"          /* the Mac, via slirp */
#define PORT 4444
#define BASH "/usr/local/bin/bash"

extern char *ptsname();          /* <stdlib.h> may not prototype these on 2.1 */

int main()
{
	int s, mfd, sfd, n;
	struct sockaddr_in sa;
	struct pollfd pfd[2];
	char *slave, buf[1024];
	pid_t pid;

	/* connect out to the Mac */
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) { perror("socket"); exit(1); }
	memset(&sa, 0, sizeof sa);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr.s_addr = inet_addr(HOST);
	if (connect(s, (struct sockaddr *)&sa, sizeof sa) < 0) {
		perror("connect"); exit(1);
	}
	write(s, "AMIX reverse shell up\n", 22);

	/* allocate a STREAMS pseudo-terminal */
	mfd = open("/dev/ptmx", O_RDWR);
	if (mfd < 0)            { perror("open /dev/ptmx"); exit(1); }
	if (grantpt(mfd) < 0)   { perror("grantpt");        exit(1); }
	if (unlockpt(mfd) < 0)  { perror("unlockpt");       exit(1); }
	slave = ptsname(mfd);
	if (slave == 0)         { perror("ptsname");        exit(1); }

	pid = fork();
	if (pid < 0) { perror("fork"); exit(1); }

	if (pid == 0) {
		/* child: own session; first tty opened becomes the controlling one */
		setsid();
		sfd = open(slave, O_RDWR);
		if (sfd < 0) { perror("open slave"); _exit(1); }
		ioctl(sfd, I_PUSH, "ptem");     /* terminal emulation module */
		ioctl(sfd, I_PUSH, "ldterm");   /* line discipline */
		close(mfd);
		dup2(sfd, 0);
		dup2(sfd, 1);
		dup2(sfd, 2);
		if (sfd > 2) close(sfd);
		close(s);
		putenv("TERM=vt100");
		execl(BASH, "bash", "-i", (char *)0);
		perror("exec bash");
		_exit(1);
	}

	/* parent: pump bytes between the socket and the pty master */
	pfd[0].fd = s;   pfd[0].events = POLLIN;
	pfd[1].fd = mfd; pfd[1].events = POLLIN;
	for (;;) {
		if (poll(pfd, 2, -1) < 0) break;
		if (pfd[0].revents & POLLIN) {
			n = read(s, buf, sizeof buf);
			if (n <= 0) break;
			write(mfd, buf, n);
		}
		if (pfd[1].revents & POLLIN) {
			n = read(mfd, buf, sizeof buf);
			if (n <= 0) break;
			write(s, buf, n);
		}
		if (pfd[0].revents & POLLHUP) break;   /* Mac closed */
		if (pfd[1].revents & POLLHUP) break;   /* bash exited */
	}

	kill(pid, SIGHUP);
	exit(0);
}
