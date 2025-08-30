# Step 8 – Wiring Harness Integration & Electronics Mounting

> Objective (Part 1): Secure the audio amplifier (MAX98357A breakout), MOSFET driver board, speaker, and both drive motors (with small drive gears) onto the `base_cover.stl`, establishing the physical foundation for later wiring, power distribution, and control electronics.
> Scope: ONLY the operations below. Later subsections (Part 2+) will handle main MCU, power, harness routing, strain relief, connectors, and cable dressing.
-> Prerequisites: Printed `base_cover.stl`, `drive_gear.stl` (2×), cleaned motor standoffs, verified hole sizes. Components on hand: MAX98357A (or compatible MAX audio board) with screw terminal block, MOSFET driver breakout (STEMMA JST PH style — e.g. Adafruit MOSFET Driver [Product 5648](https://www.adafruit.com/product/5648)), two N20 gear motors (verified ratio match), 28 mm speaker, required fasteners.

---
## 1. Parts & Hardware (This Sub‑Step)

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Printed | base_cover.stl | 1 | Mounting platform (remix) |
| Printed | drive_gear.stl | 2 | Small motor pinion gears (press / slip fit) |
| Audio | MAX98357A I2S 3 W Class D Amp | 1 | With 3‑pin or 5‑pin screw terminal (L+, L− / Vin / GND / SD) |
| Driver | MOSFET Driver Board (STEMMA JST PH, e.g. Adafruit product 5648) | 1 | For higher current loads (lighting / actuators) |
| Audio | 28 mm 8 Ω 2 W speaker | 1 | Fits circular recess center of base_cover |
| Drivetrain | N20 gear motor (3 V, 1000 RPM) | 2 | Matched pair (identical ratio) |
| Fastener | M2.5 × 8 mm screw | 4 | Two per breakout (inserted from underside) |
| Fastener | M2.5 hex nut | 4 | Secures audio & MOSFET boards |
| Fastener | M1.7 × 4 mm screw | 4 | Two per motor (standoff mounting) |
| Wire | Speaker lead pair | 1 set | Sufficient length to reach amp terminals without tension |

Assumptions:

- Mount hole spacing on base_cover matches the footprints of the selected MAX board & MOSFET driver (common Pimoroni / Adafruit pattern). Adjust washers/spacers if board backs contact surface features.
- Drive gears are the smaller printed gears intended for direct motor shaft interface (not the larger wheel gears).

## 2. Tools

- #0 / #1 Phillips or appropriate driver for M1.7 and M2.5 heads (confirm head style)
- Needle nose pliers or small nut driver (for M2.5 hex nuts)
- Tweezers (nut positioning assistance)
- Flush cutters (trim any print stringing around holes)
- Small flat file / deburring tool (clear standoff tops if needed)

## 3. Preparation

1. Inspect `base_cover.stl` for print artifacts around board mounting holes and motor standoffs; remove stringing or blobs.
2. Test fit an M2.5 screw through each board mounting hole from BELOW the base cover to ensure clearance (clear with a hand twist drill if undersized—do not melt).
3. Test fit the M1.7 screws into the motor standoff pilot holes; they should bite with gentle torque (no pre‑tapping required). If overly tight, lightly ream; if loose, plan to add a micro drop of medium CA after final positioning (do not apply yet).
4. Verify both N20 motors spin freely (brief low‑voltage test) and match direction/ratio. Mark a small dot on one shaft flat for orientation reference.
5. Lightly chamfer the bore of each `drive_gear.stl` if a brim/elephant’s foot is present.

## 4. Audio Amplifier Board Mounting (Left Side)

Orientation Goal: Screw terminals face INWARD toward the center of the base so speaker wires route directly without crossing future harness paths.

1. Invert the base cover (underside up) on a soft surface to access mounting holes safely.
2. Insert two M2.5 × 8 mm screws UP through the left-side board mounting hole pair.
3. While holding screws in place with a fingertip, flip the base upright carefully without pushing screws back out.
4. Place the MAX98357A board over the protruding screw shanks in the planned orientation (terminals inward). Ensure no bottom-side components foul the surface; add a thin spacer if necessary (not usually required).
5. Thread on the two M2.5 hex nuts using tweezers to position, then tighten evenly. Stop at snug—do not bow the PCB.
6. Verify terminal block faces the speaker recess; clearance should allow a gentle wire loop from the speaker rim.

Checkpoint A (Amplifier): Board sits flat, terminals inward, nuts secure, no PCB flex.

## 5. MOSFET Driver Board Mounting (Right Side)

1. Repeat the underside insertion for two M2.5 × 8 mm screws on the right-side mounting hole pattern.
2. Flip upright and place the MOSFET driver breakout so its primary terminal / JST / header faces inward (toward central wiring zone) for short lead runs.
3. Secure with two M2.5 hex nuts; tighten alternating until just firm.
4. Confirm vertical alignment roughly matches the amplifier board (aesthetic & harness routing symmetry).

Checkpoint B (Driver): Board secure, inward-facing connectors unobstructed.

## 6. Speaker Placement & Connection

1. Dry place the 28 mm speaker into the circular recess—ensure it seats flush without rocking. If slightly proud, check for print burrs and remove carefully.
2. Route the pre‑tinned or stripped speaker lead pair to the MAX board’s speaker output terminals (often labeled L+ / L− or + / −). Maintain slack loop avoiding sharp bends over the speaker frame.
3. Loosen the two terminal screws, insert wire ends fully (no stray strands), and tighten firmly while supporting the board (avoid twisting the PCB).
4. Lightly tug each wire to confirm secure clamp.
5. Dress excess wire into a gentle S‑bend near the speaker edge—keep center area clear for future battery or harness components.

Checkpoint C (Speaker): Speaker sits flush, wires secure in terminals, no tension when gently lifting wires.

## 7. Motor Mounting & Drive Gear Installation

Orientation Change: For this build the motor shafts face OUTWARD toward the future wheel assemblies (wheel gears + axle not installed yet). The small `drive_gear.stl` parts (pinions) are installed now; the larger `wheel_gear.stl` and axle insertion will occur in a later sub‑step.

1. Identify LEFT and RIGHT motor standoff sets on the base cover (mirrored locations). Ensure mounting faces are clean and flat.
2. Orient the first N20 motor so its shaft points outward (away from the center cavity) toward the perimeter where the wheel gear will later reside.
3. Align motor mounting holes with the two standoff pilot holes.
4. Insert first M1.7 × 4 mm screw and turn 1–2 threads—do not fully tighten.
5. Insert second M1.7 screw; once both are started, alternate tightening until the motor is held snug (avoid over‑torque stripping the plastic thread). Motor should not rotate by hand once secured.
6. Repeat for the second motor on the opposite side with identical outward shaft orientation (shafts parallel and both pointing outward).
7. Press a `drive_gear.stl` (pinion) onto each outward-facing motor shaft: Align shaft flat with the gear’s internal flat (if modeled). Support motor body while pressing to avoid bending the shaft.
8. Seat each pinion fully but avoid excessive force—aim for minimal axial play while maintaining free rotation.
9. Spin each pinion gently; observe concentric rotation (no wobble). If resistance is high, remove, lightly relieve bore, and re‑install.

Deferred: Installation of `wheel_gear.stl` components and axle/spacer will follow in a later Part 2/3 sequence once additional chassis elements are in place.

Checkpoint D (Motors & Pinions): Both motors rigid, outward pinions fully seated, rotation smooth without binding.

## 8. Wheel Gear & Axle Installation

Goal: Install bearings into each `wheel_gear.stl`, insert the axle through the chassis supports, and mesh the large wheel gears with the previously installed motor pinions.

### 8.1 Parts (This Sub‑Step)

| Item | Qty | Notes |
|------|-----|-------|
| wheel_gear.stl | 2 | Large driven gears (wheels) |
| 683ZZ bearing (3×7×3) | 4 | 2 per wheel (inner + outer) |
| 3 mm tube (axle / spacer) | 1 (≈60 mm) | As listed in BOM (ensure burr‑free ends) |

### 8.2 Preparation

1. Deburr axle ends lightly (fine file / Scotch‑Brite) so they chamfer and won’t shave bearing shields.
2. Test a single 683ZZ bearing in a wheel gear bore—target a light press fit. If excessively tight, warm gear slightly (hand‑warm only) or VERY lightly scrape bore. If loose, plan for a micro drop of removable retaining compound later (defer for now).
3. Confirm both wheel gear teeth are clean (remove stringing between teeth with a plastic dental pick—tooth profile must remain crisp).

### 8.3 Bearing Insertion

1. Press the FIRST bearing into one side of the wheel gear until fully seated (face flush with gear hub edge). Support the opposite side evenly; press straight (avoid tilt that can gouge bore).
2. Flip the gear and press the SECOND bearing into the opposite side. Bearings should meet or leave a minimal internal gap (<0.1 mm). No rattle when gently shaken.
3. Repeat for the second wheel gear.

Checkpoint E (Bearings): All four bearings seated flush, no rocking, smooth fingertip spin of inner race.

### 8.4 Axle & First Wheel Installation

1. Identify the left chassis support (choose a consistent reference; front of model aligns with eyestalk direction). You can start either side; stay consistent for documentation.
2. Begin inserting the 3 mm axle from OUTSIDE toward the center through the first chassis support hole—advance only enough so ~10–12 mm protrudes inside the cavity.
3. Slide the FIRST wheel gear (bearing pair installed) onto the protruding axle section with GEAR TEETH FACING INWARD (toward the centerline / motor pinions). Ensure full engagement of axle through both bearings.
4. Continue pushing the axle inward, guiding its leading end into the central opposite support bore. Do not yet push it fully through the far support.

### 8.5 Second Wheel Installation

1. As the axle emerges from the far side of the central support, stop with ~10–12 mm still outside.
2. Place the SECOND wheel gear onto the emerging axle end, again TEETH FACING INWARD toward the center.
3. Press the wheel assembly inward while simultaneously pushing the axle the remaining distance through the far (outer) support until both wheel gears sit between their respective outer and inner supports.
4. Center the axle so roughly equal protrusion (if any) exists on both outer faces. If design expects flush ends, adjust accordingly.

### 8.6 Mesh & Alignment Check

1. Verify each motor pinion meshes with its wheel gear: there should be slight backlash (a barely perceptible tick when holding gear & rocking pinion) but no visible gap causing partial tooth engagement.
2. Rotate each wheel gear by hand: Both should turn smoothly with even resistance; observe motor pinion rotation (motors will act as weak generators causing slight drag—normal).
3. Check axial play: Pull/push each wheel gear along axle. Total side‑to‑side movement should be minimal (<0.5 mm). If excessive, plan to add thin printed shims in a later refinement step (defer for now).
4. Inspect for rubbing: No contact between gear face and chassis support walls. If rubbing: verify bearings fully seated; lightly relieve interfering surface.

Checkpoint F (Wheel Assembly): Bearings flush, gears oriented teeth inward, smooth rotation, proper mesh backlash, minimal axial play.

NOTE: Do NOT apply lubrication yet. Lubricant (if desired) will be introduced after full drivetrain validation to avoid attracting debris during remaining assembly.

## 9. Pause Point / Deferred Work (End of Part 1)

If you intend to stop here, leave boards covered to prevent debris ingress. Otherwise continue directly into Part 2 (MCU header & Motor SHIM integration).

---

## Part 2 – Pico LiPo 2XL W Header & Motor SHIM Integration

Objective (Part 2 Segment A): Prepare the Pico LiPo 2XL W by soldering dual 10‑pin headers (one each side near the USB end) and mount/solder the Motor SHIM so it resides on the backside of the Pico with the QW/ST port aligned to the Pico’s USB connector edge. Final header pins will be clipped flush for clearance.

### 10. Parts (This Segment)

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Control | Pico LiPo 2XL W | 1 | USB end reference faces forward/front of chassis |
| Control | 10‑pin male header strip | 2 | Standard 2.54 mm; will be trimmed flush post solder |
| Control | Motor SHIM for Pico | 1 | Sits on backside; QW/ST port alignment required |
| Spacer | Plastic separator (included with header) | 2 | Acts as solder height gauge (on backside) |
| Consumable | Flux pen | As needed | Improves wetting |
| Tool | Flush cutters | 1 | For pin trimming after soldering |

### 11. Tools (Additional)

- Fine tip soldering iron (temperature controlled)
- Leaded or unleaded solder (0.5–0.7 mm)
- PCB vise / helping hands (stability)
- ESD mat / wrist strap (recommended)
- Isopropyl alcohol + lint‑free swabs (flux cleanup)

### 12. Preparation

1. Inspect Pico pads (through‑hole) – ensure no oxidation; lightly apply flux.
2. Break or cut two 10‑pin header segments; verify pin count matches required pad span (USB end group on each side).
3. Decide which side is considered the “backside” (headers’ black plastic separators must end up on the backside so they can sit between Pico and Motor SHIM for consistent standoff). Orientation of the gold pin bevels is non‑critical because pins will be clipped.

### 13. Soldering Pico Headers

1. Place first 10‑pin header through the Pico from TOP (component side) so the black plastic separator is on the BACKSIDE (underside) of the board.
2. Tack one end pin: Heat pad + pin simultaneously; apply minimal solder forming a shiny fillet. Confirm header sits perpendicular. Reheat and adjust if tilted.
3. Tack the opposite end pin; verify alignment again.
4. Flow the remaining 8 joints: Keep tip clean; dwell just long enough for full wetting. Avoid excessive solder (bulbous joints may complicate later flush clipping).
5. Repeat for second header on the other side (same orientation).
6. Visual Inspection: All joints concave, shiny, no bridges between adjacent pads.
7. (Optional) Continuity sanity: Use multimeter in continuity mode to confirm each pin has continuity from top pad to protruding pin end and no shorts to neighbors.

Checkpoint G (Headers): Both 10‑pin headers soldered straight; plastic separators flush & even on backside.

### 14. Dry Fit Motor SHIM

1. Place the Motor SHIM onto the protruding header pins (backside of Pico). Ensure the QW/ST (STEMMA QT / Qwiic) port on the SHIM lines up horizontally with the USB connector edge – this ensures correct orientation.
2. Confirm the included plastic spacer (if separate) sits between the two boards uniformly; no bowing.
3. Check that no SHIM underside component fouls against any Pico component.

### 15. Solder Motor SHIM

1. With the assembly held in a vise (Pico on top, SHIM underside accessible), tack two diagonal pins (one corner of each header) to lock alignment.
2. Inspect alignment; adjust if needed by reheating a single tack joint.
3. Solder the remaining pins systematically, alternating sides to distribute heat.
4. Allow brief cooling intervals if board becomes noticeably warm.
5. Inspect all joints (good fillets, no icicles). Reflow any dull / grainy (“cold”) joints.

Checkpoint H (SHIM Mounting): SHIM fully seated, all header pins soldered, QW/ST port aligned with USB edge.

### 16. Pin Trimming & Cleanup

1. Using flush cutters, clip protruding pin ends on the SHIM side as close as practical to solder fillets without nicking joints.
2. Angle board slightly so cut fragments eject away from sensitive areas (wear eye protection).
3. Inspect for sharp edges or partial cuts; re‑clip if necessary.
4. Clean flux residues with IPA and swab; dry thoroughly.
5. Final continuity pass (spot check): Adjacent pins should not beep; random sample 3–4 pins for proper connection.

### 17. Interim Mounting Note

At this point the Pico + Motor SHIM assembly can be set aside ESD‑safe until chassis mounting holes & standoffs (if any) are detailed in the next segment. Do not yet connect power or peripherals.

### 18. Expansion Header Set (Battery Connector End)

Goal: Install the SECOND pair of 10‑pin headers at the battery connector end (opposite USB) to provide connection points for MOSFET driver outputs, LED anode lines, audio interface / enable, and future harness signals. These pins REMAIN UNCUT to function as a vertical interconnect field.

### 19. Additional Parts

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Control | 10‑pin male header strip | 2 | Standard 2.54 mm – full length retained |

### 20. Orientation & Preparation

1. Identify the battery connector end of the Pico (opposite USB).
2. Ensure corresponding through‑holes are clear (wick old solder if needed).
3. Dry place a header: BLACK PLASTIC SPACER + LONG PINS must exit the BACKSIDE (Motor SHIM side). The short pin stubs will be soldered from the component/front side.
4. (Optional) Use a breadboard / printed jig to keep both headers orthogonal during soldering.

### 21. Soldering Expansion Headers

1. Support the assembly so the front/component side faces upward and the Motor SHIM is not bearing weight.
2. Insert first 10‑pin header from the front; confirm long pins protrude fully and spacer sits flush on backside.
3. Tack one corner pad; check 90° alignment. Reflow and adjust if tilted.
4. Tack opposite corner; re‑verify alignment.
5. Solder remaining eight pins with consistent dwell to avoid overheating earlier joints.
6. Repeat for second header on the opposite side.
7. Inspect: headers parallel to each other and to the earlier (USB‑end) clipped headers.
8. DO NOT CLIP these pins—full length required for harness connections & debugging clips.
9. Clean excess flux (avoid flexing long pins).

Checkpoint I (Expansion Headers): Two rear 10‑pin headers perpendicular, parallel, long pins unobstructed, no bridges.

### 22. Rationale & Future Use

- Long pins accept: MOSFET driver outputs, LED control lines (eye + dome), audio enable/data, additional sensors.
- Full length simplifies attaching logic analyzer or temporary jumper leads during bring‑up.
- Pin mapping / labeling will be provided in Part 3 once signal assignment is finalized.

### 23. ESD & Handling Note

Store assembly on anti‑static surface; added long pins increase leverage—avoid sideways knocks that could stress solder fillets.

---

## Part 3 – Initial Interconnect Wiring (Audio & Power Jumpers)

Objective (Part 3 Segment B): Fabricate and install the minimal point‑to‑point wiring needed to power the MAX98357A audio amplifier and bring it out of shutdown by tying SD high, while preparing clean, short Dupont jumper runs that avoid the cam / pushrod motion envelope.

Scope: ONLY the Pico ↔ MAX board short jumpers and creation of a 3‑way (tri‑fan) 3V3 distribution lead (Pico 3V3 → MAX VIN + MAX SD). Full harness (LED anodes, MOSFET driver signal, ground bus consolidation, battery / charge path, and labeling) deferred to Part 3 Segment C.

### 24. Important Power Note (Read First)

The Pico LiPo 2XL W 3V3 rail is regulator‑limited. The MAX98357A can accept 2.5–5.5 V and delivers far less output power at 3.3 V (acceptable for this build). DO NOT tie raw LiPo (VBAT ~3.7–4.2 V) directly to the 3V3 rail. If your “red JST wire” is raw battery, keep it isolated. Only proceed with the tri‑fan jumper if the third branch is intended for another 3V3 consumer (NOT raw VBAT). If unsure, omit the JST branch now—add later once confirmed with a multimeter (expect 3.3 V, not >3.5 V).

Assumption: In this step we connect ONLY Pico 3V3 → MAX VIN and Pico 3V3 → MAX SD (shutdown pin). The optional third branch to another 3V3 device is postponed unless voltage verified.

### 25. Parts (This Segment)

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Wire | 26–30 AWG silicone hookup (red) | ~60–80 mm total | To fabricate tri‑fan 3V3 lead |
| Wire | 26–30 AWG silicone (black) | 2× short (15–25 mm) | Pico GND ↔ MAX GND (choose nearest pads) |
| Connector | Female Dupont single housings | 3–4 | For individual header pin terminations |
| Tubing | 2 mm heat‑shrink | 2–3 pieces | Insulate splices / bundle |
| Consumable | Flux, solder | As needed | Clean joints |
| Tool | Flush cutters, wire strippers | 1 each | Precision prep |

### 26. Wire Length Planning

1. Temporarily place the Pico+SHIM assembly where it will roughly mount relative to the base cover (estimate distance to MAX board). Keep wires inside an imaginary safe corridor CLEAR of pushrods/cams (visualize upward travel path).
2. Measure straight‑line distance Pico 3V3 header pin → MAX VIN pad; add 5–8 mm service loop. Typical length ≈ 25–35 mm.
3. Use same base length for branch to MAX SD (they are adjacent on MAX board). The third branch (if used later) should extend only enough to reach its destination without slack droop into moving mechanisms.

### 27. Tri‑Fan 3V3 Lead Fabrication

1. Cut one “trunk” wire equal to longest required branch + 8–10 mm (for splice).
2. Cut two branch wires (VIN, SD) to final length (allow 2–3 mm stripping).
3. Strip 2–3 mm insulation from one end of each branch and one end of the trunk. Pre‑tin lightly.
4. Form a small parallel bundle: place the two branch wire tinned ends side‑by‑side against the trunk’s tinned end (creating a 3‑way junction). Heat and flow just enough solder to merge—avoid a bulky blob.
5. Slide pre‑positioned heat‑shrink over the joint and shrink (do not trap stiff solder spikes—reflow if necessary for a smooth profile).
6. Strip & tin the free ends of all three wires.
7. (Optional) Add a 2–3 mm piece of heat‑shrink half on trunk/half on branch junction for strain relief.

Checkpoint J (Tri‑Fan): 3‑way splice compact, smooth, insulated; three free tinned ends sized for header insertion.

### 28. Installing 3V3 Lead

1. Push each tinned end into a female Dupont crimp contact (or pre‑crimped lead) and crimp/solder per quality preference; insert into single housings (individual, not 3‑in‑1) for flexible routing.
2. Connect trunk output to Pico 3V3 header pin (choose a clearly labeled 3V3 pin on the expansion header if available).
3. Connect branch leads to MAX VIN and MAX SD pins/headers (SD pulled high keeps amp enabled). If MAX board lacks pin headers yet, optionally solder directly—otherwise use short male header stubs.
4. Gently dress wires so they hug the board edges; no loop should descend below the motor pinion plane.

### 29. Ground Leads

1. Identify the closest Pico GND pin to the MAX board (minimize length).
2. Fabricate two short black GND jumpers (if separate returns desired) OR a single GND if board layout convenient.
3. Connect Pico GND → MAX GND (and a second GND for future shared return if reserved). Keep slack minimal.
4. Tug test each connector; re‑crimp if movement detected.

Checkpoint K (Power Wiring): 3V3 VIN & SD present, GND linked, no wire sag into drivetrain zone.

### 30. Clearance & Motion Check

1. Rotate cam mechanism manually (if fender assembly accessible) while observing wires; verify no incursion into moving sweep volume.
2. Invert assembly gently (simulate handling) to ensure wires do not fall into gear mesh.
3. If any loop droops, shorten or add a micro cable tie / miniature adhesive pad anchor (anchor details later Part 3C).

### 31. Preliminary Power Test (Optional Now / Mandatory Later)

1. WITHOUT speaker connected, apply USB power to Pico; verify no unexpected heat on MAX board (touch test carefully).
2. Measure voltage at MAX VIN vs GND (~3.3 V).
3. If stable, reconnect speaker and briefly play a low‑volume test tone (later firmware) or skip until firmware loaded.
4. If voltage deviates (>3.4 V or <3.2 V significantly under no load), investigate regulator output before proceeding.

### 32. Deferred Wiring (Do NOT Perform Yet)

- LED anode lines (Eye + Dome x2)
- Common LED ground return bundling / strain relief
- MOSFET driver signal (white) line assignment
- LiPo / charge path harness & power switch integration
- Audio data lines (BCLK, LRCLK, DIN) from Pico to MAX board
- Labeling & heat‑shrink color codes

### 33. Part 3 Segment C – MAX Audio Signal Wiring & LED Harness Integration

Objective: Complete logical connections between Pico and MAX board (I2S digital audio), prepare/terminate all four LED leads with Dupont females, dry‑fit the head (dome + neck + eyestalk) onto shoulders ensuring keyed rear alignment, route LED wires to avoid cam/pushrod interference, then lower full upper assembly onto the base and secure with four M3 screws.

Scope: Signal (not power) wiring for audio plus LED lead termination & connection. Final MOSFET driver input assignment still deferred.

#### 33.1 Additional Parts / Materials

| Category | Item | Qty | Notes |
|----------|------|-----|-------|
| Wire | 28–30 AWG (assorted colors) | 3 short | I2S lines BCLK, LRCLK, DIN |
| Connector | Female Dupont housings | 7–8 | 3× audio, 4× LEDs, spare |
| Wire | LED harness (existing) | 4 leads | Blue (Eye Anode), Red L, Red R, Black GND |
| Tool | Flush cutters / small snips | 1 | Trim lengths |
| Tool | Heat‑shrink (1.5–2 mm) | 4 pcs | LED lead strain relief (optional) |

Assumptions: MAX board pin labels (typical) – VIN, GND, DIN, BCLK, LRC (LRCLK / WSEL), SD (already tied high). Confirm silkscreen before soldering.

#### 33.2 Pico Pin Selection (Provisional)

Proposed mapping (adjust later in firmware if conflicts arise):

- BCLK  → Pico GP10
- LRCLK → Pico GP11
- DIN   → Pico GP12
- Eye LED anode (Blue)  → Pico GP13
- Dome Red Left anode   → Pico GP14
- Dome Red Right anode  → Pico GP15
- LED Common GND        → Any GND pin (use same rail already established)

Rationale: Consecutive GPIO block simplifies firmware pin muxing; leaves lower pins for motors / future sensors. Update table later if reserved pins collide with SHIM default mapping.

#### 33.3 Cut & Prepare Audio Jumpers

1. Dry route three short jumpers from chosen Pico pins (GP10–GP12) to MAX board pads (BCLK, LRCLK, DIN). Keep each wire just long enough with a tiny service loop (≈ 3–5 mm slack).
2. Strip 2–3 mm insulation, tin ends.
3. If MAX board already has header pins, terminate Pico ends with female Dupont; otherwise solder directly to MAX pads (preferred for lowest profile). If direct solder: feed wire from pad edge, wet pad then withdraw iron quickly.
4. Dress wires flat against board surfaces; avoid crossing over the speaker cavity.

Checkpoint L (Audio Signals): All three I2S lines connected, no solder bridges, wires clear of moving pathways.

#### 33.4 LED Lead Dupont Termination

1. Trim each of the four LED harness leads (Blue, Red L, Red R, Black) to the minimal length reaching selected Pico header pins with a 5–8 mm gentle loop.
2. Strip ~2 mm insulation; crimp female Dupont contacts (light solder wick assist if crimper quality marginal).
3. (Optional) Slide heat‑shrink over each lead base for strain relief; shrink after crimp solder touch‑up.
4. Label (tape flag or micro heat‑shrink color code) if color ambiguity exists.

#### 33.5 Head Assembly Dry Fit & Wire Routing

1. Place shoulder + arm + skirt + fender stack upright.
2. Align neck stack rear key with shoulder rear keyway (previous orientation conventions). Lower dome/neck assembly carefully—DO NOT yet glue.
3. Feed LED leads downward through the central cavity toward Pico expansion headers, keeping them centered—avoid hugging pushrod channels.
4. Gently articulate arms / rotate dome through allowable range ensuring no lead snags or wraps around the cam region.
5. Adjust loop sizes until wires remain tension‑free at extremes.

Checkpoint M (Head Routing): All four LED leads reach Pico area, no interference during motion tests.

#### 33.6 LED Connections to Pico

1. Connect Blue (Eye) → GP13 (as mapped) via expansion header.
2. Connect Red Left → GP14; Red Right → GP15.
3. Connect Black (LED common ground) to nearby GND pin (same ground bus as MAX board).
4. Quick continuity check: Verify no short between anode pins and ground (multimeter resistance mode; should show open / high until LEDs forward biased through resistors inside dome board).

Checkpoint N (LED Termination): All four leads seated, correct pins, ground secure, no shorts.

#### 33.7 Stack Onto Base Cover

1. Lift full upper assembly (fender + skirt + shoulders + neck + dome) above base_cover.
2. Ensure all intermediate wires (audio jumpers, tri‑fan 3V3, LED leads) are dressed inward and below future pinch lines.
3. Lower assembly straight down, visually tracking clearance at pushrods, gears, and speaker rim.
4. Align mounting holes; insert four M3 screws (from above) into base_cover mounting points—start each 2–3 turns.
5. Tighten incrementally in a cross pattern to snug (avoid warping base_cover). Do not overtighten into printed plastic.
6. Final motion test: Rotate dome, move arms, manually turn cam—confirm zero wire entanglement.

Checkpoint O (Chassis Integration): Assembly seated, 4× M3 engaged snug, all motions free, wiring clear.

#### 33.8 (Optional) Temporary Power-On Test

1. Apply USB power; verify Pico boots, MAX board not heating, no LED flicker on idle (unless firmware drives them).
2. If firmware available, briefly toggle each LED output (test script) verifying correct color/position.
3. Power down; disconnect until final harness strain relief added (later segment).

#### 33.9 Deferred / Next

- MOSFET driver signal pin assignment & wiring
- Full harness labeling & bundling strategy
- Battery integration (LiPo path, switch usage already on-board)
- Audio test tone playback validation
- Strain relief anchors & adhesive tie‑downs

---

---

## 10. Verification Checklist (Part 1 + Segment A)

| Item | Pass Criteria |
|------|---------------|
| Amplifier board | Mounted left, terminals inward, no flex |
| MOSFET driver | Mounted right, connectors inward |
| Speaker seating | Flush in recess, wires secure, slack present |
| Motor orientation | Shafts outward & parallel (wheel side) |
| Pinion seating | Both pinions fully pressed, no wobble |
| Wheel gear bearings | 2 per wheel fully seated, no rattle |
| Wheel gear orientation | Teeth inward toward pinions |
| Axle centering | Axle centered; minimal protrusion imbalance |
| Gear mesh | Even mesh, slight backlash, no binding |
| Motor fastening | 2 screws each, no spin or wobble |
| Drive gears | Fully seated, smooth manual rotation |
| Fastener integrity | No stripped plastic, nuts snug |
| Pico headers | Two straight 10‑pin headers installed |
| Header joints | Smooth, shiny, no bridges |
| Motor SHIM alignment | QW/ST port aligned to Pico USB edge |
| SHIM seating | Spacer flush; no board bow |
| Pins trimmed | All header pins clipped flush (no protruding spikes) |
| Expansion headers (rear) | Two 10‑pin sets installed at battery end |
| Expansion header orientation | Long pins & spacers on backside (Motor SHIM side) |
| Expansion pins condition | Not clipped; straight & parallel |
| Tri‑fan 3V3 splice | Compact, insulated, no exposed conductor |
| 3V3 distribution | Pico 3V3 → MAX VIN + SD (verified) |
| GND jumper | Secure, minimal slack, proper return |
| Wire clearance | No wires in cam/pushrod/gear envelope |
| Audio signals (Checkpoint L) | BCLK, LRCLK, DIN connected cleanly (no bridges) |
| LED routing (Checkpoint M) | Four LED leads reach Pico without interference |
| LED termination (Checkpoint N) | Anodes & ground on mapped pins, no shorts |
| Chassis integration (Checkpoint O) | Upper stack seated; 4× M3 secure; motion unobstructed |

## 11. Troubleshooting (Part 1 + Segment A)

| Symptom | Likely Cause | Remedy |
|---------|--------------|-------|
| Board tilts / rocks | Debris under PCB / uneven print | Remove, scrape high spot, remount |
| Speaker rattles | Not fully seated / warped recess | Deburr recess edge; add thin foam ring |
| Motor shifts under torque | Screws under‑tightened | Incrementally tighten both screws |
| Gear wobble | Ovalized bore / misaligned press | Reprint gear; press supporting shaft properly |
| Tight / no backlash | Axle not fully seated or gears forced inward | Re-seat wheels; ensure bearings fully home; verify pinion depth |
| Excessive backlash | Pinion not fully seated / axle off-center | Press pinion further; recenter axle; add shim later if needed |
| Axial wheel drift | Lack of lateral constraint | Plan thin shim (future step) or slight spacer print |
| Screw spins (no bite) | Over‑stripped plastic | Slightly larger screw, CA + re‑tap after cure |
| Expansion header lean | Uneven tacking / mechanical bump | Reheat one end, straighten, retack |
| Expansion header bridge | Excess solder / flux pooling | Flux + wick then reflow |
| Bent long pins | Handling stress | Realign gently with smooth pliers |
| Tri‑fan splice bulky | Excess solder / cold joint | Reflow with flux; wick excess; re‑shrink tubing |
| MAX not powering | Incorrect pin (not 3V3) / open splice | Verify pin mapping; continuity test splice; re‑solder |
| SD floating (amp off) | SD branch not connected or poor contact | Reseat Dupont; test continuity to splice |
| Wire fouls mechanism | Excess length / poor dressing | Shorten wire; add anchor; reroute tight to board edge |
| Header won’t sit flat | Solder blob in through‑hole / bent pin | Wick excess with braid; straighten pin; reseat |
| Bridged header pins | Excess solder / flux pooling | Use solder wick + flux; reflow clean joint |
| Dull / grainy joint | Insufficient heat or dirty tip | Clean tip, reflow with flux |
| SHIM misaligned | Tack shifted before full solder | Reflow one corner, reseat, retack opposite corner |
| Difficult pin trimming | Cutter not flush or joint too bulbous | Reflow to reduce solder volume, use sharp flush cutters |

## 12. Next Step

Proceed to Step 8 Part 3 – Mounting Pico + SHIM to Chassis & Power Distribution Planning (forthcoming).

---
Navigation: ← Previous [Step 7 – Fender Mechanism & Skirt Integration](./step7_fender_mechanism.md) | Next → (Pending Step 8 Part 2) | Index ↔ [Assembly Index](./README.md)

Revision: 0.1 (2025‑08‑24) Initial draft (Part 1) focusing on physical mounting only.
Revision: 0.2 (2025‑08‑24) Added Part 2 (Pico headers & Motor SHIM, expansion headers) and Part 3 Segment B (initial power wiring).
Revision: 0.3 (2025‑08‑24) Added Part 3 Segment C (I2S signal wiring, LED termination, head integration & chassis stack), standardized title.
