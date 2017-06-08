function thrustOn(r,c)

global saferglobals;

saferglobals.safer.switchboard(r,c) = 1;

updateSaferView(saferglobals.plot.drawing,saferglobals.safer.switchboard);
