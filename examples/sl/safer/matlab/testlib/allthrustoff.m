function allThrustOff

global saferglobals;

saferglobals.safer.switchboard(1:4,1:6) = 0;

updateSaferView(saferglobals.plot.drawing,saferglobals.safer.switchboard);
