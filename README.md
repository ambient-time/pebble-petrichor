# Petrichor

Three drops turn a puddle into a clock. H marks the hour, M the minute, and S
the second. Twelve standing rings around each drop fill as its value increases;
small numbers give the exact local hour, minute, and second.

The second emitter makes a small splash every second. The minute adds a larger,
slower splash at each minute boundary, and the hour sends out the largest wave
at the top of each hour. The waves spread through the other gauges and fade.

Round and rectangular screens use a triangle of emitters: hour at the upper
left, second at the upper right, and minute below. Color watches use cyan rings
on black. Monochrome watches use solid filled rings, dotted empty rings, and a
partial arc for a fraction of a ring.

The face updates once a second. Tap your wrist for about six seconds of smoother
ripples, then it returns to its usual cadence. Leaving the face stops that timer.
It needs no phone connection, account, settings, or network access.

## Watches

| Target | Display |
| --- | --- |
| basalt | 144 × 168, color |
| chalk | 180 × 180, color |
| diorite | 144 × 168, monochrome |
| emery | 200 × 228, color |
| flint | 144 × 168, monochrome |
| gabbro | 260 × 260, color |

## Build and verify

Use Pebble SDK 4.33.1 and a host C compiler. Normal builds use the checked-in
resources. Python 3 with Pillow regenerates
the menu icon. The face has no external font or library dependencies.

```sh
make test
pebble build --sdk 4.33.1
```

Pebble names the output after the project directory. With this directory named
`petrichor`, the result is `build/petrichor.pbw`. The source ZIP contains the
project at its root and can build outside the Ambient Time checkout.

The native verification scripts use the Python environment containing
`pebble-tool`. Run each target before packaging:

```sh
python test/emulator.py basalt --fresh
python test/emulator.py chalk --fresh
python test/emulator.py diorite --fresh
python test/emulator.py emery --fresh
python test/emulator.py flint --fresh
python test/emulator.py gabbro --fresh
python test/package.py
python test/independent.py --pebble pebble
```

Each capture uses a fresh local emulator profile. The script verifies process
ownership before closing it. Checks cover installation, logs, changing seconds,
natural noon and midnight rollovers, a wrist tap, and the return to one update
per second. Packaging verifies every target's application and resource CRC,
metadata, and evidence hash. [Evidence notes](evidence/README.md) describe the
results and remaining physical-watch checks.

The release directory holds the versioned PBW, independent source ZIP, and
SHA-256 checksums.

## Source and adaptation

Luke Steuber created [Petrichor](https://datapoems.io/clocks/petrichor/)
and this Pebble port. The supplied HTML and screenshot are in `reference/`;
[provenance](reference/provenance.json) records their exact file hashes.

The port keeps the twelve-ring gauges, 24-hour mapping, three splash sizes,
three propagation speeds, quadratic fades, and three trailing crests per wave.
Ripple ages come directly from the local clock. A resumed face therefore shows
the waves that belong to that time without building a backlog of missed drops.
At most six waves can be alive together. Host tests compare 691,200 time samples
with the source equations and check gauge values and label bounds.

The native triangle leaves space for all three gauges and their labels. Round
gauges use 17% of the shorter screen dimension as their radius; rectangular
gauges use 20.5%. The exact numbers remain beneath each emitter, as in the
source's embedded layout. Small bitmap lettering replaces the browser font.

Cyan brightness steps replace translucent strokes and additive blur on color
screens. The faint water gradient becomes black. Monochrome screens distinguish
passing crests from standing gauges with sparse dots. The brief bead pulse uses
a clock-based linear fade instead of a frame-count decay. The browser's
continuous animation becomes one-second samples with optional tap motion.
At that default cadence, successive second splashes form a repeating pattern;
the second gauge still advances, and minute and hour waves keep spreading.

The package UUID is `4150a96f-88c8-48cd-ad81-74a1fc34c8c7`.
Keep it unchanged when updating this face.

Copyright 2026 Luke Steuber. [MIT License](LICENSE).
