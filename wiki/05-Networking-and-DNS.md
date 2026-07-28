# Networking and DNS

`network_card = a2065` gives the Zorro II LANCE card, the only NIC AMIX
supports out of the box. FS-UAE emits `a2065=slirp`; confirm from the log:

    A2065: 'slirp' 00:00:00:32:33:34

SLIRP is user-mode NAT: guest `10.0.2.15`, gateway `10.0.2.2` (the Mac),
DNS `10.0.2.3`.

## Interface

The interface is **`aen0`**, not `le0` or `en0`. The 2.1 installer never asks
for a netmask, so it must be configured by hand:

    ifconfig aen0 10.0.2.15 netmask 255.255.255.0 up

The default route **needs a trailing metric** or the command is rejected:

    /usr/sbin/route add default 10.0.2.2 1

Make both permanent in `/etc/inet/rc.inet`.

## DNS

A DNS bundle is published on the wiki as `amix_dns.zip`
(<https://www.amigaunix.com/lib/exe/fetch.php/scripts_configs:amix_dns.zip>).
It sets up a local `named` forwarding to the SLIRP resolver, with
`/etc/resolv.conf` pointing at `nameserver 127.0.0.1`.

Two fixes were needed before it would load — both findings from this build,
not documented upstream:

- The SOA records lack the RNAME field (`@ IN SOA 127.0.0.1. (`), which a 1992
  BIND rejects as a database format error.
- `named.root` carries AAAA records that BIND 4 cannot parse.

Keep the domain as `nodomain`. Otherwise lookups get the domain appended and
`ping` breaks.

## Inbound connections do not work on macOS

`uae_a2065 = slirp_inbound` opens a **fixed** set of host ports — 21–23 and 80.
All are privileged, and FS-UAE runs unprivileged, so none can be bound.
Verified with the guest fully booted and `A2065: 'slirp_inbound'` in the log:
`lsof -nP -iTCP -sTCP:LISTEN -p <pid>` shows no listeners at all.

`uae_slirp_redir` and `uae_slirp_ports` exist and the core accepts them, but it
accepts the option *name* without validating the value, so a `result: 1` in the
log proves nothing.

Outbound is unrestricted, so the way in is a **reverse tunnel opened from
AMIX**, with Remote Login enabled on the Mac:

    # on AMIX
    ssh -R 2222:10.0.2.15:22 user@10.0.2.2
    # then, from any Mac terminal, while that session stays open
    ssh -p 2222 root@localhost

OpenSSH 3.9 offers only SHA-1 key exchange and `ssh-rsa`/`ssh-dss` host keys,
all disabled by default in modern OpenSSH, so the Mac's `/etc/ssh/sshd_config`
needs them re-enabled:

    KexAlgorithms +diffie-hellman-group-exchange-sha1,diffie-hellman-group1-sha1
    HostkeyAlgorithms +ssh-rsa,ssh-dss
    PubkeyAcceptedAlgorithms +ssh-rsa
    Ciphers +aes128-cbc,3des-cbc
    MACs +hmac-sha1

That weakens the Mac's sshd for every client; scope it with a
`Match Address 10.0.2.0/24` block or revert it afterwards.
