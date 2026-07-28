# Installing AMIX

Follow the procedure on the Amiga Unix Wiki:
<https://amigaunix.com/doku.php/installation>. This page records the
emulator-specific parts only.

## SCSI IDs are hard-coded

The installer expects **tape at SCSI ID 4 and disk at SCSI ID 6**. These are
not configurable.

## Attaching the tape

FS-UAE has no GUI option for a SCSI tape. It needs a raw pass-through line:

    uae_uaehf0 = tape0,ro,:/path/to/tape_21,0,0,0,512,0,,scsi4,SCSI1

FS-UAE renumbers it internally so it does not collide with the hardfile.
Confirm from the log:

    Adding A3000 mainboard SCSI TAPE unit 4 ('.../tape_21')
    TAPEEMU INDEX: '.../tape_21/index.tape'

## The disk

A 600 MB hardfile at SCSI 6, `hard_drive_0_type = rdb`. The installer creates
the RDB itself, writing `RDSK` at **block 2** with three partitions:

| Partition | DosType | Becomes |
|---|---|---|
| `UNIX_Root` | `UNI\1` | `/dev/dsk/c6d0s1`, mounted `/` |
| `UNIX_Swap` | `resv` | swap |
| `UNIX_Boot` | `UNI\0` | boot |

Slice numbers follow position in the partition chain.

## During the install

Set warp mode expectations first: **⌘W and ⌘Q are macOS window/quit commands**
and will kill the emulator mid-install. FS-UAE documents Mod+W as warp mode and
Mod is Cmd on macOS, but macOS wins. Rebinding via `keyboard_key_f10` and
`keyboard_key_w_ctrl` was tried and neither took effect — the keystrokes pass
through to AMIX. Unresolved.

Take a snapshot as soon as the install finishes.
