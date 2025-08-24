# Step 4 – Neck Stack & Dome Integration

> Objective: Stack and align the neck ring assembly, route the 4‑wire dome harness cleanly, and finalize the mechanical interface between the dome (Step 3) and the body via neck pin & stalk hardware.
> Prerequisites: Completed Step 2 (Eyestalk) & Step 3 (Dome Subassembly) with labeled harness (ANODE RED L, ANODE RED R, ANODE EYE BLUE, GND BLACK). `stalk_tab.stl` and `dome_nut.stl` not yet installed.

## 1. Parts & Materials

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | neck_a.stl | 1 | Base segment (PLA+/black) |
| Printed | neck_b.stl | 1 | Diffuser ring (transparent PETG) |
| Printed | neck_c.stl | 1 | Spacer (PLA+/black) |
| Printed | neck_d.stl | 1 | Diffuser ring (transparent PETG) |
| Printed | neck_e.stl | 1 | Spacer (PLA+/black) |
| Printed | neck_f.stl | 1 | Top ring (transparent PETG) |
| Printed | neck_pin.stl | 1 | Rotation limit pin |
| Printed | stalk_tab.stl | 1 | Deferred from Step 3 |
| Printed | dome_nut.stl | 1 | Internal stalk retainer |

## 2. Tools

- Needle file / deburring tool
- Flush cutters (trim stray stringing inside channels)
- Fine tweezers / hemostats (wire guiding)
- Multimeter (continuity sanity check)

*No adhesives or lubricants required for this step.*

### Harness Recap

Four conductor bundle from dome:

- Red (Left Dome Anode)
- Red (Right Dome Anode)
- Blue (Eye Anode)
- Black (Common Ground)
Bundle should remain untwisted; provide gentle spiral if rotation is planned (avoid tight twist that adds spring force).

## 3. Neck Stack Planning

Order (bottom → top): neck_a (with center shaft + rear key) → neck_b → neck_c → neck_d → neck_e → neck_f → (dome interface).

Design Considerations:

- Pure friction stack: no glue; components remain serviceable.
- Rear KEY: neck_a's center shaft has a longitudinal key along the REAR (opposite eyestalk). Each subsequent ring has a matching internal notch so orientation is automatic—do not force if misaligned; back off, realign keyway, slide again.
- Maintain a clear central bore; harness must move freely.
- Neck pin provides rotation stop; it must align with the curved slot in the dome stalk.

Optional Dry Run:

1. Slide all rings and pass a scrap wire through to confirm bore continuity.
2. Identify any internal zits / elephant’s foot; lightly scrape or sand.

## 4. Preparation

1. Inspect each ring’s inner channel; remove stringing.
2. Lightly chamfer (optional) the leading inner edge of each bore with a deburring tool to reduce abrasion on wires.
3. (Optional) Test a thin PTFE / nylon washer between neck_f and dome underside if you anticipate frequent rotation.
4. Group the three anode wires together temporarily with a small piece of low‑tack tape 20–30 mm below dome exit; leave ground slightly offset for easy identification.

## 5. Harness Routing Strategy

- Stack the entire neck (A→F) FIRST over the fixed center shaft that is part of neck_a; bore stays empty initially.
- Lower the dome so the dome stalk slides INSIDE neck_a's center shaft and neck_pin enters slot—this captures the stack.
- Form a relaxed service loop just beneath the stalk base (above neck_f) by pulling a little slack back upward; this loop absorbs rotation.
- Keep harness centered; if it drifts to a wall, gently wiggle while pulling—do not twist hard.

## 6. Assembly Steps

1. Place neck_a on a flat, level surface (top facing up).
2. Slide neck_b down until the rear key seats in its notch (orientation enforced). No rocking.
3. Slide neck_c (keyed); correct any tilt immediately.
4. Slide neck_d, then neck_e (keyed automatically, no marking required).
5. Slide neck_f (keyed). Verify vertical alignment: no radial steps; top surface concentric with neck_a. Empty bore smooth end‑to‑end.
6. Insert neck_pin into LEFT side hole of neck_f (front = eyestalk side, rear = keyed side) until fully seated; hand ream only if required.
7. Lower the dome/eyestalk assembly straight down so the neck_pin enters the curved slot on the LEFT of the dome stalk AND the dome stalk slides concentrically inside neck_a's keyed center shaft (rear key still opposite eyestalk).
8. Install `stalk_tab.stl` on the exposed threaded end/post with its downward tab pointing toward the eyestalk (forward reference).
9. Thread on `dome_nut.stl` until just snug (dome still rotates). Do NOT over‑tighten; slight looseness ensures free motion and keeps shaft round.
10. Feed the harness: From dome underside, guide the four wires down through the hollow dome stalk and neck_a shaft. If friction occurs, use tweezers from below to pull each wire sequentially; avoid twisting bundle.
11. Create a small service loop: pull 15–20 mm of harness back upward beneath the stalk base, then relax to form a gentle arc.
12. Rotation Test: Push tab left/right; dome should sweep until slot edges meet neck_pin without the harness tightening. Resize loop if needed.
13. Wire Dressing: Ensure harness exits centered below neck_a; straighten any unintended twist; retag labels if obscured.
14. Electrical Checkpoint: Individually energize each anode (current‑limited) while rotating—verify no flicker or ground interruption.

## 7. Verification Checklist

| Item | Pass Criteria |
|------|---------------|
| Bore clearance | Harness feeds smoothly after full stack; no scraping |
| Rotational stop | neck_pin engages slot smoothly both directions |
| Harness slack | Loop present; no tension at extremes |
| Eyestalk orientation | Forward; stalk tab aligned downward toward eyestalk |
| Ring alignment | Vertical; no radial step between rings |
| Dome retention | Nut snug; dome rotates with gentle tab push |
| Electrical continuity | All 3 anodes + ground intact through movement |

## 8. Troubleshooting

| Symptom | Likely Cause | Action |
|---------|--------------|--------|
| Dome binds / high friction | Over‑tightened dome nut / misaligned rings | Loosen nut slightly; realign stack |
| Limited rotation | Neck_pin not fully seated in slot | Reseat dome ensuring pin in curved slot |
| Early wire tension | Service loop too small or skipped | Recreate loop (Step 9) and retest |
| Flicker on rotation | Harness pinched under a ring | Lift top rings sequentially; free harness |
| Pin won’t seat | Hole undersized | Hand ream lightly; avoid enlarging slot engagement angle |

## 9. Staging for Next Step

- Coil remaining harness length gently inside body cavity zone or secure to temporary anchor to avoid accidental tug.
- Photograph inside bore showing slack loop for future documentation.
- Prepare for Step 5 (Shoulders & Appendages) which will introduce arm/gun assemblies and additional wiring paths.

## 10. Next Step

Proceed to Step 5 – Arm & Shoulder Integration.

---
Navigation: ← Previous [Step 3 – Dome Assembly & LED Distribution Board](./step3_dome.md) | Next → [Step 5 – Arm & Shoulder Integration](./step5_shoulders_arms.md) | Index ↔ [Assembly Index](./README.md)

Revision: 0.1 (2025‑08‑23) Initial draft of neck stacking & integration.
