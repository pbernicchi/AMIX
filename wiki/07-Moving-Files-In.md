# Moving files in

Three routes were tried. Only two work.

## Tape — the good one

A tape at SCSI ID 4 coexists with the boot disk; a second *hardfile* does not.
Different emulation path (`scsitape`), so it does not trip whatever wedges the
WD33C93. The install tape was on the bus for the entire 2.1 installation.

Build a directory of files plus an `index.tape` listing them in order — each
listed file becomes one tape file:

    uae_uaehf0 = tape0,ro,:/path/to/tape_xfer,0,0,0,512,0,,scsi4,SCSI1

Archives must be **v7 format with numeric uid/gid**:

    tar --format=v7 --uid 0 --gid 0 -cf 01_ssh.tar *.pkg

AMIX's tar segfaults on ustar archives whose owner names do not exist locally —
`tar: problem reading group entry` then `Segmentation Fault - core dumped`.
v7 has no `uname`/`gname` fields at all, which avoids it. Verify by checking
that the magic field at offset 257 is zeroed rather than `ustar`.

Reading it back:

    cd /var/tmp
    tar xvf /dev/rmt/4n

The `n` suffix is the no-rewind device: repeat the command to walk successive
tape files. Plain `/dev/rmt/4` rewinds on close and gives you file 1 every time.
**The tape rewinds when the emulator restarts.**

`/usr/ucb/mt` is the BSD version and appends the `n` itself, so
`mt -f /dev/rmt/4n fsf 1` becomes `/dev/rmt/4nn: cannot open` — pass the base
name `/dev/rmt/4`. If `mt` will not cooperate, read past unwanted archives with
tar into a scratch directory and delete it.

## Floppy — works, low capacity

Write the payload into an ADF and read the raw device:

    dd if=/dev/dsk/fd0 of=setclk bs=512 count=17    # raw binary
    tar xvf /dev/dsk/fd0                            # v7 tar archive

This is the mechanism Commodore's own patch disks use. FS-UAE reads
`floppy_image_*` only at launch, so to get a new file in mid-session, overwrite
an image already in the swap list and re-insert it.

## Second hardfile — does not work

Do not do this. See [FS-UAE configuration](02-FS-UAE-Configuration.md).
