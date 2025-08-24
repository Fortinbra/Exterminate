# Step 2 – Eyestalk Assembly

> Objective: Build and verify the eyestalk LED subassembly and prepare it for later integration with the dome and control electronics.
> Prerequisite: All printed eyestalk parts and small hardware gathered in Step 1.

> Geometry Clarification: The 3 mm tube (with the decorative `discs.stl` piece) remains fully exposed along its length. The printed `eye_stalk_A.stl` and `eye_stalk_B.stl` parts are only short mounting tabs/collars at the rear (opposite the eyeball) that secure the tube to the dome later. They do NOT form an enclosing sheath around the tube. At this stage the only enclosed element is the LED housed inside the eyeball shell.

## 1. Parts & Materials

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | eyeball_A.stl | 1 | Front half |
| Printed | eyeball_B.stl | 1 | Back half |
| Printed | eye_stalk_A.stl | 1 | Left stalk half |
| Printed | eye_stalk_B.stl | 1 | Right stalk half |
| Printed | discs.stl | 1 | Single printed cluster of aesthetic discs (slides onto tube) |
| Electronic | 3 mm blue LED | 1 | Forward voltage ~2.8–3.2 V |
| Electronic | 30 AWG silicone wire (blue) | 1 length | Eye LED anode (+) lead (tinned both ends) |
| Electronic | 30 AWG silicone wire (black) | 1 length | Ground / cathode lead (tinned both ends) |
| Proto | 2‑hole perf board snippet | 1 | Just two copper pads inline |
| Mechanical | 3 mm OD tube (SS or brass) | 1 (≈40 mm) | Shaft passes wires |
| Protective | Liquid electrical tape / conformal coat | As needed | Insulation |
| Optional | 1.5–2 mm heat shrink | 2 short pieces | Strain relief (alternative to perf) |

## 2. Tools

- Fine tip temperature‑controlled soldering iron
- Rosin core solder (0.5–0.7 mm)
- Flush cutters (sharp; for lead trimming)
- Tweezers or small needle nose pliers
- Magnification (head loupe) (recommended)
- Isopropyl alcohol + lint‑free swab (flux cleanup)
- Small brush / applicator for liquid electrical tape (if not built-in)

### Color Convention

For this build: all grounds / cathodes are BLACK. The eye LED anode uses a BLUE wire to visually match and simplify later identification when routing through the stalk and into the dome harness. (General +3V3 distribution elsewhere may still use red, but not for this specific subassembly.)

## 3. Pre‑Check

1. Dry‑fit the 3 mm tube in the stalk halves to confirm it seats fully and is not pinched.
2. Pass both wire lengths through the tube (without LED) to verify they slide freely. If tight, deburr tube ends and gently ream internal burrs with a fine needle.
3. Confirm LED polarity: longer lead = anode (+), flat side on rim = cathode (−).

## 4. LED Perf Subassembly

Follow this exactly; cramped tolerances later make rework tedious.

1. Prepare perf snippet: Cut / snap a fragment containing exactly two copper pads in a single row. De-burr edges so it sits flat.
2. Tin each copper pad lightly (just a sheen). Avoid creating a solder dome that would prevent the LED from seating flush.
3. Insert the 3 mm blue LED leads through the two perf holes from the non‑copper side so the LED body sits flush against the bare (non‑copper) face. Keep the LED centered and perpendicular.
4. Solder the LED leads on the copper side. Use minimal solder; you only need full wetting of pad + lead.
5. Trim both LED leads as close to the solder fillet as possible (nearly flush) using sharp flush cutters. Do not stress the solder joints—support the LED body while cutting.
6. Pre‑tin ~2–3 mm of one end of each 30 AWG wire (blue = anode, black = cathode). Keep the tin layer thin to ease insertion.
7. Insert the pre‑tinned wire ends into the SAME holes already occupied by the trimmed LED leads (you are essentially stacking into the remaining pad barrel). This can be fiddly:
   - Slightly angle the tinned end and rotate while applying gentle pressure.
   - If hole is fully filled, briefly reflow the solder with the iron while nudging wire in.
8. Once each wire end is partially seated, touch the soldering iron to reflow the existing joint; feed in a *very* small amount of solder only if necessary to achieve a smooth fillet.
9. Inspect joints (magnification): Both wires must be mechanically secure, no stray whiskers, and no solder bridges between pads.
10. Gently pull each wire (tug test). If movement occurs, reflow and add a trace more solder.

## 5. Insulation / Protection

1. Clean flux residue with a swab and isopropyl alcohol; dry thoroughly.
2. Apply a thin coat of liquid electrical tape or conformal coating over the exposed pad area and trimmed lead stubs. Ensure full coverage between the two pads and up the wire insulation transition.
3. Allow the coating to cure per manufacturer instructions (usually a few minutes tack, longer full cure). Avoid thick blobs that could increase diameter excessively.
4. Optional: Instead of coating, slip pre‑cut micro heat‑shrink over each joint before soldering (requires longer remaining LED leads) — not usually feasible with this minimal perf approach.

Rationale: The coated underside prevents accidental shorting against the interior of the metal tube or any conductive debris while keeping overall diameter minimal.

### Electrical Checkpoint 1 – Post LED / Perf / Insulation

Before enclosing anything inside the eyeball, do a quick live test:

1. Attach a current‑limited 3.3 V source (or bench supply set to 3.3 V, 20 mA limit) through a 150 Ω series resistor to the LED leads (BLUE = + via resistor, BLACK = −).
2. Verify steady illumination (no flicker when gently flexing wires). If intermittent, reflow joints now—much easier before mechanical integration.
3. Measure voltage across LED (expect ~2.8–3.2 V). Significantly lower may indicate a partial short; significantly higher means the resistor is not truly in series (re‑check test setup).
4. Disconnect power before proceeding.

## 6. Eyeball Assembly & Tube Integration

1. Seat the LED (with perf snippet) into the opening in `eyeball_A.stl` (front). It should be snug; lightly relieve the hole ONLY if the LED will not press in square. Do not force—avoid cracking the shell.
2. Test that the BLUE (anode) and BLACK (cathode) wires can pass cleanly through the rear opening in `eyeball_B.stl` without sharp bends. Enlarge the channel very slightly with a needle file if insulation scuffs.
3. Feed both wires through the hole in `eyeball_B.stl` then bring the two halves together, ensuring the LED remains centered and there is zero wire strain when the halves touch.
4. Hold the halves together and verify the seam alignment. Adjust LED depth if necessary so the lens sits correctly relative to the front surface.
5. Keeping the eyeball halves aligned, feed the free wire ends DOWN through the 3 mm tube until the tube reaches the rear opening of `eyeball_B.stl`.
6. Slide the single printed `discs.stl` piece (represents multiple stacked discs) onto the tube now. It is purely decorative (not a spacer) and must be installed before the tube is later CA‑bonded into the dome (cannot be added afterward).
7. Insert the tube fully into the rear eyeball hole; the perf board/LED should not twist. If the coating snags, withdraw slightly, re‑orient, and re‑insert with a gentle rotation.
8. Confirm: (a) LED centered, (b) halves fully closed, (c) no tension on wires, (d) tube seated flush or to designed depth, (e) discs positioned as desired.
9. Bond the eyeball halves: apply a minimal amount of thin CA glue around the interior seam (capillary method). Avoid flooding to prevent frosting. Optionally use activator sparingly *after* excess fumes dissipate.
10. Wick a tiny drop of CA where the tube meets the rear hole to lock it (avoid wicking CA onto wires). Let cure fully.
11. Electrical Checkpoint 2 – Post Tube Bond: Re‑test LED exactly as in Checkpoint 1. If the LED now fails or flickers, suspect CA migration, mechanical strain on a joint, or a short to the tube (inspect coating edge). Rework before continuing.
12. Do NOT yet glue the eyeball/stalk assembly into the dome; that occurs in the dome step.

Tip: To reduce CA fogging on the lens, keep airflow moving (small fan) and avoid enclosing the assembly while curing.

## 7. Stalk Mount Tabs Bonding

With the eyeball and tube already unified and discs installed, permanently bond the two small rear mounting tab pieces (they do not cover the tube length):

1. Test fit again: position `eye_stalk_A.stl` (left) and `eye_stalk_B.stl` (right) around the rear end of the tube only, confirming the tube’s seating feature engages and wires exit without pinch.
2. Lightly deburr any internal print artifacts that touch the discs or eyeball—remove ONLY what interferes.
3. (Optional) Lightly scuff (600–800 grit) the tube contact pads inside the halves for better CA adhesion; blow away dust.
4. Route the BLUE and BLACK wires straight out the rear channel; add a tiny piece of low‑tack tape to keep them centered if they spring sideways.
5. Apply small dots / short beads of medium (not thin) CA to: alignment posts, perimeter mating ribs, and the tube capture features inside the tab pieces. Avoid excess that could wick along the exposed tube toward the discs.
6. Close the halves starting from the eyeball end, ensuring seams align. Squeeze gently; do not torque. Wipe any squeeze‑out immediately with a clean swab (no accelerator yet).
7. Verify: (a) eyeball still centered, (b) decorative discs untouched and free to remain visually even, (c) tube fully seated front to back in the rear collar, (d) wires move freely when gently pulled.
8. Wick a *tiny* amount of thin CA at the rear tube exit if additional locking is desired—keep adhesive off wires.
9. Optionally apply accelerator sparingly from the *outside* after fumes have partially dissipated to reduce frosting risk.
10. Hold or tape in position until initial set; then let cure per adhesive spec before handling (~5–10 min typical for full handling strength).
11. Do NOT snap into the dome yet—dome integration occurs in the dedicated dome step to keep access for wiring. The exposed tube + discs should remain clean; avoid handling with adhesive on fingers.

## 8. Final Electrical Test (After Stalk Mount Tabs Bonded)

- Perform a final illumination check after Section 7 bonding to ensure no stress introduced a latent fault.
- Optional: gently rotate the eyeball while powered (without twisting wires) to be sure movement does not induce flicker.

## 9. Staging for Next Steps

- Coil and label the two wire leads (masking tape flag: "Eye + (Blue) / Eye − (Black)"). Recommended length to leave untrimmed: ≥150 mm until harness planning.
- Visually confirm no CA bloom on the lens; if present, very lightly polish later (do not attempt now while joints are green).

## 10. Troubleshooting Quick Reference

| Symptom | Likely Cause | Action |
|---------|--------------|--------|
| LED dead | Reversed polarity or cold joint | Reflow joints; verify anode to blue |
| LED intermittent | Hairline solder crack | Reflow & add minimal solder |
| Short to tube | Coating incomplete | Add second thin insulation layer |
| Wires won’t insert | Hole fully filled with solder | Reflow while gently pushing wire |
| High resistance dim LED | Excessively long thin wire / poor joint | Reflow; verify supply voltage |

## 11. Next Step

Proceed to Step 3 – Dome Assembly & LED Distribution Board.

---
Navigation: ← Previous [Step 1 – Gather / Print All Parts](./step1_parts.md) | Next → [Step 3 – Dome Assembly & LED Distribution Board](./step3_dome.md) | Index ↔ [Assembly Index](./README.md)

Revision: 0.2 (2025-08-24) Added revision line for consistency.
