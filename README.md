# JDM — hands-on workshop

Only src/jdm game modules changed. OS screens, keyboard bus, audio, other games,
board, wiring and library versions remain unchanged.

Controls: up/down select one of five contextual actions; Enter performs it.
Left/right selects a diagnostic node, or a tool during hand repair and QC.
Tab toggles garage/component view. 1/2/3 selects save slot. H hires a mechanic
(500, then 150/day). M delegates a diagnosed job with a purchased part.
Esc saves and exits; failed saving keeps the game open.

Loop: accept client, read symptom, select and test one of four nodes. Healthy
nodes report OK; the matching faulty node unlocks replacement purchase.
Enter starts hand repair, requiring five tool choices: remove fasteners,
remove old component, fit replacement, reconnect, tighten. The required tool
is shown; wrong tools do not advance or charge money. Drawn part position
changes on these steps, without continuous animation or a timer. Alternatively
hire and delegate, waiting 1–2 paid working days. QC requires selecting TESTER.
Collect payment, build reputation, accept a different client.

Ten original stylised vector car variants have circular tyres and five-spoke
rims, door/window seams, lamps, bumpers, grilles, optional spoiler/pop-up lamps,
and an open bonnet during repair. Four component diagrams include a drilled
circular brake disc, caliper/pads, fuel circuit, cooling circuit and suspension.
These are stylised illustrations, not exact manufacturer drawings.

Existing version-1 save record layout and slots are preserved. remaining=10–14
encodes the five manual steps; remaining=1–2 still represents mechanic days.
Old saves can be loaded by this firmware; older firmware cannot safely resume
new manual-repair saves. New-game reset requires a second Enter; creating a
new game alone does not overwrite the slot until saving a subsequent action.
No card formatting, artwork overwriting or other application's save changes.
No SD graphics reads are required by the new vector renderer.

Device checks: test wrong/right diagnostic nodes, five repair steps including
wrong tools, TESTER QC, payment, save/load mid-repair; separately load an old
mechanic job. Verify legibility and redraw time on the actual e-paper screen.
Compilation is not proof of physical screen operation.
