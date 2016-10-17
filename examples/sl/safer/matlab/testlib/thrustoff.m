function thrustOff(r,c)

global saferglobals;

saferglobals.safer.switchboard(r,c) = 0;

updateSaferView(saferglobals.plot.drawing,saferglobals.safer.switchboard);
