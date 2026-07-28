# Credits

This project is documentation of a build, not original software. Almost
everything it stands on is someone else's work.

## Amiga Unix Wiki — <https://www.amigaunix.com/>

The reference for anything AMIX-specific: the installation procedure, the
distribution media, the patch disks, the DNS configuration bundle and the SVR4
package collection all come from there. Without it this build would not have
been possible. If you find this repository useful, go and read theirs.

## Software

| | |
|---|---|
| **Amiga UNIX (AMIX)** | Commodore-Amiga, Inc. System V Release 4 for the A2500UX / A3000UX, 1991–1992 |
| **FS-UAE** | Frode Solheim — <https://fs-uae.net/> |
| **WinUAE** | Toni Wilen — the emulation core FS-UAE derives from, including the SCSI tape emulation this build depends on |
| **Kickstart ROM** | Commodore; licensed redistribution by Cloanto (Amiga Forever) |

## Packages and contributions

- **Michael Parson** — re-bundled the SVR4 packages that make this system
  usable: gcc, binutils, OpenSSH, OpenSSL, perl, vim, ncurses, screen and the
  rest. The entire software chapter of this documentation rests on that work.
- **oddsocks** — the 2013 patch disk for the hard-disk install route.
- **Lutz Jaenicke** — PRNGD, without which OpenSSH cannot generate a key on a
  kernel that predates `/dev/random`.
- **Louis LeBlanc** — the Solaris 2.6 entropy-gathering configuration shipped
  as `/etc/prngd.conf`.
- The contributor of the `amix_dns.zip` bundle to the Amiga Unix Wiki.

## Preservation

- **TOSEC** — the distribution image sets.

## What is original here

The emulator configuration and the findings recorded in [`wiki/`](wiki/Home.md)
and [`config-reference.md`](config-reference.md): the boot failures and their
causes, the SCSI bus limits, the tape-based transfer route, the archive format
requirements, and the macOS-specific networking constraints. Those are marked
as findings from this build on the pages where they appear, and are not
attributable to any of the above.
