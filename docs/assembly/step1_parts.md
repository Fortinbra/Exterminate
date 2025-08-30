# Step 1 – Gather / Print All Parts

> Objective: Produce all required 3D printed components and sort them for later assembly.
> Model Sources: Main body [Printables 1351261](https://www.printables.com/model/1351261-animated-dalek-with-sound) + RC / alternate [Printables 1389560](https://www.printables.com/model/1389560-rc-dalek-with-sound)

## 1. Overview
 
| Group | Representative Parts | Qty | ALT |
|-------|----------------------|-----|-----|
| Dome Assembly | dome.stl, light_x2.stl, dome_stalk_ver3, stalk_tab, dome_nut | 1 set | — |
| Eyestalk Assembly | eyeball_A/B, eye_stalk_A/B, discs | 1 set | — |
| Neck Assembly | neck_a–f, neck_pin | 1 set | — |
| Shoulders & Appendages | shoulders(.no_holes), long_slat_x17, short_slat_x7, ball_clamp, ball_x4, gun_tubes, plunger | 1 set | Smooth variant |
| Skirt & Hemispheres | skirt.stl, hemi_x65 | 1 set | Filename typo: prints single hemisphere (need 64) |
| Fender & Cam Mechanism | fender, motor_mount, cam_1/2, cam_cap, pushrod_a/b, cam_motor_clamp | 1 set | — |
| Base & Drive Platform | base_cover, drive_gear x2, wheel_gear x2, bearings, gears, electronics | 1 set | — |

### Component Breakdown

#### Eyestalk Assembly (Printed + Supporting Parts)

Printed parts (remixed model):

- `eyeball_A.stl`
- `eyeball_B.stl`
- `eye_stalk_A.stl`
- `eye_stalk_B.stl`

Clarification:

- `eyeball_A.stl` and `eyeball_B.stl` are the front and back halves of the eyeball shell.
- `eye_stalk_A.stl` and `eye_stalk_B.stl` are the left and right stalk body halves that enclose the wiring and support the eye.

Printed part (original model set):

- `discs.stl` (eye discs / spacers)

Non‑printed / hardware:

- 1 × 3 mm blue LED (rated for 3.3 V logic supply; typical Vf 2.8–3.2 V @ 5–10 mA)
- 2 × lengths 30 AWG silicone or enamel wire (color pair, e.g. Red = +3V3, Black = GND) – length TBD based on routing (recommend ≥150 mm to allow slack)
- 1 × proto / perf board scrap trimmed to just 2 holes (inline) for LED lead support or strain relief (optional if heat‑shrink used)
- 1 × 3 mm OD tube, 40 mm long (eyestalk shaft)
	- Stainless steel: 0.6 mm wall (rigid, durable)
	- Brass: 0.3 mm wall (lighter, easier to cut / deburr)

Purpose / notes:

- The 2‑hole perf snippet provides a mechanical anchor so LED leads aren’t stressed when routing through the tube.
- Choose tube wall thickness balancing stiffness vs. internal clearance (thinner wall = more internal diameter margin for wire pair if tolerance is tight).
- Pre‑tin LED leads, slide heat shrink, then feed wires through tube before final solder if clearance is tight.

Eyestalk BOM quick reference:

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | eyeball_A.stl | 1 | Front half (remix) |
| Printed | eyeball_B.stl | 1 | Rear half (remix) |
| Printed | eye_stalk_A.stl | 1 | Left stalk half (remix) |
| Printed | eye_stalk_B.stl | 1 | Right stalk half (remix) |
| Printed | discs.stl | 1 set | Eye discs/spacers (original) |
| Electronic | 3 mm blue LED | 1 | 3.3 V rail (with 150 Ω resistor) |
| Electronic | 30 AWG wire (red) | 1 length | Power lead |
| Electronic | 30 AWG wire (black) | 1 length | Ground lead |
| Proto | 2‑hole perf snippet | 1 | Optional support |
| Mechanical | 3 mm tube SS 0.6 mm wall | 1 (40 mm) | Rigid option |
| Mechanical | 3 mm tube Brass 0.3 mm wall | 1 (40 mm) | Lighter alternative |

 

#### Dome Assembly (Printed + Supporting Parts)

Printed parts (original model set):

- `dome.stl`
- `light_x2.stl` (print 2× – left/right dome light covers)
- `dome_stalk_ver3.stl` (stalk mount / pass‑through)
- `stalk_tab.stl` (dome rotation drive tab)
- `dome_nut.stl` (locking nut / internal retainer)

Recommended materials:

- Transparent PETG (or clear PLA) for `light_x2.stl` diffusers
- PLA / PLA+ for dome shell and stalk (focus on surface finish; enable concentric top layers)

Electronics & wiring:

- 2 × 3 mm red LEDs (3.3 V logic rail, each individually resistor‑limited)
- 3 × 150 Ω resistors (one per LED: two dome reds + blue eyestalk)
- 1 × Perf / proto board 31 mm × 16 mm (trimmed to size; 2.54 mm grid)
- Wire lengths (choose slack to reach main PCB comfortably):
	- 1 × Blue wire (eye LED positive feed passes through stalk into dome board)
	- 2 × Red wires (left/right dome LED anode feeds from board out if needed OR directly from board V+ distribution)
	- 1 × Black wire (common ground return to main PCB; eye LED cathode and both dome LED cathodes join here)

Wiring concept:

- Central perf board inside dome acts as small distribution node.
- Eye LED wires route up through `dome_stalk_ver3` into the dome; eye positive (blue) passes through its own 150 Ω resistor before joining +3V3 bus.
- Both dome LEDs each get their own 150 Ω resistor from +3V3 bus to anode; cathodes tied to ground bus.
- Common ground (black) and +3V3 (one of the red wires) exit downward with the eye LED positive (blue) if that LED is resistor‑limited on main PCB instead of in the dome.
- Keep wires centered to avoid chafing during potential dome rotation.

Perf board layout tip:

- Reserve one long edge for +3V3 bus (joined resistor leads) and opposite edge for ground bus.
- Place LEDs near ends aligned to `light_x2` diffusers; keep at least one empty hole row outermost for mechanical glue or tape.

Dome sub‑BOM quick reference:

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | dome.stl | 1 | Shell; smooth finish priority (original) |
| Printed | light_x2.stl | 2 | Transparent PETG diffusers (original) |
| Printed | dome_stalk_ver3.stl | 1 | Eyestalk mount / wire channel |
| Printed | stalk_tab.stl | 1 | Dome rotation drive tab |
| Printed | dome_nut.stl | 1 | Internal locking piece |
| Electronic | 3 mm red LED | 2 | ~2.0 V Vf; individual resistors |
| Electronic | 150 Ω resistor | 3 | One per LED (2× red, 1× blue) |
| Proto | Perf board 31×16 mm | 1 | Trim to 31×16 mm |
| Wire | Red wire | 2 lengths | +3V3 feeds (or bus + spare) |
| Wire | Blue wire | 1 length | Eye LED positive feed |
| Wire | Black wire | 1 length | Common ground return |
| Fastener | M2 × 6 mm screw | 2 | Secures internal dome hardware |

Notes:

- Place the eye LED's 150 Ω resistor near the stalk wire entry for easy identification.
- Color coding improves rapid identification during later wiring and troubleshooting.
- Keep total current budget in mind (two red LEDs ≈ 16–18 mA typical; add eye LED accordingly).

#### Neck Assembly (Printed Parts)

All neck components come from the original model set and are named sequentially:

- `neck_a.stl`
- `neck_b.stl` (transparent PETG)
- `neck_c.stl`
- `neck_d.stl` (transparent PETG)
- `neck_e.stl`
- `neck_f.stl` (transparent PETG)
- `neck_pin.stl` (rotation limit pin)

Material guidance:

- Transparent (or translucent) PETG for `neck_b`, `neck_d`, `neck_f` to visually differentiate layers and allow for potential future illumination.
- PLA / PLA+ (matte black recommended) for `neck_a`, `neck_c`, `neck_e` for contrast and dimensional stability.

Tips:

- Maintain consistent cooling on PETG rings (fan ~40–50%) to reduce haze.
- Keep perimeter speed slower (~25 mm/s) for optical clarity on transparent parts.
- Dry-fit stack with dome stalk to ensure unobstructed central channel if wiring / rotation planned.

Neck mini‑BOM quick reference:

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | neck_a.stl | 1 | Base segment (PLA+ black) |
| Printed | neck_b.stl | 1 | Diffuser ring (transparent PETG) |
| Printed | neck_c.stl | 1 | Structural spacer (PLA+ black) |
| Printed | neck_d.stl | 1 | Diffuser ring (transparent PETG) |
| Printed | neck_e.stl | 1 | Structural spacer (PLA+ black) |
| Printed | neck_f.stl | 1 | Top ring (transparent PETG) |
| Printed | neck_pin.stl | 1 | Limits dome rotation (PLA+ black) |

#### Shoulders & Appendage Mounts

The shoulder section supports the arm (plunger) and gun assemblies plus the skirt slat array (if using the version with holes).

Printed shell choice (original model set):

- `shoulders.stl` (standard, includes slat mounting interface)
- `shoulders_no_holes.stl` (use if you prefer a smooth shoulder section with no external slats)

If you choose `shoulders.stl`, also print the slats:

- `long_slat_x17.stl` (print 17)
- `short_slat_x7.stl` (print 7)

Appendage / weapon system printed parts:

- `ball_clamp.stl`
- `gun_tubes.stl`
- `ball_x4.stl` (print 4 – for multi‑ball joint assembly)
- `plunger.stl`

Non‑printed mechanical:

- 1 × 3 mm OD tube, 75 mm length (plunger or gun support shaft)
- 1 × 3 mm OD tube, 57 mm length (secondary shaft)
	- Same material guidance as eyestalk: stainless (rigid) or brass (easy to cut). No wiring runs inside these; deburr ends for clean fit and safety.

Material guidance:

- Shoulders shell: PLA+ (color: bronze / gold or body color) for surface finish.
- Slats: Match or accent metallic; batch print for consistency. Consider light sanding prior to paint.
- Ball joints / clamp: PETG or PLA+; PETG offers a bit more toughness for moving joints.

Assembly planning notes:

- Decide early whether you will install slats; this affects post‑processing (filling holes vs. leaving them open).
- Print a single long and short slat first to validate fit before committing to full batch.
- Keep tube stock slightly longer until dry‑fit; trim to exact length after confirming internal clearances.

Shoulders mini‑BOM quick reference:

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | shoulders.stl | 1 | With slat holes (choose OR shoulders_no_holes) |
| Printed | shoulders_no_holes.stl | 1 | Smooth variant (omit slats) |
| Printed | long_slat_x17.stl | 17 | Long slats (only with shoulders.stl) |
| Printed | short_slat_x7.stl | 7 | Short slats (only with shoulders.stl) |
| Printed | ball_clamp.stl | 1 | Holds ball assembly |
| Printed | ball_x4.stl | 4 | Multi‑ball joint parts |
| Printed | gun_tubes.stl | 1 | Barrel cluster |
| Printed | plunger.stl | 1 | Plunger assembly |
| Mechanical | 3 mm tube (75 mm) | 1 | Stainless or brass |
| Mechanical | 3 mm tube (57 mm) | 1 | Stainless or brass |
| Fastener | M3 × 8 mm screw | 3 | Shoulder assembly fastening |

Notes:

- Omit unused slat prints if selecting the no‑holes variant to save time and material.
- Track slat quantities in batches (e.g., print 4–6 at a time) to catch dimensional drift early.
- Lightly chamfer tube ends after cutting for safer handling.

#### Skirt & Hemispheres

The lower skirt forms the primary body silhouette and carries the decorative hemispheres.

Printed parts (original model set):

- `skirt.stl`
- `hemi_x65.stl` (source filename typo; prints single hemisphere – print 64 total + 4–8 extras)

Guidance:

- Batch hemispheres in plate layouts that avoid heat build‑up (add a sacrificial tower if needed for consistent cooling and gloss).
- Use a consistent filament lot for visible hemis to minimize color variation.
- Consider printing test hemis in draft to dial in surface finish before committing to full runs.

Skirt mini‑BOM quick reference:

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | skirt.stl | 1 | Main lower body section |
| Printed | hemi_x65.stl | 64 (+ spares) | Filename typo retained; print multiple |
| Fastener | M3 × 8 mm screw | 2 | Skirt attachment |

Notes:

- Extra hemis save time later when a few inevitably roll away or get sanding blemishes.
- Light post‑processing (deburr base edge) improves adhesion when press‑fitting or gluing to skirt panels.

#### Fender & Drive Cam Mechanism

The fender (from the remix) provides the lower perimeter and supports the motion cam system (original model parts) used for animating movement (e.g., head or accessory motion depending on configuration).

Printed parts:

- `fender.stl` (remix model)
- `motor_mount.stl` (original model)
- `cam_1.stl` (original model)
- `cam_cap.stl` (original model)
- `cam_2.stl` (original model)
- `pushrod_a.stl` (original model)
- `pushrod_b.stl` (original model)
- `cam_motor_clamp.stl` (original model)

Guidance:

- Print functional cam and pushrod parts in PETG or ABS for improved wear resistance vs. standard PLA.
- Ensure smooth cam lobe surfaces; consider slight horizontal expansion compensation if fit is tight.
- Dry-fit pushrods to cams before committing to final assembly; lightly ream pivot holes instead of drilling to maintain concentricity.

Fender / Cam mini‑BOM quick reference:

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | fender.stl | 1 | Lower perimeter piece (remix) |
| Printed | motor_mount.stl | 1 | Cam drive motor mount |
| Printed | cam_1.stl | 1 | Primary cam |
| Printed | cam_cap.stl | 1 | Cam retainer |
| Printed | cam_2.stl | 1 | Secondary cam (if used) |
| Printed | pushrod_a.stl | 1 | Pushrod segment A |
| Printed | pushrod_b.stl | 1 | Pushrod segment B |
| Printed | cam_motor_clamp.stl | 1 | Secures motor to mount |
| Fastener | M3 × 8 mm screw | 6 | Mounting fender & cam assembly |

Notes:

- Lightly lubricate cam contact surfaces with a plastic‑safe dry lube if motion feels rough.
- Reprint individual cam pieces rather than sanding excessively if geometry accuracy is critical; dimensional integrity affects motion timing.

#### Base & Drive Platform

The base houses propulsion, power, audio, and control electronics.

Printed parts:

- `base_cover.stl`
- `drive_gear.stl` (print 2)
- `wheel_gear.stl` (print 2)

Mechanical / drivetrain:

- 1 × 3 mm OD tube, 60 mm length (axle / spacer)
- 4 × 683ZZ ball bearings (3×7×3 mm)
- 2 × N20 gear motors, 3 V, 1000 RPM (ensure identical ratio)

Audio subsystem:

- 1 × 28 mm 8 Ω 2 W speaker
- 1 × Adafruit I2S 3 W Class D Amplifier (MAX98357A breakout — see [Adafruit product 3006](https://www.adafruit.com/product/3006))

Motor / power electronics:

- 1 × Pimoroni Pico LiPo 2 XL W (controller board variant)
- 1 × Motor SHIM for Pico (Pimoroni)
- 1 × Motor Connector Shim (MCS) (pack of 2) (use as needed)
- 1 × Motor Connector Shim Cable pack (2 cables, 150 mm) (one per motor)
- 1 × Adafruit MOSFET Driver (STEMMA JST PH 2 mm) (for higher current loads or lighting/actuators — see [Adafruit product 5648](https://www.adafruit.com/product/5648))

Power storage:

- 1 × LiPo 3.7 V 2500 mAh battery (ensure connector compatibility or adapt)

Wiring / assembly guidance:

- Verify motor shaft diameter matches printed gear bore; lightly ream if tight—avoid force that could bend motor shaft.
- Test‑fit bearings in printed pockets: aim for snug push fit; adjust horizontal expansion settings if loose.
- Keep audio amplifier physically isolated from high current motor leads to reduce EMI; twist motor lead pairs.
- Plan battery placement for center of gravity (low and centered) and accessible charging port / switch.
- Provide strain relief for LiPo leads; never pinch between printed parts.

Base mini‑BOM quick reference:

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | base_cover.stl | 1 | Structural base top/cover |
| Printed | drive_gear.stl | 2 | Drive gears |
| Printed | wheel_gear.stl | 2 | Wheel interface gears |
| Mechanical | 3 mm tube (60 mm) | 1 | Axle / spacer |
| Mechanical | 683ZZ bearing 3×7×3 | 4 | Wheel/support bearings |
| Drivetrain | N20 motor 3 V 1000 RPM | 2 | Matched pair |
| Audio | 28 mm 8 Ω 2 W speaker | 1 | Mount to printed bracket (future) |
| Audio | MAX98357A I2S amp | 1 | Adafruit breakout |
| Control | Pimoroni Pico LiPo 2 XL W | 1 | Main MCU board |
| Control | Motor SHIM for Pico | 1 | Motor interface |
| Control | Motor Connector Shim (MCS) | 1 (pack) | Provides motor headers |
| Control | MCS Cable 150 mm | 2 | Motor leads |
| Power | LiPo 3.7 V 2500 mAh | 1 | Main battery |
| Driver | Adafruit MOSFET Driver (STEMMA) | 1 | Auxiliary loads |
| Fastener | M3 × 8 mm screw | 4 | Base cover / structural points |
| Fastener | M2.5 × 8 mm screw | 4 | Board / shim mounting |
| Fastener | M2.4 hex nut | 4 | Captive nuts for M2.4 fasteners |
| Fastener | M1.7 × 4 mm screw | 4 | Small device / clamp mounting |

---
Navigation: Next → [Step 2 – Eyestalk Assembly](./step2_eyestalk.md) | Index ↔ [Assembly Index](./README.md)

Revision: 0.1 (2025-08-24) Standardized objective wording & added revision line.

