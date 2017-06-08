From: "Marcel Verhoef" <Marcel.Verhoef@chess.nl>
Date: 2005”N11ŒŽ21“ú 2:50:19:JST
To: "'Shin Sahara from Home'" <ss@shinsahara.com>, "'John Fitzgerald'" <John.Fitzgerald@newcastle.ac.uk>
Cc: "'Peter Gorm Larsen'" <pgl@iha.dk>
Subject: VDM++ real-time stuff (copy of our working archive)
Reply-To: <Marcel.Verhoef@chess.nl>

Hello Shin, John,

As agreed in the netmeeting, here is the archive with all the goodies Peter
and I worked on during our session in Aarhus.

fme06.pdf contains the current draft of our FM'06 paper
vdmppsem.pdf is a "brain dump" document collecting ideas on the RT stuff
Improvement*.* is the current note sent to Shin, containing an annotated
specification (which runs through VICE no problems).

radnav/*  contains a working version of the case study that works with the
latest VICE release that Shin built with the 'time' construct. This
directory also contains showvice.jar (either double-click or run "java -jar
showvice.jar"). After running VICE a file called "logfile" is created. Load
this file using showvice. Select a part of the trace that you want to
visualise and go the the file menu and press "draw MSC".

VDM++ RT Action List.doc contains an overview of actions Peter and I agreed
wrt the work.

Have fun. Don't hesitate to ask questions.

Marcel Verhoef

Ps. Rename the attachment (replace _ with .)

--------------------------------------------------------------------------------------------------

From: Shin Sahara from Home <ss@shinsahara.com>
Date: 2005”N11ŒŽ21“ú 22:06:01:JST
To: <Marcel.Verhoef@chess.nl>
Cc: Shin Sahara from Home <ss@shinsahara.com>, "'John Fitzgerald'"  
<John.Fitzgerald@newcastle.ac.uk>, "'Peter Gorm Larsen'" <pgl@iha.dk>
Subject: Re: VDM++ real-time stuff (copy of our working archive)

DEar Marcel,

On 2005/11/21, at 2:50, Marcel Verhoef wrote:

> Hello Shin, John,
>
> As agreed in the netmeeting, here is the archive with all the  
> goodies Peter
> and I worked on during our session in Aarhus.
>
> fme06.pdf contains the current draft of our FM'06 paper
> vdmppsem.pdf is a "brain dump" document collecting ideas on the RT  
> stuff
> Improvement*.* is the current note sent to Shin, containing an  
> annotated
> specification (which runs through VICE no problems).
>
> radnav/*  contains a working version of the case study that works  
> with the
> latest VICE release that Shin built with the 'time' construct. This
> directory also contains showvice.jar (either double-click or run  
> "java -jar
> showvice.jar"). After running VICE a file called "logfile" is  
> created. Load
> this file using showvice. Select a part of the trace that you want to
> visualise and go the the file menu and press "draw MSC".
>
> VDM++ RT Action List.doc contains an overview of actions Peter and  
> I agreed
> wrt the work.
>
> Have fun. Don't hesitate to ask questions.

Thank you.
I'll use the files for advertising the real-time extensions to CSK  
and VDM users.

Still, I don't understand well of this case study and VICE.
I think I need a few weeks to reach asking questions level.

Cheers,
Shin

--------------------------------------------------------------------------------------------------

From: "Marcel Verhoef" <Marcel.Verhoef@chess.nl>
Date: 2005”N11ŒŽ22“ú 8:56:47:JST
To: "'Shin Sahara from Home'" <ss@shinsahara.com>
Cc: "'John Fitzgerald'" <John.Fitzgerald@newcastle.ac.uk>, "'Peter  
Gorm Larsen'" <pgl@iha.dk>
Subject: RE: VDM++ real-time stuff (copy of our working archive)
Reply-To: <Marcel.Verhoef@chess.nl>

> Still, I don't understand well of this case study and VICE.
> I think I need a few weeks to reach asking questions level.

I can understand that you have this problem. You are looking at a  
snapshot
of some ongoing research work - it is hard to grasp the idea when you  
are
not fully familiar with the case study. Have a look at section 3 of
http://www.cs.ru.nl/research/reports/info/ICIS-R05005.html that gives an
overview of the case study on a more abstract level. The VDM++ model
describes the same system. Another possible place to read about the case
study is www.mpa.ethz.ch

Hope this helps.

Marcel



 