# Sources and media

## Distribution images

From the Amiga Unix Wiki downloads page,
<https://www.amigaunix.com/doku.php/downloads>, and the TOSEC set:

| Item | Notes |
|---|---|
| `amix_21_boot.adf`, `amix_21_root.adf` | 2.1 install floppies |
| `amix_21_tape.zip` + `amix_21_tape_seglist.zip` | 2.1 install tape, 29 segments |
| 2.1p2a patch disk | Commodore's official patch |
| 2.1 patch disk (oddsocks, 2013) | For the HD-install hack — *not* what this build uses |
| 2.03 boot/root/tape | Older release, kept as a fallback |

Originals live in `media/` in this repo, gitignored: they are large and not
redistributable.

## Kickstart ROM

`amiga-os-204-a3000.rom` — KS 2.04 (A3000) rev 37.175. The Cloanto-encrypted
copy works provided `rom.key` sits in the same `Kickstarts/` directory; FS-UAE
decrypts on load. Confirm from the log:

    UAE: KS ROM v2.04 (A3000) rev 37.175 (512k)

## Tape segment preparation

The tape is a directory of plain SVR4 cpio segments. Two things matter:

- The TOSEC 2.03 dump ships them **gzipped** (`i_NN.cpio.gz`); they must be
  gunzipped before use.
- The directory **must** contain an `index.tape` file listing the segment
  filenames in tape order. Without it, WinUAE's directory scan matches nothing
  and the tape reads as empty — with no error to explain why.

The `index.tape` requirement is a finding from this build.
