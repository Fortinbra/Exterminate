# Step 3 – Dome Assembly & LED Distribution Board

> Objective: Install the dome lighting (two red LEDs), integrate the completed eyestalk wiring, and prepare a small perf board power distribution node while preserving serviceability for later neck and body integration.
> Prerequisite: Step 2 eyestalk assembly fully completed and electrically verified (both Electrical Checkpoints passed, wires labeled, minimum 150 mm free lead length).

## 1. Parts & Materials

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | dome.stl | 1 | Shell (surface finish priority) |
| Printed | light_x2.stl | 2 | Transparent / translucent diffusers |
| Printed | dome_stalk_ver3.stl | 1 | Eyestalk pass‑through / support |
| Printed | stalk_tab.stl | 1 | Rotation / mechanical drive tab |
| Printed | dome_nut.stl | 1 | Internal retainer / lock |
| Printed | led_adapter_3mm_5mm.stl | 2 | 3mm-to-5mm LED centering adapter (Printables 995139) |
| Electronic | 3 mm red LED | 2 | Dome indicators (Vf ~2.0 V) |
| Electronic | 150 Ω resistor | 3 | One per LED (low‑side / cathode) |
| Proto | Perf board 31 × 16 mm | 1 | 2.54 mm grid (trimmed) |
| Wire | Red wire (down harness) | 2 lengths | Anode lines to main PCB (Left / Right dome LEDs) |
| Wire | Blue wire (down harness) | 1 length | Eyestalk anode line to main PCB |
| Wire | Black wire (down harness) | 1 length | Common ground return (post‑resistor bus) |
| Wire | Red wire (short pigtails) | 2 short | LED anode pigtails from each red LED to board |
| Wire | Black wire (short pigtails) | 2 short | LED cathode pigtails (into individual resistors) |
| Wire | Blue wire (from Step 2) | 1 | Eyestalk anode lead (trim to board) |
| Wire | Black wire (from Step 2) | 1 | Eyestalk cathode lead (into its resistor) |
| Fastener | M2 × 6 mm screw | 2 | Board / internal hardware securing (if used) |
| Adhesive | Tiny CA / hot glue / UV resin | As needed | Board & diffuser retention |

Note: All three LEDs use low‑side (cathode) resistors located on this subboard. This permits individual anode control (blinking / modulation) from the main PCB with just four wires exiting the dome (3 anodes + 1 common ground).

## 2. Tools

- Soldering iron (fine tip)
- Rosin core solder (0.5–0.7 mm)
- Flux pen (optional but recommended)
- Flush cutters (leads + perf trim)
- Fine file / 400–600 grit sandpaper (edge deburr)
- Tweezers / small pliers
- Multimeter (continuity + voltage)
- Bench / current‑limited 3.3 V supply (or Pico + series resistors)
- Masking tape (wire labeling & temporary retention)

### Color Convention Recap

- BLUE: Eyestalk LED anode (+) (separate control line).
- RED (two lines): Left & Right dome LED anodes (+) (independent control possible).
- BLACK: Common ground return (after individual resistors).

## 3. Perf Board Design

Topology (Low‑Side Current Limiting):

- Each LED cathode connects via its own 150 Ω resistor to a common ground bus.
- All three resistor ground ends join to the single black ground down‑wire.
- Each LED anode remains isolated and leaves the dome on its own control wire (two red, one blue) for direct drive (active‑high) from the main PCB pins (or via transistors later if needed).

Layout Goals:

- Group three resistors in a row forming a neat barrier between LED cathode pigtails and ground bus.
- Provide clearly labeled through pads (or adjacent paired pads) for: A_RED_L, A_RED_R, A_EYE (blue), and GND.
- Maintain mechanical clearance so adapters / diffusers seat without pressing on solder joints.
- Keep at least one outer hole row free for anchoring glue / resin.

Wire Strategy:

- Short local pigtails (red = anode, black = cathode) run from each dome LED up at the diffuser position back to the perf board.
- Eyestalk wires (blue anode, black cathode) are fed through the rectangular opening (future stalk snap area) BEFORE snapping stalk in, trimmed, and soldered to board pads (blue to A_EYE pad, black to its resistor input pad).
- Four-wire down harness (2× red anodes, 1× blue anode, 1× black ground) exits through `dome_stalk_ver3` central channel.

Electrical Behavior:

- Driving a red LED: Set its anode line HIGH (3.3 V); current flows anode → LED → resistor → ground bus → common ground lead.
- Eye LED behaves identically on blue anode line.
- Typical current per LED ≈ (3.3 V − Vf_LED) / 150 Ω → red: (3.3 − 2.0)/150 ≈ 8.7 mA; blue eye (assuming Vf ≈ 2.9 V) ≈ 2.7 mA (slightly dim). Use 120 Ω for eye if higher brightness desired (optional note on board).

Expected Total Current (all on): ~20–24 mA depending on chosen eye resistor value.

## 4. Assembly Steps

1. Cut perf board to 31 × 16 mm; deburr edges (no copper burrs to nick insulation).
2. Plan pad usage: reserve one edge row for common ground bus; mark three adjacent isolated pads (or pad pairs) for anodes A_RED_L, A_RED_R, A_EYE.
3. Pre‑tin pads for the three resistors and the ground bus.
4. Prepare the two red LEDs: solder a short red pigtail to each anode and a short black pigtail to each cathode (long lead = anode). Keep pigtails just long enough to comfortably reach board (avoid excess bulk). Optional heat shrink on individual joints.
5. Insert each LED into its `led_adapter_3mm_5mm.stl`, then slide that adapter/LED subassembly into a `light_x2.stl` diffuser. Confirm snug but not forced.
6. Press‑fit each populated diffuser (light) into its hole on the dome top from the outside. Lenses/adapters should sit flush. If resistance is high, lightly relieve the dome hole—not the diffuser—until smooth press fit.
7. (Optional) Apply a tiny fillet of thin CA from the INSIDE perimeter of each hole to lock the light in place. Avoid wicking onto lenses.
8. Position the perf board loosely inside the dome; route the four LED pigtail pairs (2× red/black) to their intended pads without crossing sharply.
9. Solder each black cathode pigtail to one end of its own 150 Ω resistor; solder the opposite end of each resistor to the forming ground bus (do NOT connect the ground down‑wire yet).
10. Solder each red anode pigtail to its labeled anode pad (A_RED_L / A_RED_R). Keep pads isolated (no solder bridges).
11. Feed the eyestalk wires (blue anode, black cathode) ONLY through the rectangular opening where the stalk will later snap (do not snap stalk yet). Trim to length while leaving ~10–15 mm service slack to the board.
12. Solder eyestalk black (cathode) to the third resistor input; solder that resistor’s output to the ground bus.
13. Solder eyestalk blue (anode) to pad A_EYE.
14. Install the four down‑harness wires: two RED (from A_RED_L and A_RED_R pads), one BLUE (from A_EYE pad), and one BLACK from the common ground bus. Provide strain relief (small adhesive dab or short heat‑shrink over pad + insulation).
15. Electrical Checkpoint 1 (Board + All LEDs): Apply 3.3 V individually (one at a time) to each anode down‑wire while grounding the black common. Verify correct brightness; no other LEDs should glow faintly. Log current per LED.
16. After successful test, SNAP the eyestalk assembly into the rectangular opening. This can require firm, even pressure and slight rocking. Support the dome to avoid flex cracks. Ensure existing wires are not pinched and have relief.
17. Orient `dome_stalk_ver3.stl` under the dome so that the curved slit for the neck pin is to the LEFT of the eyestalk when looking down along the eyestalk facing forward. Route the four-wire harness down the center channel.
18. Secure `dome_stalk_ver3.stl` with two M2 × 6 mm screws. Tighten only until snug—do NOT over‑torque (threads are in printed plastic and can strip). If resistance spikes, back out and clear debris.
19. Electrical Checkpoint 2 (Post Snap & Stalk Install): Repeat quick per‑anode test; gently flex stalk and harness. No flicker permitted.
20. Electrical Checkpoint 3 (Final Dome Subassembly): Power all three anodes together briefly; verify combined current in expected range. Observe for thermal issues (none expected at <30 mA total).

Tip: Keep adhesive away from LED lens edges to prevent diffusion artifacts.

## 5. Wiring Reference

| Wire | Origin | Destination (Later) | Notes |
|------|--------|---------------------|-------|
| Red Anode L | A_RED_L pad | MCU pin / driver | Left dome LED control (active HIGH) |
| Red Anode R | A_RED_R pad | MCU pin / driver | Right dome LED control (active HIGH) |
| Blue Anode Eye | A_EYE pad | MCU pin / driver | Eyestalk LED control (active HIGH) |
| Black GND | Ground bus (post 3 resistors) | Main PCB ground | Common cathode return |

## 6. Verification Checklist

| Item | Pass Criteria |
|------|---------------|
| Red LEDs polarity | Each illuminates alone when its anode is driven HIGH (others off) |
| Individual resistors | Three total: one per LED on cathode side |
| Eye LED resistor | Present on board (no external series R required) |
| Isolation | No ghosting when only one anode energized |
| Current draw | Per LED within spec (≈8–9 mA red, expected eye value) |
| Mechanical clearance | Board secure; adapters & diffusers not stressing leads |
| Light retention | Lights firmly seated; optional CA fillet applied internally only |
| Eyestalk snap fit | Fully seated; no wire pinch; orientation correct |
| Stalk orientation | Curved slit on `dome_stalk_ver3` is left of eyestalk (forward view) |
| Wire freedom | Movement causes zero flicker |
| Diffuser seating | Uniform glow; no major edge leaks |

## 7. Troubleshooting

| Symptom | Likely Cause | Action |
|---------|--------------|--------|
| One red LED dark | Reversed polarity or cold joint | Reflow / correct orientation |
| All LEDs dark | Missing ground or test powering method | Verify ground continuity & correct anode test point |
| One red LED dark | Open anode wire or cold solder at resistor | Reflow / continuity test anode path |
| Eye LED dark | Open anode or cathode resistor joint / stalk pinch | Reflow joint; check wire not crushed under snap |
| Light assembly loose | Insufficient press fit / no internal CA | Add tiny internal CA fillet (avoid lens) |
| Stalk misaligned | Incorrect stalk base orientation | Remove stalk base, rotate so slit is left of eyestalk, re‑secure |
| Excess current draw | Resistor shorted or wrong value | Measure resistor; replace |
| Ghosting (faint glow) | Leakage path / flux residue | Clean board; ensure anodes isolated |
| Flicker on movement | Strain on solder joint / insufficient relief | Add strain relief / reflow cracked joint |
| Uneven brightness | Mismatched resistor values | Measure resistors; replace incorrect |

## 8. Staging for Next Step

- Do NOT install `stalk_tab.stl` or `dome_nut.stl` yet; these will be fitted during Neck Assembly (Step 4) to preserve access and alignment flexibility.
- Coil and temporarily secure harness with low‑tack tape inside dome cavity leaving ~80–100 mm protruding for neck routing.
- Document (photo) the board layout for future reference / troubleshooting before closing up.
- Confirm all labels legible.

## 9. Next Step

Proceed to Step 4 – Neck Stack & Dome Integration.

---
Navigation: ← Previous [Step 2 – Eyestalk Assembly](./step2_eyestalk.md) | Next → [Step 4 – Neck Stack & Dome Integration](./step4_neck.md) | Index ↔ [Assembly Index](./README.md)

Revision: 0.1 (2025‑08‑23) Initial draft integrated with Step system.
