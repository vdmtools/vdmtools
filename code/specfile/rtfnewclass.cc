#include "specfile.h"
#include <fstream>
using namespace std;

#ifdef VDMPP
#define ZDZA "\x0d\x0a"

void SpecFile::rtfnewclass(ofstream & o)
{
  o << "{\\rtf1\\ansi\\ansicpg932\\uc2 \\deff0\\deflang1033\\deflangfe1041";

  o <<  "{\\fonttbl";
  o <<   "{\\f0\\froman\\fcharset0\\fprq2{\\*\\panose 02020603050405020304}Times New Roman;}";
  o <<   "{\\f1\\fswiss\\fcharset0\\fprq2{\\*\\panose 020b0604020202020204}Arial;}" << ZDZA;
  o <<   "{\\f2\\fmodern\\fcharset0\\fprq1{\\*\\panose 02070309020205020404}Courier New;}";
  o <<   "{\\f6\\fmodern\\fcharset0\\fprq1{\\*\\panose 00000000000000000000}Courier{\\*\\falt Courier New};}" << ZDZA;
  o <<   "{\\f23\\froman\\fcharset128\\fprq1{\\*\\panose 02020609040205080304}";
  o <<    "\\'82\\'6c\\'82\\'72 \\'96\\'be\\'92\\'a9{\\*\\falt MS Mincho};}";
  o <<   "{\\f28\\froman\\fcharset128\\fprq1{\\*\\panose 02020609040205080304}";
  o <<    "@\\'82\\'6c\\'82\\'72 \\'96\\'be\\'92\\'a9;}" << ZDZA;
  o <<   "{\\f29\\froman\\fcharset238\\fprq2 Times New Roman CE;}";
  o <<   "{\\f30\\froman\\fcharset204\\fprq2 Times New Roman Cyr;}";
  o <<   "{\\f32\\froman\\fcharset161\\fprq2 Times New Roman Greek;}";
  o <<   "{\\f33\\froman\\fcharset162\\fprq2 Times New Roman Tur;}" << ZDZA;
  o <<   "{\\f34\\froman\\fcharset177\\fprq2 Times New Roman (Hebrew);}";
  o <<   "{\\f35\\froman\\fcharset178\\fprq2 Times New Roman (Arabic);}";
  o <<   "{\\f36\\froman\\fcharset186\\fprq2 Times New Roman Baltic;}";
  o <<   "{\\f37\\fswiss\\fcharset238\\fprq2 Arial CE;}";
  o <<   "{\\f38\\fswiss\\fcharset204\\fprq2 Arial Cyr;}" << ZDZA;
  o <<   "{\\f40\\fswiss\\fcharset161\\fprq2 Arial Greek;}";
  o <<   "{\\f41\\fswiss\\fcharset162\\fprq2 Arial Tur;}";
  o <<   "{\\f42\\fswiss\\fcharset177\\fprq2 Arial (Hebrew);}";
  o <<   "{\\f43\\fswiss\\fcharset178\\fprq2 Arial (Arabic);}";
  o <<   "{\\f44\\fswiss\\fcharset186\\fprq2 Arial Baltic;}" << ZDZA;
  o <<   "{\\f45\\fmodern\\fcharset238\\fprq1 Courier New CE;}";
  o <<   "{\\f46\\fmodern\\fcharset204\\fprq1 Courier New Cyr;}";
  o <<   "{\\f48\\fmodern\\fcharset161\\fprq1 Courier New Greek;}";
  o <<   "{\\f49\\fmodern\\fcharset162\\fprq1 Courier New Tur;}";
  o <<   "{\\f50\\fmodern\\fcharset177\\fprq1 Courier New (Hebrew);}" << ZDZA;
  o <<   "{\\f51\\fmodern\\fcharset178\\fprq1 Courier New (Arabic);}";
  o <<   "{\\f52\\fmodern\\fcharset186\\fprq1 Courier New Baltic;}";
  o <<   "{\\f215\\froman\\fcharset0\\fprq1 MS Mincho Western{\\*\\falt MS Mincho};}";
  o <<   "{\\f213\\froman\\fcharset238\\fprq1 MS Mincho CE{\\*\\falt MS Mincho};}" << ZDZA;
  o <<   "{\\f214\\froman\\fcharset204\\fprq1 MS Mincho Cyr{\\*\\falt MS Mincho};}";
  o <<   "{\\f216\\froman\\fcharset161\\fprq1 MS Mincho Greek{\\*\\falt MS Mincho};}";
  o <<   "{\\f217\\froman\\fcharset162\\fprq1 MS Mincho Tur{\\*\\falt MS Mincho};}" << ZDZA;
  o <<   "{\\f220\\froman\\fcharset186\\fprq1 MS Mincho Baltic{\\*\\falt MS Mincho};}";
  o <<   "{\\f255\\froman\\fcharset0\\fprq1 @\\'82\\'6c\\'82\\'72 \\'96\\'be\\'92\\'a9 Western;}";
  o <<   "{\\f253\\froman\\fcharset238\\fprq1 @\\'82\\'6c\\'82\\'72 \\'96\\'be\\'92\\'a9 CE;}" << ZDZA;
  o <<   "{\\f254\\froman\\fcharset204\\fprq1 @\\'82\\'6c\\'82\\'72 \\'96\\'be\\'92\\'a9 Cyr;}";
  o <<   "{\\f256\\froman\\fcharset161\\fprq1 @\\'82\\'6c\\'82\\'72 \\'96\\'be\\'92\\'a9 Greek;}";
  o <<   "{\\f257\\froman\\fcharset162\\fprq1 @\\'82\\'6c\\'82\\'72 \\'96\\'be\\'92\\'a9 Tur;}" << ZDZA;
  o <<   "{\\f260\\froman\\fcharset186\\fprq1 @\\'82\\'6c\\'82\\'72 \\'96\\'be\\'92\\'a9 Baltic;}";
  o <<  "}";

  o <<  "{\\colortbl;\\red0\\green0\\blue0;\\red0\\green0\\blue255;\\red0\\green255\\blue255;";
  o <<  "\\red0\\green255\\blue0;\\red255\\green0\\blue255;\\red255\\green0\\blue0;\\red255\\green255\\blue0;" << ZDZA;
  o <<  "\\red255\\green255\\blue255;\\red0\\green0\\blue128;\\red0\\green128\\blue128;";
  o <<  "\\red0\\green128\\blue0;\\red128\\green0\\blue128;\\red128\\green0\\blue0;";
  o <<  "\\red128\\green128\\blue0;\\red128\\green128\\blue128;\\red192\\green192\\blue192;}";

  o <<  "{";
  o <<   "\\stylesheet{" << ZDZA;
  o <<    "\\ql \\li0\\ri0\\widctlpar\\faauto\\adjustright\\rin0\\lin0\\itap0 \\fs20";
  o <<    "\\lang1033\\langfe1041\\loch\\f0\\hich\\af0\\dbch\\af23\\cgrid\\langnp1033";
  o <<    "\\langfenp1041 \\snext0 Normal;}";
  o <<   "{\\s1\\ql \\li0\\ri0\\sb240\\sa60\\keepn";
  o <<    "\\widctlpar\\faauto\\outlinelevel0\\adjustright\\rin0\\lin0\\itap0 " << ZDZA;
  o <<    "\\b\\fs28\\lang1033\\langfe1041\\kerning28\\loch\\f1\\hich\\af1\\dbch";
  o <<    "\\af23\\cgrid\\langnp1033\\langfenp1041 \\sbasedon0 \\snext0 heading 1;}";
  o <<   "{\\*\\cs10 \\additive Default Paragraph Font;}";
  o <<   "{\\s15\\ql \\li0\\ri0\\widctlpar\\faauto\\adjustright\\rin0\\lin0\\itap0 \\shading2000 " << ZDZA;
  o <<    "\\fs20\\lang1024\\langfe1024\\loch\\f2\\hich\\af2\\dbch\\af23\\cgrid";
  o <<    "\\noproof\\langnp1033\\langfenp1041 \\snext15 VDM;}";
  o <<   "{\\*\\cs16 \\additive \\f6\\chshdng2000\\chcfpat0\\chcbpat0 \\sbasedon10 VDM_COV;}";
  o <<   "{\\*\\cs17 \\additive \\f6\\cf6\\chshdng2000\\chcfpat0\\chcbpat0 \\sbasedon10 " << ZDZA;
  o <<     "VDM_NCOV;}";
  o <<   "{\\s18\\ql \\li0\\ri0\\widctlpar\\faauto\\adjustright\\rin0";
  o <<    "\\lin0\\itap0 \\shading2000 \\fs20\\lang1024\\langfe1024\\loch\\f2\\hich\\af2\\dbch\\af23";
  o <<    "\\cgrid\\noproof\\langnp1033\\langfenp1041 \\sbasedon15 \\snext18 VDM_TC_TABLE;}";
  o <<  "}";

  o <<  "{\\info" << ZDZA;
  o <<   "{\\title Default UML-VDM++ Link forward engineering file}";
  o <<   "{\\author Paul Mukherjee}";
  o <<   "{\\operator  drk}";
  o <<   "{\\creatim\\yr2001\\mo1\\dy8\\hr9\\min20}";
  o <<   "{\\revtim\\yr2006\\mo4\\dy26\\hr13\\min54}";
  o <<   "{\\version2}";
  o <<   "{\\edmins1}";
  o <<   "{\\nofpages1}";
  o <<   "{\\nofwords0}";
  o <<   "{\\nofchars0}";
  o <<   "{\\*\\company VDMTOOLS}";
  o <<   "{\\nofcharsws0}" << ZDZA;
  o <<   "{\\vern8243}";
  o <<  "}";

  o <<  "\\paperw12240\\paperh15840\\margl1701\\margr1701\\margt1985\\margb1701";
  o <<  "\\gutter0 \\ftnbj\\aenddoc\\hyphcaps0\\formshade\\horzdoc\\dghspace120\\dgvspace120";
  o <<  "\\dghorigin1701\\dgvorigin1984\\dghshow0\\dgvshow3\\jcompress\\viewkind4\\viewscale100";
  o <<  "\\pgbrdrhead\\pgbrdrfoot " << ZDZA;
  o <<  "\\fet0{\\*\\template VDM.dot}\\sectd \\linex0\\headery709\\footery709";
  o <<  "\\colsx709\\endnhere\\sectdefaultcl {\\*\\pnseclvl1\\pnucrm\\pnstart1";
  o <<  "\\pnindent720\\pnhang{\\pntxta \\dbch .}}";
  o << "{\\*\\pnseclvl2" << ZDZA;
  o <<  "\\pnucltr\\pnstart1\\pnindent720\\pnhang{\\pntxta \\dbch .}}";
  o << "{\\*\\pnseclvl3\\pndec\\pnstart1\\pnindent720\\pnhang{\\pntxta \\dbch .}}";
  o << "{\\*\\pnseclvl4\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxta \\dbch )}}";
  o << "{\\*\\pnseclvl5\\pndec\\pnstart1\\pnindent720\\pnhang{\\pntxtb \\dbch (}" << ZDZA;
  o <<  "{\\pntxta \\dbch )}}";
  o << "{\\*\\pnseclvl6\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb \\dbch (}";
  o <<  "{\\pntxta \\dbch )}}";
  o << "{\\*\\pnseclvl7\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb \\dbch (}";
  o <<  "{\\pntxta \\dbch )}}";
  o << "{\\*\\pnseclvl8\\pnlcltr\\pnstart1\\pnindent720\\pnhang{\\pntxtb \\dbch (}" << ZDZA;
  o <<  "{\\pntxta \\dbch )}}";
  o << "{\\*\\pnseclvl9\\pnlcrm\\pnstart1\\pnindent720\\pnhang{\\pntxtb \\dbch (}";
  o <<  "{\\pntxta \\dbch )}}";
  o << "\\pard\\plain \\s1\\ql \\li0\\ri0\\sb240\\sa60\\keepn";
  o << "\\widctlpar\\faauto\\outlinelevel0\\adjustright\\rin0\\lin0\\itap0 " << ZDZA;
  o << "\\b\\fs28\\lang1033\\langfe1041\\kerning28\\loch\\af1\\hich\\af1\\dbch\\af23";
  o << "\\cgrid\\langnp1033\\langfenp1041 {\\lang2057\\langfe1041\\langnp2057 \\hich";
  o <<  "\\af1\\dbch\\af23\\loch\\f1 Default UML-VDM++ Link forward engineering file" << ZDZA;
  o <<  "\\par }";
  o << "\\pard\\plain \\ql \\li0\\ri0\\widctlpar\\faauto\\adjustright";
  o << "\\rin0\\lin0\\itap0 \\fs20\\lang1033\\langfe1041\\loch\\af0\\hich\\af0";
  o << "\\dbch\\af23\\cgrid\\langnp1033\\langfenp1041 {" << ZDZA;
  o <<  "\\par }";
  o << "\\pard\\plain \\s15\\ql \\li0\\ri0\\widctlpar\\faauto\\adjustright";
  o << "\\rin0\\lin0\\itap0 \\shading2000 \\fs20\\lang1024\\langfe1024\\loch\\af2\\hich\\af2";
  o << "\\dbch\\af23\\cgrid\\noproof\\langnp1033\\langfenp1041 {\\hich\\af2\\dbch\\af23\\loch\\f2 " << ZDZA;
  o << "-- New VDM++ classes created by the UML-VDM++ Link}";
  o << "{" << ZDZA;
  o <<  "\\par }";
  o << "\\pard\\plain \\ql \\li0\\ri0\\widctlpar\\faauto\\adjustright\\rin0\\lin0";
  o <<  "\\itap0 \\fs20\\lang1033\\langfe1041\\loch\\af0\\hich\\af0";
  o <<  "\\dbch\\af23\\cgrid\\langnp1033\\langfenp1041 {" << ZDZA;
  o <<  "\\par }";
  o << "{\\lang1030\\langfe1041\\langnp1030 " << ZDZA;
  o <<  "\\par }";
  o << "}" << ZDZA;
}
#endif // VDMPP

