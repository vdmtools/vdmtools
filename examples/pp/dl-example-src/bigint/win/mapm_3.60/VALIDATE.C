
/* 
 *  M_APM  -  validate.c
 *
 *  Copyright (C) 1999, 2000   Michael C. Ring
 *
 *  Permission to use, copy, and distribute this software and its
 *  documentation for any purpose with or without fee is hereby granted, 
 *  provided that the above copyright notice appear in all copies and 
 *  that both that copyright notice and this permission notice appear 
 *  in supporting documentation.
 *
 *  Permission to modify the software is granted, but not the right to
 *  distribute the modified code.  Modifications are to be distributed 
 *  as patches to released version.
 *  
 *  This software is provided "as is" without express or implied warranty.
 */

/*
 *      $Id: VALIDATE.C,v 1.1.1.1 2001/10/04 13:02:57 paulm Exp $
 *
 *      This file contains the validation test program. It compares 
 *	the M_APM library to the standard C lbrary math functions.
 *
 *      $Log: VALIDATE.C,v $
 *      Revision 1.1.1.1  2001/10/04 13:02:57  paulm
 *      Import of dir
 *
 *      Revision 1.15  2000/08/23 19:35:57  mike
 *      create test cases in POW where X is the same
 *      on successive calls
 *
 *      Revision 1.14  2000/07/04 20:40:35  mike
 *      increase tolerance for EXP (because of MSVC C++)
 *
 *      Revision 1.13  2000/06/20 21:23:41  mike
 *      add factorial check
 *
 *      Revision 1.12  2000/06/20 21:08:58  mike
 *      add local versions of the inverse hyperbolioc functions
 *
 *      Revision 1.11  2000/05/24 17:01:05  mike
 *      revert to prev version
 *
 *      Revision 1.10  2000/05/23 23:16:52  mike
 *      add a test case of 0^0 for the POW function
 *
 *      Revision 1.9  2000/04/03 20:27:47  mike
 *      don't do inverse hyperbolic functions for certain compilers
 *
 *      Revision 1.8  2000/04/03 19:44:32  mike
 *      added tests for new functions, CBRT plus hyperbolics
 *
 *      Revision 1.7  1999/09/19 23:26:36  mike
 *      added checks for new sin_cos and integer_pow functions
 *
 *      Revision 1.6  1999/08/15 02:01:59  mike
 *      added check to verify a large digit multiply
 *
 *      Revision 1.5  1999/06/01 20:57:27  mike
 *      change limits for EXP test
 *
 *      Revision 1.4  1999/05/31 02:06:30  mike
 *      added more comments
 *
 *      Revision 1.3  1999/05/28 21:23:39  mike
 *      add 'L' to set_long constants so no warnings from dos compile
 *
 *      Revision 1.2  1999/05/19 02:25:26  mike
 *      added some comments
 *
 *      Revision 1.1  1999/05/10 20:56:52  mike
 *      Initial revision
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "m_apm.h"

extern   void	  factorial_local(M_APM, M_APM);
extern   double   cbrt_local(double);
extern   double   asinh_local(double);
extern   double   acosh_local(double);
extern   double   atanh_local(double);

#define  LPI 3.14
#define  DECIMAL_PLACES 22
#define  FALSE 0
#define  TRUE 1

/* 
 *   create 2 'big' numbers to multiply and the expected answer.
 *   break the strings up into smaller pieces in case some compiler
 *   can not handle large literal strings.
 *
 *   used to test the fast multiply function which is only called
 *   (from 'm_apm_multiply') when the numbers are big enough.
 */

char big_num1[] = 
"5477.225575051611345696978280080213395274469499798325422689444973249\
327712272273380085843616387062576472682594285139224407200789882646558\
891471252604198935050656868264028988871051985377336304477847146986536\
309464713769216003880339617152830231153776148436193230087510835045256\
841092499066463397468011771810378709455463696797501196899751566966529\
446190616232069835581188127691864849072857146393831760238110087068567\
545626768538001478011551032000988502121305564004308018158026340813077";

char n1_a[] = 
"772177288685963910174142042278285110313723344445000002230060603984380\
798460001089869470048466029669117314706984809266832417977780903592624\
884250052380340623031446945927849505090473441248476516630231901432458\
465470895720912750543153323814420283523602258724058827551086155420853\
549896859702245888044277695896888089137210299021244272989058086629232\
576996412627679439967245590529754746902894163459053669235116328295754\
269941165926041461334545029966488544476427825505493174397776835641954";

char n1_b[] = 
"803556210450098389167380043825338101964431316065425058342596003547609\
904657373788602536274545607842879287325981760193062893370008723642669\
583372655427744596226409687803089876730115386596785649629948510927663\
393406329278774888410893062050014150190163215662530846891425696958844\
563601539213463268418908343109892922967762021901527734656639251982721\
533276580844564415304040659677509189970245006326513002463949710549268\
106515075809478710491399532823456311455040428386372045332335947051862";

char n1_c[] = 
"107566090676024222160666041340373838140015621780826268608215475193042\
085909162411131535918626500007372172272166969662646924163898017154388\
924535072240403662376882822035715115480647541706733181141296870585304\
928296854112454022329865847883473128439068670368652079008964943058144\
820568903922110708522528902086061260637459724917015798127554599990205\
334267204696828565535565382702261199373810507094501369669070306198633\
268220885260341553910812447157255688794411097585457824586540244444870";

char n1_d[] = 
"645354708408302093814115659361825948857361575360568865920738892841239\
398707494024576533538344592522706383753761907235989567746208346286981\
677249809575600353365154506909850542892657368852225576389405999679521\
853973957713685970878062104239209047062390311819359466043396336421724\
756753847020064083846850621548822321639378227857127677474264942632421\
653099331868781760919519456262199115365346726974660854989627852251634\
707491124736774691596889405976078907899247795615358960653585441580810";

char n1_e[] = 
"748666767739518891996987585043848349662696742139884472561211152841802\
954533401340226489505333875414272425646094971740253522859652408565668\
863447964641115937985350193531532885850711826196626337273944492563483\
259084655820518286797604154540875576626749422228468209306494383781835\
466692824010610209059491462476604949202385591997221848405467759364646\
620817399888905999540978012700218199136868764520350056533442518085222\
081250439619242602140331198913448649803205131109500779955624687728499";

char n1_f[] = 
"080634505486398179517820385621207768210644429659349308856399473400215\
480148692156144664654084266512636501751278809698965272745451147389352\
923649828123384641785124557446285391136457412841547142091655084507860\
067060210455513815859327040353325926295812429793211285475624871783979\
9593001432570138484284719176";


char big_num2[] = 
"6514.715853424340416942752577264296187121306025367890569443551130907\
729893380845941121480912250083701709961994013939170316266199428200423\
181498676875099211842524295697140830485800366843900975401812246755025\
061275147059045317597210288379660264486830441830653204173724625026940\
394583591075467869557828008070140920386948093483655949041332579908484\
629223526590699329131103050961112067682310593783780900650591377914554\
317053419236023223609205772830690587686006384337810995107867379330427";

char n2_a[] = 
"047899359697886753836323500940769371586171649487075231830837894276236\
573178978571382766214874384456894513165751114001281977687149413558504\
262469948651597406268299470045780821483076290148810381084742375722693\
554860721246057386892947817147378362955406567295359048952429337420586\
853395916148464753080519270260650043968923102295905598309489476339681\
131415673651447295247802813239749197364680131153277808835083832352469\
820128522571771146617324110851840436230944314666009354032880245598468";

char n2_b[] = 
"780599400834696470289637192809782909127534659581201633650133923054669\
556413244866675259484405718027539875492422869948551075316101797863190\
127973038491633241283425571135748946635096608880013299696310971163558\
626476451924618073763206196058952486224472001218830058237210175044184\
794523589607766088671796252592559649392020738192597338297346413566673\
390699927246801333082447053825876097620044308734563395035241548467264\
514978281458393504885237629229485144144681244830437495349297331121852";

char n2_c[] = 
"689270947044229390885822885020701987621900891946111900170887636220008\
745563952914591199749579914838000696439497235994480248666065128269951\
246359465819867258101213408256388391242501647222367863315417773786981\
526450325597730711685173525165280598003651897366233889161621431738941\
166028286176546703923793707094101611677933417679834705618779730590405\
327796829501503558378692469267365107857520795471401695657833047567208\
187320943323142374714584147422285042112993236985894396528082500150526";

char n2_d[] = 
"941236962849625531690970669603996946104732645648954005788060764072469\
039827600220246272449855718207221292420582718864323339502635187631819\
481073492284575881989960810934800499976066406397229279588528473331179\
590381976444746127468195072434162519195479664174180726342017233797562\
908808427087534609285859574539440892718383076986184002452805275904938\
952963112928770646292088050345919369640527290803168759396257170990943\
661762609781771445128848637641077764093153244630296207441991755977261";

char n2_e[] = 
"504144161329226123802432280526511639354026043641685873585710586520532\
707796295440630795110953576508207621813351560207201330085660633841677\
544018657566836986809083991414692602972015286878313586668346438830912\
229604279463854018324838424895516048813617031807822979046341781128407\
803189342050611698862068994451927771666408053963083582992050539687162\
414932328948904479502003336731875042015564541106062763985510221651964\
318227297735738839673795805230803288831387891975416212383779577752383";

char n2_f[] = 
"326706778125462842858059684155452002602687277565013235679104248282207\
147049365498770248263078235748265454801754914755170456445373198716942\
854177765433699371551655417657836505254274898400588622871161570788251\
533104651235660583982932518238513788785630566269514662678885716406743\
0154064676607189264538398139";


char multiply_answer[] = 
"35682568.286569981911213676242476439230777451808105840695691450976780\
177284400721701694102362592832198174809643503236516226804124772144853\
130198876385301631897383384884088639408767977014746395212327187126827\
040992987319746814027602627716377087505309943855050330385637557207396\
838835567729886840418219845302141425540751388808255364323594313768429\
422012108507028254351717518615407231598550966581332579685822053715353\
847396744220232676340267928081616756905347732423593842317584126721443";

char ma_a[] = 
"213519864935592372168050389313169906970105627148510854092320709098409\
609456478257162887626852882219938795879853643268991184302684703829614\
416502056660023096579774232816646351399716776870651587840692884332635\
310908011418101688997451727750963117609891389816564466810343642938796\
022705186794012007538477728371345651981195626058702351653621857973263\
799492207725410581958194275454644820738380545121305481044930658916351\
139106041471011765141657459068913498599558900409959829529176608832328";

char ma_b[] = 
"636540787325113396478049429405932790488593656351114899733561742081091\
592646010701537901460704932199381204448382765980317794238867591353088\
538682626147486760667900933381270893049333771821756613382507682400527\
255850343468000261394254594053834636140677526285911535209397651594556\
218998907821342272518952350370910852492116379223160658217476386981439\
083772315538567577682264002165416159554285832319247433857223426712566\
280687976098631020214035568466094255192989180116412889846863500101904";

char ma_c[] = 
"512818015829665579995565592791574036048735298163191457507808914055615\
612950759770454648751206906673314833361014339616581409298889360247070\
599047018300534985312274851894059319995700629358162468032366252290776\
133480926002284743875443267872175406139623121014944780854362904094191\
462293842209824943493633400002724196213091458374452520742971395765231\
589556628088853917398621969478172633532201065241116769282425898908424\
919675736636956283232450615148758763523403792660951044156986055308645";

char ma_d[] = 
"193740224728090761145990194391790904302686007369523333603220243380898\
952677885399293862649648186996495872708306455147831637190028315925810\
789115750063950896474450694169915427712169526443692940171631879875697\
685859929312803218422616230045438334131427162751022213920244856722979\
172927195674061549004733274820657260436494935009006215257847115148268\
661400593789849954334444884713309055795246507756578880813699084686826\
290879317628226189335262723374507000888790215151722617194127494160265";

char ma_e[] = 
"796306222899863963979596208586219389127048324006845844612435101611281\
477113759442271619657464300597771967256896393829132417069454783570627\
441809701660359695849799014190847395243792906960486494621484545239047\
683053805505440608431322318656714794332002817571646612858765346539524\
118243377471206535267915559294581304697655346690031467253320220355257\
223088597159683754663270102435067402448297053127252173468926003569315\
359378824065985266501355579849961711921610115340187465542634530103318";

char ma_f[] = 
"659402225992256606810385930594690825468182057151298250932937158133030\
677293431015858256859102052906890198719718647269468930821561891728463\
585287457490504007626854506088597722498041094324103723120511950085264\
254478293217409841897993170848716556717111495451336298362270312235815\
677114307495662773594234174442940981344768442654856841097478687029122\
882587142568802689287200134417728345982669360377953073698075593033281\
886245367488614214356643458744513084768083647413265333048625217126584";

char ma_g[] = 
"630948888964467207598935157330510882187408752822358231773877096831818\
643846176837068527144973165429712739720537823287261761833392368351930\
184070344148559528410179483068478202138795090647914823519787275577829\
425424857951671927388829041722226521510658295948123950994841662228143\
740272297111230977866790457183727563466847984138902855678199609116246\
371025834702410681410528746216001155604263598906367674908894529230065\
669171327048435709075799547698833316080863754547997965010592161272128";

char ma_h[] = 
"602731158422805520555601351300561867526086169665213837592200058456667\
583405800959234444225209402980860164720033430877566381822706692679506\
968090527517042043461862800812585379923042322324166025644747530671636\
747032197602396330168897438140102972343020229684832835043500816204147\
347368332205563208832944812925901419321126286323340738331772846612609\
488366002683659396141147185157734056984122386898139898453742234192872\
122037685460111827429310893917992288933869586634250862628926222459995";

char ma_i[] = 
"025012823193914349434410003132468599354421896299269545329051980262658\
142221082805507746380651456256442074265484520869667367485299810838555\
485427104670947792064992428309689253768188934615890446149413178556733\
757771440990957413174136485534944679877819377868453179227777396677962\
624476864891562006266871442421195994464294361770016973082159833476515\
222178193986995105514359931761402343571176562436328806721711137549821\
316119750879295934828480412074677200183415239356033257731155034247897";

char ma_j[] = 
"891795520646642261429211462374597191408558958865753282352159307133106\
663721204458575897538513144330883203387614890872077499798491224174845\
142297352122291876037056352321098855229709693809597423118678284145990\
665911206525728578108768255908640964916223635836306727288205705061553\
584684510288830959080640871638182590791526553488915696218207841905328\
597077019131953657341611282298175098699153376664989094059343995967374\
292423530758157288403604632551496549590693942532906758253545900586642";

char ma_k[] = 
"660836917035175298364748744326069664062062723040799147960315635274195\
774323316774713611601782168350661067337798329512399601641888330127599\
988067566015922831276953959188169812159221767077154994933325314014424\
846566313010909740617158085922805186058781791220395554763403891899226\
821514079086504306971309039508970931185654077489693871110638028064515\
134154112378672680686135335253581103293532469341569150073030104634700\
963997502135050628020198910845435556292905399318158011051258941183036";

char ma_l[] = 
"280444816449591224725544346133165635600496287664582691563900374320179\
699256394555626938991595170092335253953309810227528263598336859258772\
170980705034499193004631633488584307584810517636624596016064537361173\
980514467887414201123908157315839507813339746978334108813941313272503\
565753947557352126454216021347221387771562927386434939770539315783824";

char ma_m[] = 
"415969617812866213049796091597073113330021091820057240750008251660513\
262996470977133803397382032407881409117835133251508876570097629139255\
254507215533974547901331590899003284116183113254815555052684410949446\
44877737521253451928706974972437725833300014496013464";


/*
 *  start of main test procedure
 */

int main(argc, argv)
int argc;  char *argv[];
{
char	 cbuf[64], buffer2[32], *buffer, *num1, *num2, *answer;
int      total_pass, pass, numdigits, sflag, i, j, k;
double   ydiff, tolerance, yy, yy_mapm_lib, yy_sin, yy_cos,
	 yarray[20], xarray[20], xx, xx1, xx2, xdelta;

				/* declare the M_APM variables ... */
M_APM    aa_mapm;
M_APM    bb_mapm;
M_APM    cc_mapm;
M_APM    dd_mapm;
M_APM    ee_mapm;
				/* now initialize the M_APM variables ... */
aa_mapm = m_apm_init();
bb_mapm = m_apm_init();
cc_mapm = m_apm_init();
dd_mapm = m_apm_init();
ee_mapm = m_apm_init();

if ((num1 = malloc(3250 * sizeof(char))) == NULL)
  {
   fprintf(stderr,"not enough memory\n");
   exit(2);
  }

if ((num2 = malloc(3250 * sizeof(char))) == NULL)
  {
   fprintf(stderr,"not enough memory\n");
   exit(2);
  }

if ((answer = malloc(6500 * sizeof(char))) == NULL)
  {
   fprintf(stderr,"not enough memory\n");
   exit(2);
  }

/* build the large strings ... */

strcpy(num1, big_num1);
strcat(num1, n1_a);
strcat(num1, n1_b);
strcat(num1, n1_c);
strcat(num1, n1_d);
strcat(num1, n1_e);
strcat(num1, n1_f);

strcpy(num2, big_num2);
strcat(num2, n2_a);
strcat(num2, n2_b);
strcat(num2, n2_c);
strcat(num2, n2_d);
strcat(num2, n2_e);
strcat(num2, n2_f);

strcpy(answer, multiply_answer);
strcat(answer, ma_a);
strcat(answer, ma_b);
strcat(answer, ma_c);
strcat(answer, ma_d);
strcat(answer, ma_e);
strcat(answer, ma_f);
strcat(answer, ma_g);
strcat(answer, ma_h);
strcat(answer, ma_i);
strcat(answer, ma_j);
strcat(answer, ma_k);
strcat(answer, ma_l);
strcat(answer, ma_m);

total_pass = TRUE;

fprintf(stdout,"Output the first 10 random numbers ...\n");

for (i=1; i <= 10; i++)
  {
   m_apm_get_random(aa_mapm);
   m_apm_to_string(buffer2, 14, aa_mapm);

   fprintf(stdout,"Random Num %2d : %s \n",i,buffer2);
  }

i = strlen(num1) - 1;      /* ignore the decimal point in the count */
j = strlen(num2) - 1;

fprintf(stdout,"Validating the fast multiply function ...\n");
fprintf(stdout,"Multiply a %d digit number by a %d digit number\n",i,j);

m_apm_set_string(aa_mapm, num1);
m_apm_set_string(bb_mapm, num2);
m_apm_set_string(ee_mapm, answer);

m_apm_multiply(cc_mapm, aa_mapm, bb_mapm);

k = m_apm_compare(ee_mapm, cc_mapm);
if (k == 0)
   fprintf(stdout,"... Fast multiply function passes\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"... Fast multiply function FAILS\n");
  }

fprintf(stdout,"Validating the SQRT function ... (10 -> 1000)\n");

xx        = 10.0;
xdelta    = 1.02576;
pass      = TRUE;
tolerance = 1.0E-14;
buffer    = answer;

while (1)
  {
   sprintf(cbuf,"%.6E",xx);
   xx1 = atof(cbuf);

   yy = sqrt(xx1);

   m_apm_set_string(aa_mapm, cbuf);
   m_apm_sqrt(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx1,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   if (xx > 1000.0)  
     break;

   xx *= xdelta;
  }

if (pass)
  fprintf(stdout,"... SQRT function passes\n");
  
/*
 *
 */

fprintf(stdout,"Validating the CBRT function ... (10 -> 10000)\n");

xx        = 10.0;
xdelta    = 1.0873;
pass      = TRUE;
tolerance = 1.0E-14;
buffer    = answer;
sflag     = 1;

while (1)
  {
   sprintf(cbuf,"%.6E",(sflag * xx));
   xx1 = atof(cbuf);

   yy = cbrt_local(xx1);

   m_apm_set_string(aa_mapm, cbuf);
   m_apm_cbrt(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx1,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   if (xx > 10000.0)  
     break;

   xx *= xdelta;
   sflag = -sflag;
  }

if (pass)
  fprintf(stdout,"... CBRT function passes\n");
  
/*
 *
 */

fprintf(stdout,"Validating the SIN function ... (-PI -> +PI)\n");

xx        = -LPI;
xdelta    = 0.035;
pass      = TRUE;
tolerance = 1.0E-14;

while (1)
  {
   yy = sin(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_sin(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > LPI)  
     break;
  }

if (pass)
  fprintf(stdout,"... SIN function passes\n");
  
/*
 *
 */

fprintf(stdout,"Validating the COS function ... (-PI -> +PI)\n");

xx        = -LPI;
xdelta    = 0.035;
pass      = TRUE;
tolerance = 1.0E-14;

while (1)
  {
   yy = cos(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_cos(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > LPI)  
     break;
  }

if (pass)
  fprintf(stdout,"... COS function passes\n");
  
/*
 *
 */

fprintf(stdout,"Spot check the SIN_COS function ... \n");

xx        = -LPI;
xdelta    = 0.475;
pass      = TRUE;
tolerance = 1.0E-14;

while (1)
  {
   yy_sin = sin(xx);
   yy_cos = cos(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_sin_cos(bb_mapm, cc_mapm, DECIMAL_PLACES, aa_mapm);

   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy_sin;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy_sin);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   m_apm_to_string(buffer, DECIMAL_PLACES, cc_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy_cos;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy_cos);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > LPI)  
     break;
  }

if (pass)
  fprintf(stdout,"... SIN_COS function passes\n");

/*
 *
 */

fprintf(stdout,"Validating the TAN function ... (-PI -> +PI)\n");

xx        = -LPI;
xdelta    = 0.035;
pass      = TRUE;
tolerance = 1.0E-12;

while (1)
  {
   yy = tan(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_tan(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   if (fabs(yy) > 1.0)
     ydiff = (yy_mapm_lib - yy) / yy;
   else
     ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %23.16E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %23.16E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > LPI)  
     break;
  }

if (pass)
  fprintf(stdout,"... TAN function passes\n");
  
/*
 *
 */

fprintf(stdout,"Validating the ARC-SIN function ... (-1 -> +1)\n");

xx        = -0.9999;
xdelta    = 0.035;
pass      = TRUE;
tolerance = 1.0E-14;

while (1)
  {
   yy = asin(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_asin(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > 0.9999)  
     break;
  }

if (pass)
  fprintf(stdout,"... ARC-SIN function passes\n");
  
/*
 *
 */

fprintf(stdout,"Validating the ARC-COS function ... (-1 -> +1)\n");

xx        = -0.9999;
xdelta    = 0.035;
pass      = TRUE;
tolerance = 1.0E-14;

while (1)
  {
   yy = acos(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_acos(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > 0.9999)  
     break;
  }

if (pass)
  fprintf(stdout,"... ARC-COS function passes\n");
  
/*
 *
 */

fprintf(stdout,
   "Validating the ARC-TAN function ... (-PI/2 < tan(x) < PI/2)\n");

xx        = -1.57;
xdelta    = 0.035;
pass      = TRUE;
tolerance = 1.0E-14;

while (1)
  {
   xx1 = tan(xx);
   sprintf(cbuf,"%.6E",xx1);
   xx1 = atof(cbuf);

   yy = atan(xx1);
   
   m_apm_set_string(aa_mapm, cbuf);
   m_apm_atan(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);
   
   ydiff = yy_mapm_lib - yy;
   
   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx1,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > 1.57)
     break;
  }

if (pass)
  fprintf(stdout,"... ARC-TAN function passes\n");
  
/*
 *
 */

fprintf(stdout,
   "Spot check the ARC-TAN2 function (calls ARC-TAN) ...\n");

pass      = TRUE;
tolerance = 1.0E-14;

xarray[1] = -1.638;
xarray[2] = -2.179;
xarray[3] = 0.0;
xarray[4] = 0.842;
xarray[5] = 1.976;

yarray[1] = -0.374;
yarray[2] = -4.127;
yarray[3] = 0.0;
yarray[4] = 0.732;
yarray[5] = 3.261;

for (i=1; i <= 5; i++)
  {
   for (j=1; j <= 5; j++)
     {
      if (i != 3 || j != 3)        /* domain error if both == 0.0 */
        {
         xx1 = xarray[i];
         xx2 = yarray[j];
   
         yy = atan2(xx2, xx1);
               
         m_apm_set_double(aa_mapm, xx1);
         m_apm_set_double(bb_mapm, xx2);
         m_apm_atan2(cc_mapm, DECIMAL_PLACES, bb_mapm, aa_mapm);
         m_apm_to_string(buffer, DECIMAL_PLACES, cc_mapm);
         yy_mapm_lib = atof(buffer);
               
         ydiff = yy_mapm_lib - yy;
               
         if (fabs(ydiff) > tolerance)
           {
            pass = FALSE;
            total_pass = FALSE;
            
            fprintf(stdout,
            "x2 = %10.3E  x1 = %10.3E  C-math-lib = %.15E \n",xx2,xx1,yy);
            fprintf(stdout,
            "DIFF = %17.10E          M_APM_LIB  = %s \n",ydiff,buffer);
           }
        }
     }
  }

if (pass)
  fprintf(stdout,"... ARC-TAN2 function passes\n");


/*
 *
 */

fprintf(stdout,
   "Validating the SIN, COS, TAN functions ... (|input| > PI)\n");

xx        = 2.970;
xdelta    = 0.711;
pass      = TRUE;
tolerance = 1.0E-13;

while (1)
  {
   sprintf(cbuf,"%.6E",xx);
   xx1 = atof(cbuf);

   for (k=0; k <= 1; k++)        /* loop for pos angles, then neg */
     {
      if (k == 1) 
        xx1 = -xx1;

      yy = sin(xx1);
      
      m_apm_set_double(aa_mapm, xx1);
      m_apm_sin(bb_mapm, DECIMAL_PLACES, aa_mapm);
      m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
      yy_mapm_lib = atof(buffer);
      
      ydiff = yy_mapm_lib - yy;
      
      if (fabs(ydiff) > tolerance)
        {
         pass = FALSE;
         total_pass = FALSE;
   
         fprintf(stdout,
	   "SIN: x    = %12.6E   C-math-lib = %.15E \n",xx1,yy);

         fprintf(stdout,
	   "SIN: DIFF = %12.6E   M_APM_LIB  = %s \n",ydiff,buffer);
        }

      yy = cos(xx1);
      
      m_apm_set_double(aa_mapm, xx1);
      m_apm_cos(bb_mapm, DECIMAL_PLACES, aa_mapm);
      m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
      yy_mapm_lib = atof(buffer);
      
      ydiff = yy_mapm_lib - yy;
      
      if (fabs(ydiff) > tolerance)
        {
         pass = FALSE;
         total_pass = FALSE;
   
         fprintf(stdout,
	   "COS: x    = %12.6E   C-math-lib = %.15E \n",xx1,yy);

         fprintf(stdout,
	   "COS: DIFF = %12.6E   M_APM_LIB  = %s \n",ydiff,buffer);
        }

      yy = tan(xx1);
      
      m_apm_set_double(aa_mapm, xx1);
      m_apm_tan(bb_mapm, DECIMAL_PLACES, aa_mapm);
      m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
      yy_mapm_lib = atof(buffer);
      
      if (fabs(yy) > 1.0)
        ydiff = (yy_mapm_lib - yy) / yy;
      else	
        ydiff = yy_mapm_lib - yy;
      
      if (fabs(ydiff) > tolerance)
        {
         pass = FALSE;
         total_pass = FALSE;
   
         fprintf(stdout,
	   "TAN: x    = %12.6E   C-math-lib = %.15E \n",xx1,yy);

         fprintf(stdout,
	   "TAN: DIFF = %12.6E   M_APM_LIB  = %s \n",ydiff,buffer);
        }
     }

   if (xx > 40.0)
     break;

   xx += xdelta;
  }

if (pass)
  fprintf(stdout,"... SIN, COS, TAN (> PI) functions pass\n");

/*
 *
 */

fprintf(stdout,"Validating the SINH function ... (-4 -> +4)\n");

xx        = -4.0;
xdelta    = 0.0451;
pass      = TRUE;
tolerance = 1.0E-13;

while (1)
  {
   yy = sinh(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_sinh(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > 4.0)  
     break;
  }

if (pass)
  fprintf(stdout,"... SINH function passes\n");
  
/*
 *
 */

fprintf(stdout,"Validating the COSH function ... (-4 -> +4)\n");

xx        = -4.0;
xdelta    = 0.0451;
pass      = TRUE;
tolerance = 1.0E-13;

while (1)
  {
   yy = cosh(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_cosh(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > 4.0)  
     break;
  }

if (pass)
  fprintf(stdout,"... COSH function passes\n");
  
/*
 *
 */

fprintf(stdout,"Validating the TANH function ... (-4 -> +4)\n");

xx        = -4.0;
xdelta    = 0.0451;
pass      = TRUE;
tolerance = 1.0E-13;

while (1)
  {
   yy = tanh(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_tanh(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > 4.0)  
     break;
  }

if (pass)
  fprintf(stdout,"... TANH function passes\n");
  

/*
 *
 */

fprintf(stdout,"Validating the ASINH function ... (-25 -> +25)\n");

xx        = -25.0;
xdelta    = 0.651;
pass      = TRUE;
tolerance = 1.0E-13;

while (1)
  {
   yy = asinh_local(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_asinh(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > 25.0)  
     break;
  }

if (pass)
  fprintf(stdout,"... ASINH function passes\n");
  
/*
 *
 */

fprintf(stdout,"Validating the ACOSH function ... (+1 -> +30)\n");

xx        = 1.003; 
xdelta    = 0.351;
pass      = TRUE;
tolerance = 1.0E-13;

while (1)
  {
   yy = acosh_local(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_acosh(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > 30.0)  
     break;
  }

if (pass)
  fprintf(stdout,"... ACOSH function passes\n");
  
/*
 *
 */

fprintf(stdout,"Validating the ATANH function ... (-1 -> +1)\n");

xx        = -0.999;
xdelta    = 0.0481;
pass      = TRUE;
tolerance = 1.0E-13;

while (1)
  {
   yy = atanh_local(xx);

   m_apm_set_double(aa_mapm, xx);
   m_apm_atanh(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);

   ydiff = yy_mapm_lib - yy;

   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   xx += xdelta;
   if (xx > 0.999) 
     break;
  }

if (pass)
  fprintf(stdout,"... ATANH function passes\n");
  

/*
 *
 */

fprintf(stdout,
   "Validating the EXP function ... (-120 -> 120)\n");

xx        = -120.4;
xdelta    = 0.8307;
pass      = TRUE;
tolerance = 5.0E-14;

while (1)
  {
   sprintf(cbuf,"%.6E",xx);
   xx1 = atof(cbuf);

   yy = exp(xx1);
   
   m_apm_set_string(aa_mapm, cbuf);
   m_apm_exp(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);
   
   ydiff = (yy_mapm_lib - yy) / yy;
   
   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx1,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   if (xx > 120.4)
     break;

   xx += xdelta;
  }

if (pass)
  fprintf(stdout,"... EXP function passes\n");
  
  
/*
 *
 */

fprintf(stdout,
   "Validating the LOG function ... (3.1E-7 -> 3.3E+6)\n");

xx        = -15.01;
xdelta    = 0.351;
pass      = TRUE;
tolerance = 1.0E-14;

while (1)
  {
   xx1 = exp(xx);
   sprintf(cbuf,"%.6E",xx1);
   xx1 = atof(cbuf);

   yy = log(xx1);
   
   m_apm_set_string(aa_mapm, cbuf);
   m_apm_log(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);
   
   ydiff = yy_mapm_lib - yy;
   
   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx1,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }

   if (xx > 15.0)
     break;

   xx += xdelta;
  }

if (pass)
  fprintf(stdout,"... LOG function passes\n");
  
/*
 *
 */

fprintf(stdout,
   "Spot check the LOG10 function (calls LOG) ...\n");

pass      = TRUE;
tolerance = 5.0E-14;

xarray[1]  = 1.638;
xarray[2]  = 8.321e-14;
xarray[3]  = 1.0;
xarray[4]  = 7.3231e17;
xarray[5]  = 3.735e-8;
xarray[6]  = 9.997e4;
xarray[7]  = 1.0528e3;
xarray[8]  = 6.319e-6;
xarray[9]  = 2.108e9;
xarray[10] = 8.417e-3;
xarray[11] = 5.347e7;
xarray[12] = 4.291e-23;
xarray[13] = 3.874e28;
xarray[14] = 9.0e2;
xarray[15] = 4.935e-4;

for (i=1; i <= 15; i++)
  {
   xx1 = xarray[i];
   yy  = log10(xx1);
               
   m_apm_set_double(aa_mapm, xx1);
   m_apm_log10(bb_mapm, DECIMAL_PLACES, aa_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, bb_mapm);
   yy_mapm_lib = atof(buffer);
               
   ydiff = yy_mapm_lib - yy;
               
   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;

      fprintf(stdout,"x    = %17.10E   C-math-lib = %.15E \n",xx1,yy);
      fprintf(stdout,"DIFF = %17.10E   M_APM_LIB  = %s \n",ydiff,buffer);
     }
  }

if (pass)
  fprintf(stdout,"... LOG10 function passes\n");
  
/*
 *
 */

fprintf(stdout,
   "Spot check the POW function (calls EXP) ...\n");

pass      = TRUE;
tolerance = 1.0E-14;

xarray[1]  = 1.638;         yarray[1]  = 7.14;
xarray[2]  = 8.321e-12;     yarray[2]  = -5.2;
xarray[3]  = 1.0;           yarray[3]  = 4.19;
xarray[4]  = 7.3231e17;     yarray[4]  = 2.11;
xarray[5]  = 2.0;           yarray[5]  = 32.0;
xarray[6]  = 9.997e4;       yarray[6]  = -1.0;
xarray[7]  = 1.0528e3;      yarray[7]  = -3.83e-2;
xarray[8]  = 6.319e-6;      yarray[8]  = 3.17;
xarray[9]  = 2.108e9;       yarray[9]  = -2.0;
xarray[10] = 8.417e-3;      yarray[10] = 0.5;
xarray[11] = 8.417e-3;      yarray[11] = -6.2109;
xarray[12] = 5.347e7;       yarray[12] = 1.0;
xarray[13] = 10.0;          yarray[13] = -17.0;
xarray[14] = 10.0;          yarray[14] = 2.33e1;
xarray[15] = 9.0e2;         yarray[15] = 0.0;
xarray[16] = 2.0;           yarray[16] = -3.62;
xarray[17] = 2.0;           yarray[17] = 23.017;
xarray[18] = 8.27693;       yarray[18] = 5.19832;

for (i=1; i <= 18; i++)     
  {
   xx1 = xarray[i];
   xx2 = yarray[i];
   yy  = pow(xx1, xx2);
               
   m_apm_set_double(aa_mapm, xx1);
   m_apm_set_double(bb_mapm, xx2);
   m_apm_pow(cc_mapm, DECIMAL_PLACES, aa_mapm, bb_mapm);
   m_apm_to_string(buffer, DECIMAL_PLACES, cc_mapm);
   yy_mapm_lib = atof(buffer);
               
   ydiff = (yy_mapm_lib - yy) / yy;
               
   if (fabs(ydiff) > tolerance)
     {
      pass = FALSE;
      total_pass = FALSE;
            
      fprintf(stdout,
      "x1 = %10.3E  x2 = %10.3E  C-math-lib = %.15E \n",xx1,xx2,yy);
       fprintf(stdout,
      "DIFF = %17.10E          M_APM_LIB  = %s \n",ydiff,buffer);
     }
  }

if (pass)
  fprintf(stdout,"... POW function passes\n");
  

/*
 *
 */

numdigits = 70;

fprintf(stdout,
   "\nVerify Some Known Quantities to 70 Digits ...\n");

m_apm_sqrt(ee_mapm, (numdigits + 4), MM_Two);
m_apm_divide(dd_mapm, (numdigits + 4), ee_mapm, MM_Two);
m_apm_round(aa_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"SQRT(2) / 2 = \n[%s]\n",buffer);

m_apm_set_long(ee_mapm, 4L);
m_apm_divide(dd_mapm, (numdigits + 4), MM_PI, ee_mapm);
m_apm_sin(bb_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"SIN(PI / 4) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: SQRT(2) / 2 == SIN(PI / 4)\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: SQRT(2) / 2 != SIN(PI / 4)\n");
  }

m_apm_cos(bb_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"COS(PI / 4) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: SQRT(2) / 2 == COS(PI / 4)\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: SQRT(2) / 2 != COS(PI / 4)\n");
  }


m_apm_set_string(aa_mapm, "0.5");

m_apm_set_long(ee_mapm, 6L);
m_apm_divide(dd_mapm, (numdigits + 4), MM_PI, ee_mapm);
m_apm_sin(bb_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"SIN(PI / 6) = [%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: SIN(PI / 6) == 0.5\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: SIN(PI / 6) != 0.5\n");
  }


m_apm_set_string(ee_mapm, "0.75");
m_apm_sqrt(aa_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"SQRT(0.75) = \n[%s]\n",buffer);

m_apm_set_long(ee_mapm, 6L);
m_apm_divide(dd_mapm, (numdigits + 4), MM_PI, ee_mapm);
m_apm_cos(bb_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"COS(PI / 6) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: COS(PI / 6) == SQRT(0.75)\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: COS(PI / 6) != SQRT(0.75)\n");
  }


m_apm_set_string(ee_mapm, "3");
m_apm_sqrt(dd_mapm, (numdigits + 4), ee_mapm);
m_apm_add(ee_mapm, MM_Two, dd_mapm);
m_apm_round(aa_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"2 + SQRT(3) = \n[%s]\n",buffer);

m_apm_set_long(ee_mapm, 5L);
m_apm_set_long(dd_mapm, 12L);
m_apm_multiply(cc_mapm, MM_PI, ee_mapm);
m_apm_divide(ee_mapm, (numdigits + 4), cc_mapm, dd_mapm);
m_apm_tan(bb_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"TAN(5 * PI / 12) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: TAN(5 * PI / 12) == 2 + SQRT(3)\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: TAN(5 * PI / 12) != 2 + SQRT(3)\n");
  }


m_apm_set_string(ee_mapm, "0.5");
m_apm_arcsin(aa_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"ARC-SIN(0.5) = \n[%s]\n",buffer);

m_apm_set_long(ee_mapm, 6L);
m_apm_divide(dd_mapm, (numdigits + 4), MM_PI, ee_mapm);
m_apm_round(bb_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"PI / 6 = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: ARC-SIN(0.5) == PI / 6\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: ARC-SIN(0.5) != PI / 6\n");
  }


m_apm_set_string(ee_mapm, "0.75");
m_apm_sqrt(dd_mapm, (numdigits + 4), ee_mapm);
m_apm_arccos(aa_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"ARC-COS(SQRT(0.75)) = \n[%s]\n",buffer);

m_apm_set_long(ee_mapm, 6L);
m_apm_divide(dd_mapm, (numdigits + 4), MM_PI, ee_mapm);
m_apm_round(bb_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"PI / 6 = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: ARC-COS(SQRT(0.75)) == PI / 6\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: ARC-COS(SQRT(0.75)) != PI / 6\n");
  }


m_apm_arctan(aa_mapm, numdigits, MM_One);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"ARC-TAN(1.0) = \n[%s]\n",buffer);

m_apm_set_long(ee_mapm, 4L);
m_apm_divide(dd_mapm, (numdigits + 4), MM_PI, ee_mapm);
m_apm_round(bb_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"PI / 4 = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: ARC-TAN(1.0) == PI / 4\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: ARC-TAN(1.0) != PI / 4\n");
  }


m_apm_exp(aa_mapm, numdigits, MM_One);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"EXP(1.0) = \n[%s]\n",buffer);

m_apm_round(bb_mapm, numdigits, MM_E);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"E = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: EXP(1.0) == E\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: EXP(1.0) != E\n");
  }


m_apm_set_string(ee_mapm, "0.5");
m_apm_exp(aa_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"EXP(0.5) = \n[%s]\n",buffer);

m_apm_sqrt(bb_mapm, numdigits, MM_E);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"SQRT(E) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: EXP(0.5) == SQRT(E)\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: EXP(0.5) != SQRT(E)\n");
  }


m_apm_copy(ee_mapm, MM_One);

for (i=1; i <= 15; i++)
  {
   m_apm_multiply(dd_mapm, ee_mapm, MM_E);
   m_apm_copy(ee_mapm, dd_mapm);
  }

m_apm_round(bb_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"E ^ 15 = \n[%s]\n",buffer);

m_apm_set_string(ee_mapm, "15.0");
m_apm_exp(aa_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"EXP(15) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: EXP(15) == E ^ 15\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: EXP(15) != E ^ 15\n");
  }

m_apm_integer_pow(cc_mapm, numdigits, MM_E, 15);
m_apm_to_string(buffer, -1, cc_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"INTEGER_POW (E^15) = \n[%s]\n",buffer);

k = m_apm_compare(bb_mapm, cc_mapm);
if (k == 0)
   fprintf(stdout,"PASS: INTEGER_POW E ^15 == E ^ 15\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: INTEGER_POW E ^15 != E ^ 15\n");
  }


m_apm_copy(ee_mapm, MM_One);

for (i=1; i <= 26; i++)
  {
   m_apm_multiply(dd_mapm, ee_mapm, MM_PI);
   m_apm_copy(ee_mapm, dd_mapm);
  }

m_apm_divide(dd_mapm, (numdigits + 8), MM_One, ee_mapm);
m_apm_round(bb_mapm, numdigits, dd_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"1 / (PI ^ 26) = \n[%s]\n",buffer);

m_apm_integer_pow(cc_mapm, numdigits, MM_PI, -26);
m_apm_to_string(buffer, -1, cc_mapm);

fprintf(stdout,"INTEGER_POW (PI ^ -26) = \n[%s]\n",buffer);

k = m_apm_compare(bb_mapm, cc_mapm);
if (k == 0)
   fprintf(stdout,"PASS: INTEGER_POW PI ^ -26 == PI ^ -26\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: INTEGER_POW PI ^ -26 != PI ^ -26\n");
  }


m_apm_multiply(ee_mapm, MM_E, MM_E);
m_apm_round(bb_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"E ^ 2 = \n[%s]\n",buffer);

m_apm_exp(aa_mapm, numdigits, MM_Two);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"EXP(2) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: EXP(2) == E ^ 2\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: EXP(2) != E ^ 2\n");
  }


m_apm_copy(ee_mapm, MM_One);

for (i=1; i <= 13; i++)
  {
   m_apm_multiply(dd_mapm, ee_mapm, MM_E);
   m_apm_copy(ee_mapm, dd_mapm);
  }

m_apm_sqrt(dd_mapm, (numdigits + 4), ee_mapm);
m_apm_divide(ee_mapm, (numdigits + 4), MM_One, dd_mapm);
m_apm_round(bb_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"1 / SQRT(E ^ 13) = \n[%s]\n",buffer);

m_apm_set_string(ee_mapm, "-6.5");
m_apm_exp(aa_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"EXP(-6.5) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: EXP(-6.5) == 1 / SQRT(E ^ 13)\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: EXP(-6.5) != 1 / SQRT(E ^ 13)\n");
  }


m_apm_copy(ee_mapm, MM_One);

for (i=1; i <= 85; i++)
  {
   m_apm_multiply(dd_mapm, ee_mapm, MM_E);
   m_apm_round(ee_mapm, (numdigits + 4), dd_mapm);
  }

m_apm_sqrt(bb_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"SQRT(E ^ 85) = \n[%s]\n",buffer);

m_apm_set_string(ee_mapm, "42.5");
m_apm_exp(aa_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"EXP(42.5) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: EXP(42.5) == SQRT(E ^ 85)\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: EXP(42.5) != SQRT(E ^ 85)\n");
  }


m_apm_round(aa_mapm, numdigits, MM_LOG_10_BASE_E);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"LOG_10_BASE_E = \n[%s]\n",buffer);

m_apm_log(bb_mapm, numdigits, MM_Ten);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"LOG(10) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: LOG(10) == LOG_10_BASE_E\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: LOG(10) != LOG_10_BASE_E\n");
  }


m_apm_round(aa_mapm, numdigits, MM_LOG_E_BASE_10);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"LOG_E_BASE_10 = \n[%s]\n",buffer);

m_apm_log10(bb_mapm, numdigits, MM_E);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"LOG10(E) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: LOG10(E) == LOG_E_BASE_10\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: LOG10(E) != LOG_E_BASE_10\n");
  }


m_apm_round(aa_mapm, numdigits, MM_LOG_2_BASE_E);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"LOG_2_BASE_E = \n[%s]\n",buffer);

m_apm_log(bb_mapm, numdigits, MM_Two);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"LOG(2) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: LOG(2) == LOG_2_BASE_E\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: LOG(2) != LOG_2_BASE_E\n");
  }


m_apm_round(aa_mapm, numdigits, MM_LOG_3_BASE_E);
m_apm_to_string(buffer, -1, aa_mapm);

fprintf(stdout,"\n");
fprintf(stdout,"LOG_3_BASE_E = \n[%s]\n",buffer);

m_apm_set_long(ee_mapm, 3L);
m_apm_log(bb_mapm, numdigits, ee_mapm);
m_apm_to_string(buffer, -1, bb_mapm);

fprintf(stdout,"LOG(3) = \n[%s]\n",buffer);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: LOG(3) == LOG_3_BASE_E\n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: LOG(3) != LOG_3_BASE_E\n");
  }


/* check FACTORIAL functions */

fprintf(stdout,"\n");

m_apm_set_long(ee_mapm, 37L);
factorial_local(aa_mapm, ee_mapm);
m_apm_factorial(bb_mapm, ee_mapm);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: FACTORIAL 37! \n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: FACTORIAL 37! \n");
  }


m_apm_set_string(ee_mapm, "426");
factorial_local(aa_mapm, ee_mapm);
m_apm_factorial(bb_mapm, ee_mapm);

k = m_apm_compare(aa_mapm, bb_mapm);
if (k == 0)
   fprintf(stdout,"PASS: FACTORIAL 426! \n");
else
  {
   total_pass = FALSE;
   fprintf(stdout,"FAIL: FACTORIAL 426! \n");
  }


if (total_pass)
  fprintf(stdout,"\nvalidate : PASS\n\n");
else
  fprintf(stdout,"\nvalidate : FAIL\n\n");

m_apm_free(aa_mapm);
m_apm_free(bb_mapm);
m_apm_free(cc_mapm);
m_apm_free(dd_mapm);
m_apm_free(ee_mapm);

free(num1);
free(num2);
free(answer);

exit(0);
}

/****************************************************************************/

/*
 *      in case some compilers dont have a native cube root or
 *      inverse hyperbolic functions, we'll just make our own.
 *
 *      note that we are not doing any decent error checking on
 *      the inputs, these functions are here just to support 
 *      this program.
 */

double  cbrt_local(x)
double  x;
{
if (x == 0.0)
  return(0.0);
else
  {
   if (x < 0.0)
     return(-pow(-x, 0.333333333333333333));
   else
     return(pow(x, 0.333333333333333333));
  }
}

/****************************************************************************/

double  asinh_local(x)
double  x;
{
return(log(x + sqrt(x * x + 1.0)));
}

/****************************************************************************/

double  acosh_local(x)
double  x;
{
if (x >= 1.0)
  return(log(x + sqrt(x * x - 1.0)));
else
  return(0.0);
}

/****************************************************************************/

double  atanh_local(x)
double  x;
{
if (fabs(x) < 1.0)
  return(0.5 * log((1.0 + x) / (1.0 - x)));
else
  return(0.0);
}

/****************************************************************************/
/*
 *      old/slow version testing
 */
void	factorial_local(result,ainput)
M_APM	result, ainput;
{
M_APM	ftmp1, ftmp2, ftmp3;

if (m_apm_compare(ainput, MM_One) <= 0)
  {
   m_apm_copy(result, MM_One);
   return;
  }

ftmp1 = m_apm_init();
ftmp2 = m_apm_init();
ftmp3 = m_apm_init();

m_apm_copy(result, ainput);
m_apm_copy(ftmp1, ainput);

while (TRUE)
  {
   m_apm_subtract(ftmp2, ftmp1, MM_One);
   m_apm_multiply(ftmp3, ftmp2, result);

   if (m_apm_compare(ftmp2, MM_Two) <= 0)
     {
      m_apm_copy(result, ftmp3);
      break;
     }

   m_apm_subtract(ftmp1, ftmp2, MM_One);
   m_apm_multiply(result, ftmp3, ftmp1);

   if (m_apm_compare(ftmp1, MM_Two) <= 0)
     break;
  }

m_apm_free(ftmp3);
m_apm_free(ftmp2);
m_apm_free(ftmp1);
}
/****************************************************************************/
