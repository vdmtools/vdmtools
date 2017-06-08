hgtext = {'hgtext'};
aahtext = {'aahtext'};

contextswitch = {'contextswitch'};
hgaah = {'hgaah'};
hgbuttons = ...
   { ...
      'hgrrollright','hgrrlroff','hgrrollleft','hgryawright','hgrylroff', ...
      'hgryawleft','hgtdown','hgtudoff','hgtup','hgtright','hgtlroff', ...
      'hgtleft', 'hgpitchdown', 'hgpudoff', 'hgpitchup', 'hgbackward', ...
      'hgfboff','hgforward' ...
   };

aahbuttons = ...
   { ...
      'aahrollright','aahrlroff','aahrollleft','aahpitchdown','aahpudoff', ...
      'aahpitchup','aahyawright','aahylroff','aahyawleft' ...
   };
hgframes = {'hgouterframe','hgframe1','hgframe2','hgframe3','hgframe4'};
aahframes = {'aahouterframe','aahframe1','aahframe2','aahframe3'};
thrustbutton = {'applythrust'};


hgtext_h = findobjects(hgtext);
aahtext_h = findobjects(aahtext);

contextswitch_h = findobjects(contextswitch);
hgaah_h = findobjects(hgaah);

hgbuttons_h = findobjects(hgbuttons);
aahbuttons_h = findobjects(aahbuttons);

hgframes_h = findobjects(hgframes);
aahframes_h = findobjects(aahframes);

thrustbutton_h = findobjects(thrustbutton);

hgcontrolframe = union(hgbuttons_h,hgframes_h(2:5));
aahcontrolframe = union(aahbuttons_h,aahframes_h(2:4));

allcontrols = union(hgtext_h, aahtext_h);
allcontrols = union(allcontrols, contextswitch_h);
allcontrols = union(allcontrols, hgaah_h);
allcontrols = union(allcontrols, hgbuttons_h);
allcontrols = union(allcontrols, aahbuttons_h);
allcontrols = union(allcontrols, hgframes_h);
allcontrols = union(allcontrols, aahframes_h);
allcontrols = union(allcontrols, thrustbutton_h);
