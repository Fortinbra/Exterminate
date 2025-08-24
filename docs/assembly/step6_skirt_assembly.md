# Step 6 – Skirt Assembly & Shoulder Attachment

> Objective: Populate the skirt with all hemisphere details (64× `hemi_x65.stl`) using controlled CA glue, then fasten the completed shoulder/arm assembly (from Step 5) onto the skirt with two M3x8 screws.
> Filename Note: The original STL is misnamed `hemi_x65.stl` in the source set (should represent 64 hemispheres). We intentionally retain the on-disk typo here to match the provided files and avoid confusion during printing.
> Prerequisites: Step 5 complete (shoulders with both arms installed & functional). Printed skirt (`skirt.stl`) cleaned of supports; all hemispheres available.

## 1. Parts & Materials

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | skirt.stl | 1 | Base perimeter shell with 64 raised mounting pads |
| Printed | hemi_x65.stl | 64 | Filename retains original typo (prints 1 hemisphere; repeat) |
| Assembly | Shoulder + arms (Step 5) | 1 | To mount on top after hemis installed |
| Hardware | M3x8 screw | 2 | Skirt ↔ shoulder fastening |
| Consumable | CA glue (thin/medium) | As needed | Tiny drops per hemisphere |
| Optional | CA accelerator | Minimal | Use only after placement verified |
| Optional | Painter’s tape / tray | 1 | Stage hemispheres in rows |

Assumption: Skirt includes two internal bosses or through holes aligned to shoulder mounting holes.

## 2. Tools

- Fine tip CA applicator (or micro brush / toothpick)
- #1 or #2 driver (matching M3 screw head)
- Tweezers (hemisphere placement)
- Isopropyl alcohol + lint‑free cloth (surface cleaning)
- Optional: Digital scale (batch verification), clamp pads (if slight warp correction needed)

## 3. Preparation

1. Surface Clean: Wipe skirt exterior with IPA to remove dust/oils; allow to dry fully (improves bond consistency).
2. Hemis Staging: Arrange 64 hemis in a 8×8 or 16×4 grid near workspace; visual count prevents omissions.
3. Test Fit: Place a single hemisphere on several random skirt pads dry—should sit flat without rocking. Lightly deburr any pad high spots or stringing.
4. Orientation: If hemis have a preferred aesthetic (layer seam), face seams consistently (e.g., toward rear) for uniform appearance.
5. Mark Progress: Lightly pencil tick inside skirt after every 8 hemis applied (optional tracking to ensure full count).

## 4. Hemisphere Bonding Process

Recommended adhesive pattern: 1 central micro‑dot OR 3 equally spaced micro‑dots near the pad edge (avoid flooding which can squeeze out and frost surface).

Batch Workflow (repeat until complete):

1. Select 4–8 pads (a manageable batch) and apply CA micro‑dots.
2. Using tweezers, place hemispheres gently—do not slide (sliding can smear glue and tilt). Press straight down for 2–3 seconds.
3. Quick Alignment Check: Ensure each hemisphere is squarely seated (no visible tilt). Adjust within first ~5 seconds if needed.
4. (Optional) Light mist of accelerator held well back (>150 mm) after a full batch is confirmed. Avoid direct puddling.
5. Proceed to next batch; rotate the skirt periodically to keep ergonomic access and reduce accidental contact with uncured glue.

Tips:

- Minimal Glue: Excess increases bloom (white frosting) and cleanup effort.
- Consistent Pressure: Over‑pressing can splay molten edges or create a ring of squeeze‑out.
- Environment: Low humidity reduces blooming; consider gentle airflow (not aimed directly) to speed set without dust.

Completion Check: Count all hemis—should reach 64. Run a fingertip lightly over rows; feel for any loose ones (re‑glue if movement detected).

## 5. Shoulder Assembly Mounting

1. Dry Fit: Place shoulder assembly on top of skirt; confirm mounting holes align with the two internal screw bosses (or holes) in the skirt.
2. Cable Clearance: Ensure any internal wires or harness slack are not pinched between mating surfaces (if wiring already present nearby).
3. Insert First M3x8 Screw: Start by hand 2–3 turns to avoid cross‑threading.
4. Insert Second Screw similarly; only then alternate tightening each until seated—do not overtighten (stop at firm contact + a small snug). Plastic thread caution applies.
5. Rock Test: Gently rock shoulders—no wobble should be present. If wobble exists, verify both screws fully seated and surfaces free of debris.
6. Articulation Re‑Check: Move both arms through range ensuring no hemisphere interferes with lower extremes. If contact occurs, verify overhang lengths or consider subtle chamfer on interfering hemisphere edge (last resort).

## 6. Verification Checklist

| Item | Pass Criteria |
|------|---------------|
| Hemisphere count | 64 installed; no omissions |
| Bond quality | No loose hemis (light finger pressure test) |
| Glue aesthetics | Minimal/no visible squeeze‑out or frosting |
| Row consistency | Even spacing; consistent seam orientation (if applied) |
| Shoulder seating | Flush contact all around; no gaps >0.3 mm |
| Screw torque | Both M3 snug; no wobble; threads intact |
| Arm clearance | Full motion without striking hemis |

## 7. Troubleshooting

| Symptom | Likely Cause | Remedy |
|---------|--------------|--------|
| Hemisphere lifts later | Insufficient glue / oily surface | Clean pad & hemisphere base; reapply tiny CA; hold longer |
| Visible frosting | Excess glue + accelerator humidity | Polish lightly with isopropyl; reduce accelerator usage |
| Tilted hemisphere | Glue set before adjustment | Gently shear off with thin blade; clean pad; re-glue new print |
| Shoulder wobble | Uneven tightening / debris | Loosen both, clean mating face, retighten evenly |
| Screw strips | Over-torque / repeated cycles | Use slightly longer M3 or retrofit heat-set insert |
| Arm collision with hemis | Sphere overhang or hemisphere high | Verify arm build lengths; sand hemisphere crown slightly only if necessary |

## 8. Staging for Next Step

- Document any replacements (re-glued hemis) in build log for future inspection.
- Prepare internal body components / wiring harness channels (next step will begin body integration and routing).

## 9. Next Step

Proceed to Step 7 – Fender Mechanism & Skirt Integration.

---
Navigation: ← Previous [Step 5 – Arm & Shoulder Integration](./step5_shoulders_arms.md) | Next → [Step 7 – Fender Mechanism & Skirt Integration](./step7_fender_mechanism.md) | Index ↔ [Assembly Index](./README.md)

Revision: 0.1 (2025-08-23) Initial draft of skirt assembly & shoulder attachment.
Revision: 0.2 (2025-08-23) Updated next step to Fender Mechanism & Skirt Integration.
Revision: 0.3 (2025-08-24) Corrected hemisphere filename (hemi_x64) and standardized objective wording.
Revision: 0.4 (2025-08-24) Reverted to on-disk typo `hemi_x65.stl` with explanatory note (cannot rename file in set).
