# Step 7 – Fender Mechanism & Skirt Integration

> Objective: Assemble the fender cam mechanism (N20 motor, dual cams, pushrods, clamp), fasten motor mount to the fender, then stack the skirt/shoulder assembly ensuring arm ends seat into pushrod holes, securing the skirt to the fender.
> Prerequisites: Step 6 complete (skirt populated & shoulders attached). All printed mechanical parts present; N20 motor tested (brief spin) and wire leads accessible.

## 1. Parts & Materials

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | fender.stl | 1 | Base platform receiving motor mount & skirt |
| Printed | motor_mount.stl | 1 | Mounting interface (3 screws to fender) |
| Printed | cam_1.stl | 1 | Primary cam with shaft & flat reference (embossed line) |
| Printed | cam_2.stl | 1 | Secondary cam (sleeves over cam_1) |
| Printed | cam_cap.stl | 1 | Retains cam_2 axially (screwed) |
| Printed | pushrod_a.stl | 1 | Has distal hole that must face LEFT when installed |
| Printed | pushrod_b.stl | 1 | Upper hole must face RIGHT when installed |
| Printed | cam_motor_clamp.stl | 1 | Guides pushrods; clamps motor to mount (2 screws) |
| Assembly | Skirt + shoulders + arms (Step 6) | 1 | To stack over fender later |
| Electronic | N20 gearmotor | 1 | Shaft clean & free of burrs |
| Hardware | M3x8 screw | 3 | Motor mount → fender |
| Hardware | M3x8 screw | 1 | Cam cap → cam_1 shaft retention |
| Hardware | M3x8 screw | 2 | cam_motor_clamp → motor_mount |
| Hardware | M3x8 screw | 2 | Skirt → fender final attachment |
| Optional | Thread locker (low strength) | Trace | On cam cap screw if vibration expected |

Assumption: All M3 holes are printed undersized for thread forming; no inserts required.

## 2. Tools

- #1/#2 Phillips or hex driver (per screw head)
- Tweezers / needle-nose pliers (position pushrods)
- Small flat file (deburr cam shaft flat, if needed)
- Multimeter / bench supply (brief motor test)

## 3. Preparation

1. Test Motor: Apply low voltage (e.g., 3–5 V) momentarily—confirm smooth rotation, note direction for wiring later.
2. Inspect Cam Parts: Ensure cam_1 shaft flat has a crisp edge (embossed line indicates flat orientation). Lightly scrape artifacts.
3. Dry Fit: Slide cam_1 (no pushrods yet) onto motor shaft; verify firm friction fit without splitting. Remove.
4. Confirm Pushrod Orientation: Identify pushrod_a (later hole points LEFT) and pushrod_b (upper hole toward RIGHT). Mark lightly with pencil for quick reference.
5. Organize Screws: Separate 3+1+2+2 sets to avoid miscount during sequence.

## 4. Motor Mount to Fender

1. Position motor_mount on top of fender aligning the three mounting holes.
2. Insert the three M3x8 screws by hand 1–2 turns each (avoid cross‑threading).
3. Tighten incrementally in a triangle pattern until snug (do not over‑torque). Mount should sit flush—no rocking.

## 5. Cam & Pushrod Stack Assembly

Follow exact orientation—reversal will invert motion or cause interference.

1. Install cam_1: Align its internal flat to the motor shaft flat (embossed line on cam_1 indicates flat). Press fully home—shoulder flush.
2. Place pushrod_b onto cam_1: Seat over its eccentric post ensuring the UPPER hole of the pushrod faces to the RIGHT side of the assembly.
3. Slide cam_2 over the shaft of cam_1: Ensure it seats fully without pinching pushrod_b.
4. Add pushrod_a onto cam_2 (eccentric feature) so its distal hole (at the end) points to the LEFT side.
5. Fit cam_cap over the exposed end of cam_1 shaft, capturing cam_2 and pushrods.
6. Insert and tighten a single M3x8 into cam_cap (threading into cam_1). Tighten just until secure; optional trace of low‑strength thread locker if high vibration expected.

Inspection: Both pushrods should articulate independently on their respective cam lobes without binding when motor shaft is turned by hand.

## 6. Pushrod Channel Alignment (Clamp Installation)

1. Orient cam_motor_clamp so its slots align over both pushrods.
2. Gently slide the TWO pushrods through their respective slits/slots in the clamp. Ensure no twisting; rods should move freely vertically when cam rotated.
3. Keep clamp hovering (do not fasten yet) while confirming rods are not pre‑loaded sideways.

## 7. Motor & Clamp Fastening

1. Place N20 motor (with assembled cams & pushrods + loose clamp) into motor_mount pocket.
2. Lower cam_motor_clamp into final seated position atop motor_mount aligning its two screw holes.
3. Insert both M3x8 screws by hand 2–3 turns, then tighten alternately until snug (even pressure). Do not overtighten—avoid distorting clamp.
4. Rotate motor shaft manually (by turning accessible cam) through a full revolution—verify smooth pushrod reciprocation and absence of rubbing on clamp edges. Correct by slight pushrod centering if needed.

## 8. Skirt Stacking & Arm Engagement

Critical: Arm ends must insert into pushrod holes before securing skirt.

1. Position the completed skirt/shoulder assembly above the fender mechanism.
2. Lower carefully while guiding LEFT and RIGHT arm tube ends into the corresponding upper holes of pushrod_b (RIGHT‑facing hole) and pushrod_a (LEFT‑facing hole) respectively. (Ensure previous orientation steps preserved correct sides.)
3. Once both arm ends are partially inserted, lower skirt fully until its mounting holes align with the fender holes.
4. Insert two M3x8 screws (skirt → fender) finger‑tight first.
5. Confirm both arms still seated in pushrod holes (no binding) by gently oscillating each arm.
6. Tighten screws to snug; re‑test articulation by rotating motor slightly by hand—observe synchronized pushrod/arm motion.

## 9. Verification Checklist

| Item | Pass Criteria |
|------|---------------|
| Motor mount fastening | 3 screws snug; mount flush |
| Cam_1 seating | Fully home; no axial wobble |
| Pushrod_b orientation | Upper hole faces RIGHT |
| Pushrod_a orientation | Distal hole points LEFT |
| Cam_2 & cap retention | Cap secure; no axial play; screw snug |
| Pushrod travel | Smooth through full manual rotation; no clamp rub |
| Arm engagement | Both arm ends seated in pushrod holes; move with cams |
| Skirt attachment | 2 screws snug; no gap or rocking |
| Manual actuation | Hand rotation produces expected alternating arm motion |

## 10. Troubleshooting

| Symptom | Likely Cause | Remedy |
|---------|--------------|--------|
| Pushrod binds mid travel | Misaligned clamp or burr in slot | Loosen clamp, deburr slot edges, re-center rods |
| Arm slips out of pushrod | Incomplete insertion / wrong orientation | Re-seat with slight downward pressure; verify hole facing correct side |
| Cam stack axial play | Cap screw loose | Retighten; add low-strength thread locker |
| Uneven arm motion | Reversed pushrods or swapped sides | Disassemble pushrod stage; correct orientations per steps 5.2 & 5.4 |
| Motor stalls under light load | Excess friction / overtight clamp | Loosen clamp screws 1/8 turn; check for rubbing |
| Noise / chatter | Dry plastic cam interface | Very light PTFE dry lube on cam lobes (avoid motor) |

## 11. Staging for Next Step

- Plan wiring path from motor leads toward control board; leave service loop for maintenance.
- Prepare any limit switch or sensor hardware if later automation is intended.

## 12. Next Step

Proceed to Step 8 – Wiring Harness Integration & Electronics Mounting (forthcoming).

---
Navigation: ← Previous [Step 6 – Skirt Assembly & Shoulder Attachment](./step6_skirt_assembly.md) | Next → [Step 8 – Wiring Harness Integration & Electronics Mounting](./step8_wiring_electronics.md) | Index ↔ [Assembly Index](./README.md)

Revision: 0.1 (2025-08-23) Initial draft of fender mechanism & skirt integration.
Revision: 0.2 (2025-08-24) Updated navigation to active Step 8 link.
