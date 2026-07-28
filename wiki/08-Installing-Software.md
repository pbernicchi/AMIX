# Installing software

Michael Parson's re-bundled SVR4 packages are on the Amiga Unix Wiki:
<https://www.amigaunix.com/doku.php/downloads>. They install with `pkgadd`
rather than needing to be built.

## Before the first install

    groupadd staff

Every package was built on a host with a `staff` group. Without it each install
reports `attribute verification failed` and `Installation partially failed` —
cosmetic, the files still land correctly, but it obscures real errors.

## Gotchas

- **`pkgadd -d` needs an absolute path.** A bare filename fails with `errno=2`
  even from the directory containing it.
- **SVR4 `chown` has no `user:group` form.** Use `chown root x; chgrp sys x`.
- **SVR4 `grep` has no `-E`.** Use `egrep`, or install `gnugrep` early.
- Decompress `.pkg.gz` on the host side — base AMIX has no gunzip, which is
  exactly why the packager ships `fsfgzip-1.3.5.pkg` uncompressed.
- Delete each `.pkg` immediately after installing it. Holding archives and
  installed copies at once is what runs a 600 MB disk out of space.

## OpenSSH

Order: `zlib`, `openssl`, `prngd`, `openssh`.

`prngd` is not optional. There is no `/dev/random` on a 1992 kernel and this
OpenSSH build's `ssh-rand-helper` is hard-wired to PRNGD on TCP port 708 —
without it, key generation fails with `Entropy collection failed`. The package
installs `/etc/init.d/prngd`, `/etc/prngd.conf` and `/etc/rc2.d/S70prngd`, and
starts with:

    /usr/local/sbin/prngd /var/run/egd-pool tcp/localhost:708

`/var/run` is a Solaris 2.x convention that SVR4.0 predates, so create it first
if missing. Note `/etc/prngd.conf` is a Solaris 2.6 contribution and validates
each entropy source's path before using it; sources that do not resolve are
silently skipped.

Privilege separation needs an account and an empty directory:

    mkdir /var/empty; chown root /var/empty; chgrp sys /var/empty
    chmod 755 /var/empty
    groupadd sshd
    useradd -g sshd -d /var/empty -s /bin/false sshd

Host keys go where `sshd_config` expects them. `ssh-keygen` writes whatever
`-f` says — the `host` in the filename is not automatic:

    ssh-keygen -t rsa1 -f /usr/local/etc/ssh_host_key -N ""
    ssh-keygen -t rsa  -f /usr/local/etc/ssh_host_rsa_key -N ""
    ssh-keygen -t dsa  -f /usr/local/etc/ssh_host_dsa_key -N ""

Then link it into the boot sequence, after prngd's S70:

    ln -s /usr/local/etc/init.d/sshd /etc/rc2.d/S99sshd

## Compiler

**gcc 2.7.2.3 alone is enough.** The 2.5.8 → 2.6.3 → 2.7.2.3 chain described in
the packager's README is build-time ancestry, not a runtime dependency — these
are prebuilt binaries. Installing only 2.7.2.3 saves 27.6 MB and works
(verified). Install `binutils-2.7` alongside it, then immediately:

    mv /usr/local/bin/as /usr/local/bin/gas

or it shadows the system assembler.

    PATH=/usr/local/gcc-2.7.2.3/bin:/usr/local/bin:$PATH

## Shells

There is no bash in the collection. `ksh` (ksh88) is the best interactive shell
present, but line editing is off until enabled and arrows are unbound:

    set -o emacs                   # then ESC ESC completes, ESC = lists
    alias __A="$(print '\020')"    # up    -> Ctrl-P
    alias __B="$(print '\016')"    # down  -> Ctrl-N

`csh` with `set filec` gives literal Tab completion, if that matters more.
