---
name: amix-fs-uae-setup
description: Amiga 3000UX / AMIX (SVR4 2.1) FS-UAE setup lives in ~/Documents/FS-UAE/AMIX with config AMIX-A3000UX.fs-uae
metadata: 
  node_type: memory
  type: project
  originSessionId: 73c81938-bd5b-42aa-8d41-28904144422c
  modified: 2026-07-28T04:16:28.036Z
---

Amiga UNIX (AMIX) SVR4 2.1 emulation was set up 2026-07-25 for FS-UAE 3.2.35
(core = WinUAE 3300b2) on macOS.

- Config: `~/Documents/FS-UAE/Configurations/AMIX-A3000UX.fs-uae`
- Assets: `~/Documents/FS-UAE/AMIX/` — `floppies/`, `tape_21/` (+ `tape_203/`
  fallback built from the TOSEC 2.03 dump), `a3000ux.hdf` (600 MB, blank RDB)
- Reference: https://amigaunix.com/doku.php/installation

Non-obvious constraints (all verified against WinUAE `scsitape.cpp` and the
generated `~/Documents/FS-UAE/Cache/Logs/debug.uae`):

- SCSI IDs are hard-coded in the AMIX installer: **tape must be ID 4, disk ID 6**.
- The tape has no GUI option — it needs a raw `uae_uaehf0 = tape0,ro,:<dir>,...`
  line. FS-UAE renumbers it to `uaehf1` so it does not collide with the hardfile.
- A tape directory **must** contain an `index.tape` file listing the segment
  filenames in tape order. Without it the directory-scan path in WinUAE's
  `tape_nextfile()` matches only a file literally named `index.tape`, so the tape
  reads as empty.
- Tape segments are plain SVR4 cpio. The TOSEC 2.03 dump ships them gzipped
  (`i_NN.cpio.gz`) — they must be gunzipped before use.
- Fast RAM must stay at **16 MB max**; more makes the kernel mis-map SCSI.
- MMU on, JIT off (`cachesize=0`), `cpu_compatible=false` — otherwise kernel
  panics / "sort: fatal: line too long" during package install.
- ROM: `amiga-os-204-a3000.rom` (Cloanto-encrypted, decrypted via the `rom.key`
  already in `~/Documents/FS-UAE/Kickstarts/`).
- Networking: `network_card = a2065` (the only NIC AMIX supports out of the box)
  → FS-UAE emits `a2065=slirp`. Interface is **`aen0`** in AMIX; static config
  10.0.2.15/24, gw 10.0.2.2, DNS 10.0.2.3. `uae_a2065 = slirp_inbound` instead
  if inbound telnet/rlogin to AMIX is wanted. Default route needs the trailing
  metric: `/usr/sbin/route add default 10.0.2.2 1` in `/etc/inet/rc.inet`.
- Colour X needs the **A2410** board (stock ECS gives 1-bit mono X). FS-UAE's
  `graphics_card` option does NOT expose it — use `uae_gfxcard_type = A2410`
  **plus `uae_gfxcard_size = 4`**. Size defaults to 0 and the board is then
  silently never instantiated, with no error anywhere. Confirm via the log line
  `Card 1: Z2 0x00e90000  64K IO  A2410`. Then `olinit -- -tiga` (1024x768).
- **`uae_cpu_speed = max` breaks booting** — hangs at the SCSI probe, white
  screen, no `MMU enabled`. Leave the option out entirely.
- **A second hardfile on the A3000 SCSI bus white-screens the machine, full
  stop.** Tried with no RDB and with a valid AMIX-style RDB (RDSK at block 2,
  `UNI\1` partition) — both hang, during the Kickstart bus scan before AMIX
  gets control. Verified by A/B: both drives = no `MMU enabled` after 110 s;
  boot disk alone = `MMU enabled` inside 70 s. Use the floppy transfer path.
- **`SCSI command buffer overflow!` is noise, not a fault.** It floods the log
  (GB per session) during healthy boots too. The reliable boot signal is
  `68030 MMU enabled` — grep for that, never for the overflow.
- **`slirp_inbound` cannot work on macOS.** It opens a fixed set of privileged
  ports (21-23, 80) and FS-UAE runs unprivileged, so nothing binds — verified
  by `lsof` against a fully-booted guest: zero listeners. `uae_slirp_redir` /
  `uae_slirp_ports` are accepted by name without value validation, so a
  `result: 1` in the log means nothing. Slirp only fully starts once the guest
  brings up the A2065, so no-hard-drive tests can't show this.
- **The way in is to invert direction: the guest connects OUT** (outbound
  through slirp is unrestricted; 10.0.2.2 = Mac host). Two flavours: a reverse
  *SSH tunnel* for a full-TTY login (`ssh -R 2222:localhost:22 user@10.0.2.2`,
  needs legacy KEX/hostkey re-enabled in the Mac's sshd_config), or a raw
  reverse *shell* (`revsh.c`: socket→connect 10.0.2.2:port→dup to 0,1,2→exec
  /bin/sh, built `-lsocket -lnsl`; `nc -l 4444` on the Mac first).
- **HTTP is the easy file-transfer path once networking is up.** Serve on the
  Mac — `cd ~/Documents/FS-UAE/AMIX/http && python3 -m http.server 8000 &` — and
  pull on the guest with `lynx -source http://10.0.2.2:8000/<file> > <dest>`
  (lynx is installed; `-source` dumps raw bytes, so binaries work too). This is
  NOT an FS-UAE feature — it's a plain Mac-side server reached through slirp's
  host alias. Helper sources (`revsh.c`, etc.) live in that `http/` dir.
- **bash 2.05b was built from source** with the gcc 2.7.2.3 toolchain, fetched
  over that HTTP path (pull tarball → configure/make), and is now root's login
  shell. This **supersedes wiki 08's "There is no bash in the collection"** —
  ksh is no longer the only good interactive shell present.
- The A2065 dumps every packet in full hex to the log; expect ~1.5 GB per two
  hours of networked uptime. There is no option to disable it (the binary only
  has `logs_dir`, `log_flush`, `save_log`); symlink `fs-uae.log.txt` to
  `/dev/null` for long unattended runs.
- Removing `hard_drive_1*` from the config *file* does not detach the drive —
  the FS-UAE Launcher keeps its own drive list and passes it at launch anyway.
  Clear the slot in the Launcher GUI and verify in `fs-uae.log.txt`.
- File transfer in (**bootstrap path, before networking is up** — use the HTTP
  path above once it is): write the payload to a **floppy image** and read the
  raw device (`dd if=/dev/dsk/fd0` or `tar xvf /dev/dsk/fd0`) — the mechanism the
  Commodore patch disk itself uses. AMIX's tar segfaults on ustar archives whose
  uname/gname don't exist locally; build with `--format=v7 --uid 0 --gid 0`.
  FS-UAE reads `floppy_image_*` only at launch, so to get a new file in without
  restarting, overwrite a file already in the swap list and re-insert it.
- The wiki's `amix_dns.zip` is broken: its SOA records lack the RNAME field, and
  `named.root` carries AAAA records a 1992 BIND cannot parse. Fixed copies are
  in `~/Documents/FS-UAE/AMIX/floppies/`.
