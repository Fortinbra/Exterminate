# Step 5 – Arm & Shoulder Integration

> Objective: Build both arm subassemblies (plunger & gun), establish precise tube overhangs with spherical joints, then install them into the single shoulder part (`shoulders.stl`) retaining motion with clamps and M3 screws without over‑tightening.
> Prerequisites: Step 4 complete (neck & dome integrated). Printed parts on hand: `shoulders.stl`, 4× `ball_x4.stl`, `plunger.stl`, `gun_tubes.stl`, 2× `ball_clamp.stl`. Tubes pre‑cut to 75 mm (plunger) and 57 mm (gun).

## 1. Parts & Materials

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | shoulders.stl | 1 | Dual‑socket shoulder body (left/right integrated) |
| Printed | ball_x4.stl | 4 | Four identical halves → 2 spheres |
| Printed | ball_clamp.stl | 1 | Single dual-arm retainer (embossed text faces OUT) |
| Printed | plunger.stl | 1 | LEFT arm end effector |
| Printed | gun_tubes.stl | 1 | RIGHT arm end effector |
| Raw | Tube 75 mm | 1 | LEFT arm shaft |
| Raw | Tube 57 mm | 1 | RIGHT arm shaft |
| Consumable | CA glue (thin/medium) | Minimal | Tiny dots only |
| Optional | CA accelerator | As needed | After alignment verified |
| Optional | Fine sandpaper (400+) | Strip | Deburr tube ends |
| Hardware | M3 screw (≈10–12 mm) | 1 | Single central screw for shared clamp |
| Optional | M3 washer | 1 | If seat supports; prevents screw head sink |

Assumption: Tubes already cut square; deburr inner & outer edges lightly.

## 2. Tools

- Flush cutters / deburring tool (print nubs, tube edge cleanup)
- Digital calipers or steel ruler (verify overhangs)
- Fine tip marker (temporary length marks)
- Tweezers / small clamp (hold halves during cure)
- #1/#2 Phillips or hex driver (per M3 screw head)
- Flashlight (inspect clamp seating)

## 3. Design & Alignment Notes

- Single Shoulder Body: `shoulders.stl` provides LEFT (plunger) and RIGHT (gun) sockets; no separate left/right housings.
- Spheres: Each full sphere = two `ball_x4` halves. Interior should be light slip fit. If tight: light scrape; if loose: rely on minimal CA spot tacking.
- Overhang Measurements: 75 mm tube → 19 mm (short) / 46 mm (long). 57 mm tube → 17 mm (short) / 30 mm (long). Measurements are from OUTER sphere surface to tube ends post‑assembly.
- Functional Ends: Long overhang always receives plunger or gun_tubes.
- Adhesive Strategy: 2–3 tiny (≈1 mm) dots near perimeter; avoid flooding to keep potential adjustability until locked.
- Clamp Orientation: Embossed text (e.g., "OUT") must remain readable when looking into the shoulder interior after installation; ensures correct chamfer orientation contacting BOTH sphere bands.
- Retention vs Locking: Single screw should only secure, not compress; both arms must articulate smoothly with moderate hand force. Tightening affects BOTH joints—adjust incrementally.
- Plastic Thread Care: First drive forms threads—advance slowly, back off to clear chips if resistance spikes.

## 4. Preparation

1. Inspect & Deburr: Clean `ball_x4` halves (remove elephant’s foot, stringing) and both tube ends (inner & outer chamfer light).
2. Dry Fit Halves: Ensure each half slides on its tube without force; correct with light scrape if required.
3. Mark Overhangs:
   - 75 mm tube: From one end mark 19 mm; from the opposite end mark 46 mm.
   - 57 mm tube: Mark 17 mm and 30 mm from opposite ends.
4. Shoulder Socket Prep: Inspect both sockets of `shoulders.stl`; remove any blobs that could high‑spot the sphere. Optional: ultra‑thin PTFE trace.
5. Stage Clamp & Screw: Confirm the single M3 screw threads freely through clamp hole (clear print fuzz now, not over the joints).

## 5. LEFT Arm Sphere & Plunger (75 mm Tube)

Target overhangs after sphere placement: 19 mm (short) and 46 mm (long).

1. Select two ball halves for this sphere; identify any visually cleaner seam edges for outward facing sides.
2. Slide first half onto tube roughly to center; do NOT set depth yet.
3. Add tiny CA dots to mating rim of the second half (avoid interior bore); align and press halves together around tube.
4. Immediately slide the forming sphere along tube until overhangs match 19 mm and 46 mm marks (measure from outer sphere surface). Adjust before cure locks.
5. Verify: Total of measured overhangs + sphere outer diameter ≈ 75 mm (sanity check). Overhang tolerance ±0.5 mm acceptable.
6. (Optional) Apply a micro drop of CA at seam points if gaps visible; avoid flooding.
7. After initial set, wick a minimal CA dot at two diametrically opposite points where tube exits sphere if axial slip seems likely (optional).
8. Attach `plunger.stl`:
   - Lightly scuff inside contact surface if glossy.
   - Apply a small ring or 3–4 dots of CA inside plunger socket.
   - Push plunger onto the LONG side (46 mm) tube end until fully seated; a tiny ring of tube should remain visible only if designed so—otherwise flush.
   - Hold until set. Ensure plunger axis aligns with tube (no tilt).

Electrical Checkpoint: (Not applicable – purely mechanical in this step.)

## 6. RIGHT Arm Sphere & Gun (57 mm Tube)

Target overhangs after sphere placement: 17 mm (short) and 30 mm (long).

1. Repeat sphere forming process with remaining two ball halves on 57 mm tube.
2. Position sphere to achieve 17 mm and 30 mm exposed tube lengths; confirm before full cure.
3. Secure seam minimally as in Left Arm (Steps 5.5–5.7 analog).
4. Install `gun_tubes.stl` on the LONG (30 mm) side:
   - Dry fit first; ensure a slight slip fit. Adjust (very light ream) if tight.
   - Apply sparse CA inside the printed part’s bore (avoid ends to prevent squeeze-out).
   - Slide into position leaving a small, even band of tube exposed on both sides of the gun_tubes part (visual symmetry). If design expects flush, adjust accordingly (state assumption here: slight exposure desired per user instruction).
   - Maintain axial alignment until set.

## 7. Shoulder Insertion & Retention (Both Arms – Shared Clamp)

Short tube sides (19 mm plunger / 17 mm gun) go INSIDE `shoulders.stl`; long sides with end effectors remain OUTSIDE.

1. Insert LEFT (plunger) arm short end; seat sphere in left socket—do not release yet.
2. Insert RIGHT (gun) arm short end; seat sphere in right socket.
3. Center Both: Adjust each sphere so overhangs remain correct and end effectors aim neutrally forward. Support assemblies so they don't drop.
4. Place the single `ball_clamp.stl` over BOTH spheres simultaneously (embossed text readable). Ensure clamp lands flat on both socket rims.
5. Start M3 screw by hand 2–3 turns (avoid cross‑thread). Verify clamp not lifting one side.
6. Incremental Tightening: Turn screw until initial contact with both spheres (feel resistance). Test articulation of BOTH arms.
7. Micro‑Adjust: Tighten in 1/16–1/8 turn increments, alternating articulation tests. Stop once both arms have smooth, gently resistive motion—no binding.
8. Final Check: Ensure neither arm is appreciably tighter; if one is stiff and the other loose, back off slightly, re-center clamp, retighten.

## 8. Verification Checklist

| Item | Pass Criteria |
|------|---------------|
| Left sphere overhangs | 19 mm ±0.5 / 46 mm ±0.5 |
| Right sphere overhangs | 17 mm ±0.5 / 30 mm ±0.5 |
| Plunger alignment | Axis colinear; no wobble |
| Gun_tubes placement | Even narrow tube band both ends (or specified flush) |
| Clamp orientation | Text readable; seated flush across both sockets |
| Screw torque | Both arms pivot smoothly; neither overly tight nor loose |
| Seams | No gaps >0.3 mm; minimal CA residue |

## 9. Troubleshooting

| Symptom | Likely Cause | Remedy |
|---------|--------------|--------|
| Sphere drifts before cure | Too little initial friction / excess glue | Hold gently until tack; add micro dot at tube exit after positioning |
| Overhang short/long | Mis-measured from wrong reference (center vs surface) | Reheat gently with hot air (low) if just cured and reposition; otherwise reprint sphere halves |
| Visible seam gap | Print warp or debris | Open gently before full cure; scrape interference; re-glue |
| Plunger/gun_tubes skew | Pressed before square seating | While CA still green (<30s), realign; otherwise debond (acetone / debonder if material supports) and retry |
| Glue bloom (white frosting) | Excess accelerator or humid environment | Reduce accelerator; gently polish with isopropyl + microfiber |
| One arm tight, other loose | Clamp seated unevenly or sphere not fully seated | Loosen slightly, reseat both spheres, retighten gradually |

## 10. Staging for Next Step

- Label left vs right assemblies subtlely on hidden tube portions if future orientation matters.
- Record actual overhang measurements for reference (tolerances OK) in build log.
- Prepare any shoulder mounting brackets or pivot hardware (introduced next step) and confirm clearances with spheres dry-fitted.

## 11. Next Step

Proceed to Step 6 – Skirt Assembly & Shoulder Attachment.

---
Navigation: ← Previous [Step 4 – Neck Stack & Dome Integration](./step4_neck.md) | Next → [Step 6 – Skirt Assembly & Shoulder Attachment](./step6_skirt_assembly.md) | Index ↔ [Assembly Index](./README.md)

Revision: 0.3 (2025-08-23) Updated for single shared clamp & screw retaining both arms simultaneously.
Revision: 0.4 (2025-08-23) Next step renamed to Skirt Assembly & Shoulder Attachment.
