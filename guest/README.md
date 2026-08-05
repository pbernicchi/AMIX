# Guest-side files

Files that live **inside** AMIX, laid out to mirror the guest filesystem so the
install target is obvious. Transfer them in over the HTTP path (see
[`config-reference.md`](../config-reference.md) → "Getting files into AMIX", or
wiki [07 Moving Files In](https://github.com/pbernicchi/AMIX/wiki/07-Moving-Files-In)).

| Path | What it is |
|---|---|
| [`etc/init.d/tunnel`](etc/init.d/tunnel) | Persistent, auto-reconnecting reverse SSH tunnel to the Mac. Install per the header comment; links as `/etc/rc2.d/S99tunnel`. |
| [`etc/init.d/revsh`](etc/init.d/revsh) | Runs `/usr/local/sbin/revsh` at boot — reverse shell to a `nc` listener on the Mac, with a real pty via `/usr/bin/script`. Links as `/etc/rc2.d/S99revsh`. |
