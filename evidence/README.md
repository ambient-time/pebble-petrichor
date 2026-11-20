# Petrichor evidence

The production PBW passed installation and native checks on all six targets.
`validation.json` records its hash, rendering samples, free heap, and cadence.
Each target report includes native logs and capture paths.

I inspected hero, quarter-hour, evening, noon-rollover, and midnight-rollover
captures on every target. All three circular gauges and their exact values fit
both display shapes. Cyan brightness separates filled and empty rings on color
screens. On monochrome screens, staggered dots keep empty rings circular while
solid rings show progress. The larger round display uses larger labels.
Noon reads H 12, M 00, S 00; midnight reads H 00, M 00, S 00.

Host tests cover 691,200 time samples plus seven exact wave-expiry boundaries.
They compare wave life, propagation, quadratic fade, and all three trailing
crests against the supplied source equations. Separate checks cover every
possible gauge value, all twelve ring fills, and six display layouts. Address
and undefined-behavior sanitizers pass.

Native checks confirm changing seconds, natural noon and midnight rollovers,
a tap burst, and the return to one redraw per second. The final Flint run
passed after one screenshot-connection timeout in an earlier profile. The
retry used the same production PBW; the failed attempt logged no app fault.

Emulator timing and heap samples describe these runs. Physical-watch battery
life, daylight readability, and wrist-tap sensitivity remain untested.
The one-second default samples a repeating second-wave pattern; brief tap
animation shows the intermediate spreading motion.

`independent-build.json` records a clean rebuild from the released source ZIP.
It compares every application byte apart from the SDK header CRC, resource
timestamp, and validated GNU build ID, and checks resource packs for exact
agreement. This proof stays outside the ZIP whose hash it records.
