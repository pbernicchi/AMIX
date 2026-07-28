# FS-UAE configuration

Working config: [`config/AMIX-A3000UX.fs-uae`](../config/AMIX-A3000UX.fs-uae).

The FS-UAE Launcher rewrites that file alphabetically and strips comments when
it saves, which is why the reasoning lives here and not inline.

## Settings

| Setting | Value | Why |
|---|---|---|
| `amiga_model` | `A3000` | ECS, A3000 chipset, built-in WD33C93 SCSI |
| `kickstart_file` | `amiga-os-204-a3000.rom` | KS 2.04 (A3000) 37.175 |
| `chip_memory` | `2048` | 2 MB chip |
| `motherboard_ram` | `16384` | **Ceiling.** AMIX recognises 4–16 MB; above that the kernel mis-maps the SCSI controller |
| `uae_cpu_model` / `uae_mmu_model` | `68030` | The MMU is mandatory — AMIX will not run without it |
| `uae_fpu_model` | `68882` | As per the real A3000UX |
| `uae_cachesize` | `0` | JIT **off**. JIT + MMU panics the kernel |
| `uae_cpu_compatible` etc. | `false` | "More compatible" causes kernel panics and `sort: fatal: line too long` during package install |
| `hard_drive_0_controller` | `scsi6` | SCSI ID 6 is hard-coded in the installer |
| `hard_drive_0_type` | `rdb` | Without it the installer detects no disks |
| `network_card` | `a2065` | The only NIC AMIX supports out of the box |
| `uae_gfxcard_type` | `A2410` | Colour X; not exposed by FS-UAE's own `graphics_card` option |
| `uae_gfxcard_size` | `4` | **Required.** Defaults to 0, and the board is then silently never instantiated |

## Settings to avoid

**`uae_cpu_speed = max` breaks booting.** With the CPU unthrottled the WD33C93
never completes its handshake and the boot never leaves the SCSI probe: white
screen, no `MMU enabled` in the log. Leave the option out entirely.

**Do not put a second hardfile on the SCSI bus.** Any additional `hard_drive_N`
hangs the machine during the Kickstart bus scan, before AMIX gets control.
Verified by A/B with an otherwise identical config: two drives gave no
`MMU enabled` after 110 s; the boot disk alone reached it inside 70 s. Tried
both with no RDB and with a valid AMIX-style RDB — the disk label is not the
issue. A *tape* at ID 4 is fine (see [Moving files in](07-Moving-Files-In.md)).

Both are findings from this build.

## Checking a boot

`68030 MMU enabled` in the log is the reliable signal that the kernel took over:

    grep -c "MMU enabled" ~/Documents/FS-UAE/Cache/Logs/fs-uae.log.txt

**Do not diagnose with `SCSI command buffer overflow!`.** It floods the log —
gigabytes per session — during hung *and* healthy boots alike. It is noise.

Boot takes roughly 70 s, or 145 s with the tape attached; the extra time is the
bus probe. Expect one `CPU reset` partway through, which is the normal
two-stage bootstrap.
