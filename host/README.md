# Host-side files

Helpers that run on the **Mac** (the FS-UAE host), the counterpart to the
guest-side files in [`guest/`](../guest).

| Path | What it is |
|---|---|
| [`bin/revsh-listen`](bin/revsh-listen) | Listener for the AMIX reverse shell — gives the guest pty a raw terminal on the Mac and restores your tty on exit. Prefers `socat`, falls back to `nc`. Install e.g. `cp bin/revsh-listen /opt/homebrew/bin/`. |

## revsh-listen

```sh
revsh-listen            # loop: keeps relistening for the guest's reconnects
revsh-listen -1         # one shot: exit after a single session
revsh-listen 4444       # explicit port (default 4444)
```

- **Idle** (no guest connected): Ctrl-C quits the listener.
- **In a session**: Ctrl-C goes to the guest (interrupts the remote command);
  exit the guest shell to end the session.

The socat gotcha it encodes: write `tcp-listen:` **before** `file:$(tty),raw` —
socat opens the second address only after the first connects, so the terminal
stays cooked (Ctrl-C works) while idle and only goes raw once the guest is on.
The reverse order flips your terminal to raw immediately and you can't Ctrl-C
out until something connects.
