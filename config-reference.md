# Amiga 3000UX / AMIX — FS-UAE configuration reference

Working config: `~/Documents/FS-UAE/Configurations/AMIX-A3000UX.fs-uae`
(The Launcher rewrites that file alphabetically and drops comments when it
saves, so the reasoning lives here instead.)

Installed system: Amiga UNIX SVR4 2.1, patched to **2.1p2a / kernel 2.1c**
(`uname -a` → `UNIX_System_V amix 4.0 2.1c 0800430 Amiga (Unlimited) m68k`)

## Settings and why

| Setting | Value | Why |
|---|---|---|
| `amiga_model` | `A3000` | ECS + A3000 chipset extras + built-in SCSI |
| `kickstart_file` | `amiga-os-204-a3000.rom` | KS 2.04 (A3000) 37.175. Cloanto-encrypted; decrypted via the `rom.key` in `Kickstarts/` |
| `chip_memory` | `2048` | 2 MB chip |
| `motherboard_ram` | `16384` | **Hard ceiling.** AMIX only recognises 4–16 MB fast RAM; more and the kernel mis-maps the SCSI controller |
| `uae_cpu_model` / `uae_mmu_model` | `68030` | MMU is mandatory — AMIX will not run without it |
| `uae_fpu_model` | `68882` | As per A3000UX |
| `uae_cpu_compatible`, `uae_cpu_cycle_exact`, `uae_blitter_cycle_exact` | `false` | "More compatible" causes kernel panics and `sort: fatal: line too long` during package install |
| `uae_cachesize` | `0` | JIT **off** — JIT + MMU panics the kernel |
| `uae_cpu_speed` | **omitted** | `max` **breaks booting** — the A3000 WD33C93 never completes its handshake and the boot never leaves the SCSI probe (white screen, no `MMU enabled` in the log). Leave unset |
| `hard_drive_0_controller` | `scsi6` | **SCSI ID 6 is hard-coded** in the AMIX installer |
| `hard_drive_0_type` | `rdb` | Installer detects no disks without it |
| `network_card` | `a2065` | Only NIC AMIX supports out of the box → emits `a2065=slirp` |
| `uae_gfxcard_type` | `A2410` | Colour X. Not exposed by FS-UAE's own `graphics_card` option |
| `uae_gfxcard_size` | `4` | **Required.** Defaults to 0, and the board is then silently never instantiated |

## Tape — SCSI ID 4 (the working way to move files in)

No GUI option exists; it needs a raw line. Swap the directory for whichever
tape you want mounted:

    uae_uaehf0 = tape0,ro,:/Users/pbernicc/Documents/FS-UAE/AMIX/tape_xfer,0,0,0,512,0,,scsi4,SCSI1
    uae_uaehf0 = tape0,ro,:/Users/pbernicc/Documents/FS-UAE/AMIX/tape_21,0,0,0,512,0,,scsi4,SCSI1   # install tape

The tape directory needs an `index.tape` listing the filenames in tape order.
Without it WinUAE's directory scan matches nothing and the tape reads as empty.
Each listed file becomes one tape file, in order.

**A tape at ID 4 coexists with the boot disk — a second *hardfile* does not.**
The install tape was on the bus for the whole 2.1 installation. Different
emulation path (`scsitape`), so it does not trip whatever wedges the WD33C93
when two hardfiles are present.

`tape_xfer/` currently holds the amigaunix.com SVR4 package archive, split so
you only extract what fits:

| Tape file | Size | Contents |
|---|---|---|
| `01_ssh.tar` | 13 MB | zlib, openssl, openssh 3.9p1, prngd + the README |
| `02_tools.tar` | 30 MB | gzip, GNU tar, bzip2, unzip, less, ncurses, vim, nvi, sudo, screen, grep, sed, par, lynx, perl |
| `03_devel.tar` | 76 MB | gcc 2.5.8 / 2.6.3 / 2.7.2.3, binutils, libg++, gettext, texinfo, libtool |

Archives are v7 (`--format=v7 --uid 0 --gid 0`) and the packages are stored
**uncompressed** — base AMIX has no gunzip, which is exactly why the packager
ships `fsfgzip-1.3.5.pkg` uncompressed. Install with `pkgadd -d <file>.pkg`.
Masters (still gzipped) are in `AMIX/packages/`, staging copies in
`packages/staging/`. Source: https://www.amigaunix.com/doku.php/downloads

Build order matters for the compilers: gcc-2.5.8 → gcc-2.6.3 → gcc-2.7.2.3,
each built by the previous one. `gdb-4.17`, `amiwm` (needs X11R5 from the
Gateway CD) and `atari-motif` were deliberately not pulled.

**No CD-ROM path exists** — the kernel has no `hsfs`/`cdfs`, confirmed on the
running system, so the Gateway Volume 2 ISO can only be read on the Mac side.

## transfer.hdf — SCSI ID 5 (DISABLED — does not work)

**A second hardfile on the A3000 SCSI bus white-screens the machine.** Two
attempts, both dead:

1. Bare tar stream, no RDB at block 0 — white screen.
2. 32 MB image with a **valid** RigidDiskBlock built to AMIX's own conventions
   (RDSK at block 2, 1 head x 64 sectors x 1024 cyls, one `XFER` partition at
   cyls 2-1023, DosType `UNI\1`, flags NOMOUNT, v7 tar payload at byte 65536
   = the partition start) — **also white screen.**

Confirmed by a clean A/B with an otherwise identical config: with both drives,
no `MMU enabled` after 110 s; with the boot disk alone, `MMU enabled` inside
70 s. It hangs during the Kickstart bus scan, before AMIX gets control, which
is why nothing appears on screen. Both hardfiles open cleanly first
(`size=614400K`, `size=32768K`, `rdb mode: 1`, no errors), so the images are
fine as far as FS-UAE is concerned.

**Do not diagnose with `SCSI command buffer overflow!`.** It floods the log
(GB per session) in the hung case *and* in a perfectly healthy boot. It is
noise. The reliable signal is `68030 MMU enabled` — that line means the AMIX
kernel took over; its absence means the boot never got off the SCSI probe:

    grep -c "MMU enabled" ~/Documents/FS-UAE/Cache/Logs/fs-uae.log.txt

The image is kept at `AMIX/transfer.hdf` in case a later FS-UAE/WinUAE core
fixes this. **Use the floppy path instead** (see "Getting files into AMIX") —
it is proven and it is what Commodore's own patch disks use.

Note: removing the `hard_drive_1*` lines from the config file is **not enough**.
The Launcher keeps its own copy of the drive list and passes it at launch even
when the file has no `hard_drive_1` path line. Clear slot 1 in the Launcher's
Hard Drives panel, and confirm via the log that only unit 6 is added.

## Installed software (beyond base 2.1p2a)

OpenSSH 3.9p1 + OpenSSL + zlib + prngd, all in `/usr/local`. sshd needs
prngd listening on `tcp/localhost:708` (no `/dev/random` on a 1992 kernel);
`/etc/rc2.d/S70prngd` starts it, `S99sshd` starts sshd after it. Host keys are
`/usr/local/etc/ssh_host{,_rsa,_dsa}_key` — ssh-keygen writes whatever `-f`
says, so the `host` in the name is not automatic.

Tools: ncurses, gzip, GNU tar, bzip2, unzip, less, vim 5.8, nvi, sudo, screen,
GNU grep (gives the `-E` the system grep lacks), GNU sed, par, lynx, perl 5.005.

Compiler: **gcc 2.7.2.3 alone** in `/usr/local/gcc-2.7.2.3`, plus binutils,
libg++, gettext, texinfo, libtool. The 2.5.8 → 2.6.3 → 2.7.2.3 chain is
build-time ancestry only — 2.7.2.3 runs standalone (verified), so the two older
gccs (27.6 MB) are unnecessary. Rename `/usr/local/bin/as` to `gas` after
installing binutils or it shadows the system assembler.

`pkgadd -d` needs an **absolute** path; a bare filename fails with errno=2.
Packages were built on a host with a `staff` group — `groupadd staff` first or
every install reports "partially failed" on attribute verification (cosmetic;
files still install). Same for SVR4 `chown`, which has no `user:group` form.

## Verified log lines (what a good boot looks like)

    UAE: KS ROM v2.04 (A3000) rev 37.175 (512k)
    Adding A3000 mainboard SCSI HD unit 6 ('.../a3000ux.hdf')
    A2065: 'slirp' 00:00:00:32:33:34
    Card 1: Z2 0x00e90000   64K IO  A2410

## Inbound networking — not possible on macOS

`uae_a2065 = slirp_inbound` opens a **fixed** set of host ports (21-23, 80).
All are privileged, and FS-UAE runs unprivileged, so none can be bound —
verified: with `A2065: 'slirp_inbound'` in the log and AMIX fully booted,
`lsof -nP -iTCP -sTCP:LISTEN -p <pid>` shows **no listeners at all**.
`uae_slirp_redir` / `uae_slirp_ports` exist as options and the core accepts
them (`result: 1`) but never register a host forward with libslirp. Tested
**properly with a booted guest**: config loaded `uae_slirp_redir = tcp:2323:23`,
the log confirmed `SLIRP polling thread started` and `Slirp start`, and still
only the serial listener appeared — no `2323`. (Earlier no-hard-drive tests were
worthless here: without a booted guest slirp never fully starts, so no forward
could appear regardless. The polling thread only starts once the guest brings
up the A2065.)

Outbound is unrestricted, so the way in is a **reverse tunnel opened from
AMIX** (needs Remote Login on the Mac):

    # on AMIX
    ssh -R 2222:localhost:22 pbernicc@10.0.2.2
    # then from any Mac terminal, while that session stays open
    ssh -p 2222 root@localhost

Expect to have to re-enable legacy crypto in the Mac's
`/etc/ssh/sshd_config` — OpenSSH 3.9 offers only SHA-1 key exchange and
`ssh-rsa`/`ssh-dss` host keys, all disabled by default in modern OpenSSH.

To make that tunnel survive **guest reboots**, install `guest/etc/init.d/tunnel`
(in this repo) and link it as `/etc/rc2.d/S99tunnel` — the same rc convention as
`S70prngd` / `S99sshd`. It runs an auto-reconnect loop with key auth. OpenSSH 3.9
predates `ExitOnForwardFailure`, so the loop — not ssh — handles reconnection;
set `ClientAliveInterval 60` on the Mac so a dead tunnel frees port 2222 for the
next attempt.

## AMIX-side notes

- Interface is `aen0`. Static: 10.0.2.15 / 255.255.255.0, gw 10.0.2.2, DNS 10.0.2.3.
- Default route needs a trailing metric: `/usr/sbin/route add default 10.0.2.2 1`
- Local `named` forwards to 10.0.2.3; `/etc/resolv.conf` → `nameserver 127.0.0.1`
- Keep the domain as `nodomain`, or lookups get the domain appended and `ping` breaks.
- Colour X: `olinit -- -tiga` (1024×768) or `olinit -- -tiga -tm 3` (800×600)
- **Always `shutdown -i0` before quitting FS-UAE.** ⌘Q or ⌘W on a running system
  leaves the root filesystem dirty. (⌘W is macOS Close Window — it is *not* warp
  mode, despite Mod+W being FS-UAE's documented warp binding.)

## Getting files into AMIX

**Once networking is up, HTTP is the easy path.** Outbound through slirp is
unrestricted and the Mac host is `10.0.2.2`, so serve the files on the Mac and
pull them from the guest — no media juggling:

    # on the Mac
    cd ~/Documents/FS-UAE/AMIX/http && python3 -m http.server 8000 &
    # in AMIX (lynx is installed; -source dumps raw bytes, so binaries work too)
    lynx -source http://10.0.2.2:8000/revsh.c > /tmp/revsh.c

This is a plain Python server on the Mac reached through slirp's host alias —
**not** an FS-UAE feature. It's how the from-source builds (bash 2.05b, the gcc
toolchain helpers) and `revsh.c` were pulled in; keep the served files in
`~/Documents/FS-UAE/AMIX/http/`.

Before networking exists, use the **floppy** bootstrap instead. Write the
payload to a floppy image and read the raw device:

    dd if=/dev/dsk/fd0 of=setclk bs=512 count=17     # raw binary
    tar xvf /dev/dsk/fd0                              # v7 tar archive

Build archives with `--format=v7 --uid 0 --gid 0`. AMIX's tar segfaults on ustar
archives whose owner names don't exist locally ("problem reading group entry").

FS-UAE reads `floppy_image_*` only at launch. To get a new file in mid-session,
overwrite a file already in the swap list and re-insert it.

## Snapshots (APFS clones — near-zero disk cost)

| Image | State |
|---|---|
| `a3000ux-postinstall.hdf` | Bare 2.1 install, no patch, no network |
| `a3000ux-2.1c-patched.hdf` | 2.1p2a patched, networking + DNS |
| `a3000ux-2.1c-x11.hdf` | Above + working setclk + colour X on the A2410 |

Restore by copying one over `a3000ux.hdf` while the emulator is not running.
Use `cp -c` (APFS clone) and **rename the old file aside first** rather than
letting `cp` truncate in place, so the snapshot can never be damaged mid-copy:

    cd ~/Documents/FS-UAE/AMIX
    mv a3000ux.hdf a3000ux-old.hdf
    cp -c a3000ux-2.1c-x11.hdf a3000ux.hdf

Restored from `a3000ux-2.1c-x11.hdf` on 2026-07-27 after the transfer.hdf
white screen. The failed image is parked at `a3000ux-whitescreen-20260727.hdf`
and can be deleted once the restore is confirmed good.
