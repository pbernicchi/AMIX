# Patching to 2.1p2a

Two different patch disks exist and they are not interchangeable:

- **2.1p2a (Commodore)** — the official patch. This is the one to use for a
  normal tape install. Brings the kernel to 2.1c.
- **2.1 patch disk (oddsocks, 2013)** — for the hard-disk-install hack. Not
  applicable here.

See <https://www.amigaunix.com/doku.php/patch-disk>.

Insert the 2.1p2a disk, follow its instructions, then:

    shutdown -i6

Everything including networking should come back up. Confirm:

    $ uname -a
    UNIX_System_V amix 4.0 2.1c 0800430 Amiga (Unlimited) m68k

Snapshot again once verified.

## Snapshots

APFS clones make these effectively free:

    cd ~/Documents/FS-UAE/AMIX
    cp -c a3000ux.hdf a3000ux-2.1c-patched.hdf

To restore, **rename the live file aside first** rather than letting `cp`
truncate in place, so a mid-copy failure cannot damage the snapshot:

    mv a3000ux.hdf a3000ux-old.hdf
    cp -c a3000ux-2.1c-patched.hdf a3000ux.hdf
