# AMIX

Amiga UNIX (System V Release 4) 2.1p2a running on an emulated Amiga 3000UX
under FS-UAE on macOS.

```
$ uname -a
UNIX_System_V amix 4.0 2.1c 0800430 Amiga (Unlimited) m68k
```

## Contents

| Path | |
|---|---|
| [`wiki/`](wiki/Home.md) | Full setup documentation, start at `Home.md` |
| [`config-reference.md`](config-reference.md) | Working reference for the running system |
| [`config/`](config/) | The FS-UAE configuration |
| [`AMIX.command`](AMIX.command) | Double-clickable launcher, with a same-instance guard |
| `media/` | Original ROM and distribution images — gitignored |
| [`memory/`](memory/) | Project notes |

## Quick start

```sh
./AMIX.command
```

Boot takes ~70 s, or ~145 s with the tape attached. Always `shutdown -i0`
inside AMIX before closing the emulator.

## No media here

This repository contains **documentation and configuration only**. No Amiga
UNIX images, Kickstart ROM or distribution media are included or redistributed.

- The **Kickstart ROM** is Commodore copyright. Obtain it legitimately — a
  licensed copy ships with [Amiga Forever](https://www.amigaforever.com/) from
  Cloanto, which is what this build uses.
- **AMIX distribution images** are hosted by the
  [Amiga Unix Wiki](https://www.amigaunix.com/doku.php/downloads), not here.
- **SVR4 packages** come from that same wiki and carry their own licences.

Local copies live in `media/`, which is gitignored and never committed.

## A warning about the SSH instructions

[Networking and DNS](wiki/05-Networking-and-DNS.md) explains how to re-enable
SHA-1 key exchange and `ssh-rsa` host keys in macOS's `sshd_config`. That is
necessary to reach an OpenSSH 3.9 build from 2004, and it **meaningfully
weakens your SSH server for every client that connects to it**. Scope it to the
emulator's subnet with a `Match Address` block, or revert it afterwards. Do not
apply it to anything exposed to a network you do not control.

## Licence

Code (`AMIX.command`, `config/`) is MIT — see [LICENSE](LICENSE).
Documentation (`README.md`, `CREDITS.md`, `config-reference.md`, `memory/`,
`wiki/`) is CC BY 4.0 — see [LICENSE-DOCS](LICENSE-DOCS).

## Credit

The installation procedure, distribution media and SVR4 package collection come
from the **Amiga Unix Wiki** — <https://www.amigaunix.com/>. Departures from
that procedure are noted as such in the wiki pages here.

Full acknowledgements in [CREDITS.md](CREDITS.md) — this build rests on the
work of many people, in particular Michael Parson's SVR4 package bundles and
the WinUAE SCSI tape emulation.
