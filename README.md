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

## Credit

The installation procedure, distribution media and SVR4 package collection come
from the **Amiga Unix Wiki** — <https://www.amigaunix.com/>. Departures from
that procedure are noted as such in the wiki pages here.
