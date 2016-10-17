function allThrustOn

global saferglobals;

saferglobals.safer.switchboard(1:4,1:6) = 1;

updateSaferView(saferglobals.plot.drawing,saferglobals.safer.switchboard);
