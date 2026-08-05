/* revsh-nopty - reverse shell, no pty. FALLBACK for when /dev/ptmx is
 * unavailable; the canonical revsh.c gives a real pty and is preferred.
 *
 * AMIX (guest) connects OUT to a listener on the Mac (host) and runs an
 * interactive bash. The socket is not a tty and cannot be made a controlling
 * terminal, so this is a "dumb" shell -- no job control, no arrow-key editing,
 * no curses/vi. bash -i still gives a real prompt and runs commands (bash
 * writes the prompt to stderr, which -i forces on regardless of tty).
 *
 * Build on the guest:  cc -o revsh revsh-nopty.c -lsocket -lnsl
 * Install:             cp revsh /usr/local/sbin/revsh
 * On the Mac first:    while : ; do nc -l 4444 ; done
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HOST "10.0.2.2"          /* the Mac, via slirp */
#define PORT 4444
#define BASH "/usr/local/bin/bash"

int main()
{
	int s;
	struct sockaddr_in sa;

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

	dup2(s, 0);
	dup2(s, 1);
	dup2(s, 2);
	if (s > 2) close(s);

	putenv("TERM=vt100");
	execl(BASH, "bash", "-i", (char *)0);
	perror("exec bash");
	exit(1);
}
