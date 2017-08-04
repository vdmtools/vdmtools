/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/localeutil.cc,v $
*  * VERSION
*  *    $Revision: 1.7 $
*  * DATE
*  *    $Date: 2006/04/20 07:38:36 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "localeutil.h"

#if QT_VERSION < 0x040000
#include <qtextcodec.h>
#endif // QT_VERSION < 0x040000

#include "interface2TB.h"

#define MENU_Latin9 "West European (ISO 8859-15)"
#define MENU_UTF8   "Unicode (UTF-8)"
#define MENU_JIS    "Japanese (JIS)"
#define MENU_SJIS   "Japanese (Shift-JIS)"
#define MENU_EUCJP  "Japanese (EUC)"
#define MENU_BIG5   "Chinese Traditional (Big5)"
#define MENU_GBK    "Chinese Simplified (GBK)"
#define MENU_EUCKR  "Korean (EUC)"
#define MENU_KOI8R  "Russian (KOI8-R)"
#define MENU_HEBREW "Hebrew (ISO 8859-8)"
#define MENU_TAMIL  "Tamil (ISCII)"
#define MENU_END    ""

#define Q3CODEC_Latin9 "ISO 8859-15"
#define Q3CODEC_UTF8   "UTF-8"
#define Q3CODEC_SJIS   "SJIS"
#define Q3CODEC_EUCJP  "eucJP"
#define Q3CODEC_BIG5   "Big5"
#define Q3CODEC_GBK    "GBK"
#define Q3CODEC_EUCKR  "eucKR"
#define Q3CODEC_KOI8R  "KOI8-R"
#define Q3CODEC_HEBREW "ISO 8859-8"
#define Q3CODEC_TAMIL  "TSCII"

static const char* menuItems[] = {MENU_Latin9,
                                  MENU_SJIS,
#ifndef _MSC_VER
                                  MENU_EUCJP,
#endif // _MSC_VER
//                                  MENU_JIS,
                                  MENU_BIG5,
                                  MENU_GBK,
                                  MENU_EUCKR,
                                  MENU_KOI8R,
                                  MENU_HEBREW,
                                  MENU_TAMIL,
                                  MENU_UTF8,
                                  MENU_END
                                 };
                                  
QStringList menulist;
QStringList codeclist;

QString QTLOCALE::checkQCodec(const QString & name)
{
#if QT_VERSION >= 0x040000
#if QT_VERSION >= 0x040000
  QTextCodec* pCodec = QTextCodec::codecForName (name.toLatin1());
#else
  QTextCodec* pCodec = QTextCodec::codecForName (name.toAscii());
#endif // QT_VERSION >= 0x040000
#else
  QTextCodec* pCodec = QTextCodec::codecForName (name);
#endif // QT_VERSION >= 0x040000
  if( pCodec == NULL ) return "";

  return pCodec->name(); 
}

QStringList QTLOCALE::getMenuList()
{
  if( menulist.size() > 0 )
    return menulist;
  
  int index = 0;
  while(true)
  {
    QString menustr (menuItems[index]);
    if( menustr == "" ) break;
    menulist.append(menustr);
    index++;
  }
  return menulist;
}

QString QTLOCALE::MenuNameToCodecName(const QString & name)
{
  if ( name == MENU_Latin9 ) return Q3CODEC_Latin9;
  else if ( name == MENU_UTF8 ) return Q3CODEC_UTF8;
  else if ( name == MENU_SJIS ) return Q3CODEC_SJIS;
  else if ( name == MENU_EUCJP ) return Q3CODEC_EUCJP;
  else if ( name == MENU_BIG5 ) return Q3CODEC_BIG5;
  else if ( name == MENU_EUCKR ) return Q3CODEC_EUCKR;
  else if ( name == MENU_GBK ) return Q3CODEC_GBK;
  else if ( name == MENU_KOI8R ) return Q3CODEC_KOI8R;
  else if ( name == MENU_HEBREW ) return Q3CODEC_HEBREW;
  else if ( name == MENU_TAMIL ) return Q3CODEC_TAMIL;

  else if ( name == Q3CODEC_Latin9 ) return Q3CODEC_Latin9;
  else if ( name == Q3CODEC_UTF8 ) return Q3CODEC_UTF8;
  else if ( name == Q3CODEC_SJIS ) return Q3CODEC_SJIS;
  else if ( name == Q3CODEC_EUCJP ) return Q3CODEC_EUCJP;
  else if ( name == Q3CODEC_BIG5 ) return Q3CODEC_BIG5;
  else if ( name == Q3CODEC_EUCKR ) return Q3CODEC_EUCKR;
  else if ( name == Q3CODEC_GBK ) return Q3CODEC_GBK;
  else if ( name == Q3CODEC_KOI8R ) return Q3CODEC_KOI8R;
  else if ( name == Q3CODEC_HEBREW ) return Q3CODEC_HEBREW;
  else if ( name == Q3CODEC_TAMIL ) return Q3CODEC_TAMIL;

  else return name;
}

QString QTLOCALE::Q3CodecNameToMenuName(const QString & name)
{
  if ( name == MENU_Latin9 ) return MENU_Latin9;
  else if ( name == MENU_UTF8 ) return MENU_UTF8;
  else if ( name == MENU_SJIS ) return MENU_SJIS;
  else if ( name == MENU_EUCJP ) return MENU_EUCJP;
  else if ( name == MENU_BIG5 ) return MENU_BIG5;
  else if ( name == MENU_EUCKR ) return MENU_EUCKR;
  else if ( name == MENU_GBK ) return MENU_GBK;
  else if ( name == MENU_KOI8R ) return MENU_KOI8R;
  else if ( name == MENU_HEBREW ) return MENU_HEBREW;
  else if ( name == MENU_TAMIL ) return MENU_TAMIL;

  if ( name == Q3CODEC_Latin9 ) return MENU_Latin9;
  else if ( name == Q3CODEC_UTF8 ) return MENU_UTF8;
  else if ( name == Q3CODEC_SJIS ) return MENU_SJIS;
  else if ( name == Q3CODEC_EUCJP ) return MENU_EUCJP;
  else if ( name == Q3CODEC_BIG5 ) return MENU_BIG5;
  else if ( name == Q3CODEC_EUCKR ) return MENU_EUCKR;
  else if ( name == Q3CODEC_GBK ) return MENU_GBK;
  else if ( name == Q3CODEC_KOI8R ) return MENU_KOI8R;
  else if ( name == Q3CODEC_HEBREW ) return MENU_HEBREW;
  else if ( name == Q3CODEC_TAMIL ) return MENU_TAMIL;

  else return name;
}

// for old pref
QString QTLOCALE::NumberToMenuName(int num)
{
  switch (num)
  {
    case 0: return MENU_Latin9;
    case 1: return MENU_BIG5;
    case 2: return MENU_EUCJP;
    case 3: return MENU_EUCKR;
    case 4: return MENU_GBK;
    case 5: return MENU_JIS;
    case 6: return MENU_KOI8R;
    case 7: return MENU_HEBREW;
    case 8: return MENU_SJIS;
    case 9: return MENU_TAMIL;
    default: return MENU_UTF8;
  }
}

QString QTLOCALE::MenuNameToCharSet(const QString & name)
{
#ifdef _MSC_VER
  bool win = true;
#else
  bool win = false;
#endif // _MSC_VER

  if( name == MENU_Latin9 )
    return (win ? "28605" : "en_US.ISO-8859-15") ;
  else if( name == MENU_UTF8 )
    return (win ? "65001" : "en_US.UTF-8");
  else if( name == MENU_SJIS )
    return (win ?   "932" : "ja_JP.SJIS");
  else if( name == MENU_EUCJP )
    return (win ? "51932" : "ja_JP.eucJP");
  else if( name == MENU_BIG5 )
    return (win ?   "950" : "zh_TW.Big5");
  else if( name == MENU_EUCKR )
    return (win ? "51949" : "ko_KR.eucKR");
  else if( name == MENU_GBK )
    return (win ?   "936" : "zh_CN.GBK");
  else if( name == MENU_KOI8R )
    return (win ? "20866" : "ru_RU.KOI8-R");
  else if( name == MENU_HEBREW )
    return (win ? "28598" : "he_IL.ISO-8859-8");
  else if( name == MENU_TAMIL )
    return (win ? "57004" : "ta_IN.TSCII-0");
  else return "";
}

QString QTLOCALE::getDefaultCodec()
{
  QString lang (QTLOCALE::getLanguage());
#if defined _MSC_VER
#elif defined( __APPLE_CC__ ) // Mac OS X
#else // Linux & Solaris
#endif
  return "UTF-8";
}

QString QTLOCALE::getLanguage()
{
  return Qt2TB::getLanguageI();
/*
#if defined _MSC_VER
  int langID = GetUserDefaultLangID();
  int mainID = langID & 0xFF;
  int subID = (langID & 0xFF00) >> 8;
  QString lang;
  switch( mainID )
  {
    case LANG_AFRIKAANS:                  lang = "af_ZA"; break;
    case LANG_ALBANIAN:                   lang = "sq_AL"; break;
    case LANG_ARABIC:
    {
      switch( subID )
      {
        case SUBLANG_ARABIC_SAUDI_ARABIA: lang = "ar_SA"; break;
        case SUBLANG_ARABIC_IRAQ:         lang = "ar_IQ"; break;
        case SUBLANG_ARABIC_EGYPT:        lang = "ar_EG"; break;
        case SUBLANG_ARABIC_LIBYA:        lang = "ar_LY"; break;
        case SUBLANG_ARABIC_ALGERIA:      lang = "ar_DZ"; break;
        case SUBLANG_ARABIC_MOROCCO:      lang = "ar_MA"; break;
        case SUBLANG_ARABIC_TUNISIA:      lang = "ar_TN"; break;
        case SUBLANG_ARABIC_OMAN:         lang = "ar_OM"; break;
        case SUBLANG_ARABIC_YEMEN:        lang = "ar_YE"; break;
        case SUBLANG_ARABIC_SYRIA:        lang = "ar_SY"; break;
        case SUBLANG_ARABIC_JORDAN:       lang = "ar_JO"; break;
        case SUBLANG_ARABIC_LEBANON:      lang = "ar_LB"; break;
        case SUBLANG_ARABIC_KUWAIT:       lang = "ar_KW"; break;
        case SUBLANG_ARABIC_UAE:          lang = "ar_AE"; break;
        case SUBLANG_ARABIC_BAHRAIN:      lang = "ar_BH"; break;
        case SUBLANG_ARABIC_QATAR:        lang = "ar_QA"; break;
        default: break;
      }
      break;
    }
    case LANG_ARMENIAN:                   lang = "hy_AM"; break;
    case LANG_ASSAMESE:                   lang = "";      break;
    case LANG_AZERI:                      lang = "az_AZ"; break;
    case LANG_BASQUE:                     lang = "eu_ES"; break;
    case LANG_BELARUSIAN:                 lang = "be_BY"; break;
    case LANG_BENGALI:                    lang = "bn_IN"; break;
    case LANG_BULGARIAN:                  lang = "bg_BG"; break;
    case LANG_CATALAN:                    lang = "ca_ES"; break;
    case LANG_CHINESE:
    {
      switch( subID )
      {
        case SUBLANG_CHINESE_TRADITIONAL: lang = "zh_TW"; break;
        case SUBLANG_CHINESE_SIMPLIFIED:  lang = "zh_CN"; break;
        case SUBLANG_CHINESE_HONGKONG:    lang = "zh_HK"; break;
        case SUBLANG_CHINESE_SINGAPORE:   lang = "zh_SG"; break;
        default:                          lang = "zh_CN"; break;
      }
      break;
    }
//    case LANG_CROATIAN:                   lang = "hr_HR"; break;
    case LANG_CZECH:                      lang = "cs_CZ"; break;
    case LANG_DANISH:                     lang = "da_DK"; break;
    case LANG_DUTCH:
    {
      switch( subID )
      {
        case SUBLANG_DUTCH:               lang = "nl_NL"; break;
        case SUBLANG_DUTCH_BELGIAN:       lang = "nl_BE"; break;
        default:                          lang = "nl_NL"; break;
      }
      break;
    }
    case LANG_ENGLISH:
    {
      switch( subID )
      {
        case SUBLANG_ENGLISH_US:          lang = "en_US"; break;
        case SUBLANG_ENGLISH_UK:          lang = "en_GB"; break;
        case SUBLANG_ENGLISH_AUS:         lang = "en_AU"; break;
        case SUBLANG_ENGLISH_CAN:         lang = "en_CA"; break;
        case SUBLANG_ENGLISH_NZ:          lang = "en_NZ"; break;
        case SUBLANG_ENGLISH_EIRE:        lang = "en_US"; break;
        case SUBLANG_ENGLISH_SOUTH_AFRICA: lang = "en_ZA"; break;
        case SUBLANG_ENGLISH_JAMAICA:     lang = "en_US"; break;
        case SUBLANG_ENGLISH_CARIBBEAN:   lang = "en_US"; break;
        case SUBLANG_ENGLISH_BELIZE:      lang = "en_US"; break;
        case SUBLANG_ENGLISH_TRINIDAD:    lang = "en_US"; break;
        case SUBLANG_ENGLISH_PHILIPPINES: lang = "en_PH"; break;
        case SUBLANG_ENGLISH_ZIMBABWE:    lang = "en_ZW"; break;
        default:                          lang = "en_US"; break;
      }
      break;
    }
    case LANG_ESTONIAN:                   lang = "et_EE"; break;
    case LANG_FAEROESE:                   lang = "fo_FO"; break;
    case LANG_FARSI:                      lang = ""; break;
    case LANG_FINNISH:                    lang = "fi_FI"; break;
    case LANG_FRENCH:
    {
      switch( subID )
      {
        case SUBLANG_FRENCH:              lang = "fr_FR"; break;
        case SUBLANG_FRENCH_BELGIAN:      lang = "fr_BE"; break;
        case SUBLANG_FRENCH_CANADIAN:     lang = "fr_CA"; break;
        case SUBLANG_FRENCH_SWISS:        lang = "fr_CH"; break;
        case SUBLANG_FRENCH_LUXEMBOURG:   lang = "fr_LU"; break;
        case SUBLANG_FRENCH_MONACO:       lang = "fr_FR"; break;
        default:                          lang = "fr_FR"; break;
      }
      break;
    }
    case LANG_GEORGIAN:                   lang = "ka_GE"; break;
    case LANG_GERMAN:
    {
      switch( subID )
      {
        case SUBLANG_GERMAN:              lang = "de_DE"; break;
        case SUBLANG_GERMAN_SWISS:        lang = "de_CH"; break;
        case SUBLANG_GERMAN_AUSTRIAN:     lang = "de_AT"; break;
        case SUBLANG_GERMAN_LUXEMBOURG:   lang = "de_LU"; break;
        case SUBLANG_GERMAN_LIECHTENSTEIN: lang = "de_DE"; break;
        default:                          lang = "de_DE"; break;
      }
      break;
    }
    case LANG_GREEK:                      lang = "el_GR"; break;
    case LANG_GUJARATI:                   lang = ""; break;
    case LANG_HEBREW:                     lang = "he_IL"; break;
    case LANG_HINDI:                      lang = "hi_IN"; break;
    case LANG_HUNGARIAN:                  lang = "hu_HU"; break;
    case LANG_ICELANDIC:                  lang = "is_IS"; break;
    case LANG_INDONESIAN:                 lang = "id_ID"; break;
    case LANG_ITALIAN:
    {
      switch( subID )
      {
        case SUBLANG_ITALIAN:             lang = "it_IT"; break;
        case SUBLANG_ITALIAN_SWISS:       lang = "it_CH"; break;
        default:                          lang = "it_IT"; break;
      }
      break;
    }
    case LANG_JAPANESE:                   lang = "ja_JP"; break;
    case LANG_KANNADA:                    lang = ""; break;
    case LANG_KASHMIRI:                   lang = ""; break;
    case LANG_KAZAK:                      lang = ""; break;
    case LANG_KONKANI:                    lang = ""; break;
    case LANG_KOREAN:                     lang = "ko_KR"; break;
    case LANG_LATVIAN:                    lang = "lv_LV"; break;
    case LANG_LITHUANIAN:
    {
      switch( subID )
      {
        case SUBLANG_LITHUANIAN:          lang = "lt_LT"; break;
#if (_MSC_VER < 1400 )
        case SUBLANG_LITHUANIAN_CLASSIC:  lang = "lt_LT"; break;
#endif
        default:                          lang = "lt_LT"; break;
      }
      break;
    }
    case LANG_MACEDONIAN:                 lang = "mk_MK"; break;
    case LANG_MALAY:
    {
      switch( subID )
      {
        case SUBLANG_MALAY_BRUNEI_DARUSSALAM: lang = "ms_MY"; break;
        case SUBLANG_MALAY_MALAYSIA:      lang = "ms_MY"; break;
        default:                          lang = "ms_MY"; break;
      }
      break;
    }
    case LANG_MALAYALAM:                  lang = ""; break;
    case LANG_MANIPURI:                   lang = ""; break;
    case LANG_MARATHI:                    lang = "mr_IN"; break;
    case LANG_NEPALI:                     lang = ""; break;
    case LANG_NEUTRAL:                    lang = ""; break;
    case LANG_NORWEGIAN:
    {
      switch( subID )
      {
        case SUBLANG_NORWEGIAN_BOKMAL:    lang = "no_NO"; break;
        case SUBLANG_NORWEGIAN_NYNORSK:   lang = "nn_NO"; break;
        default:                          lang = "no_NO"; break;
      }
      break;
    }
    case LANG_ORIYA:                      lang = ""; break;
    case LANG_POLISH:                     lang = "pl_PL"; break;
    case LANG_PORTUGUESE:
    {
      switch( subID )
      {
        case SUBLANG_PORTUGUESE:          lang = "pt_PT"; break;
        case SUBLANG_PORTUGUESE_BRAZILIAN: lang = "pt_BR"; break;
        default:                          lang = "pt_PT"; break;
      }
      break;
    }
    case LANG_PUNJABI:                    lang = ""; break;
    case LANG_ROMANIAN:                   lang = "ro_RO"; break;
    case LANG_RUSSIAN:                    lang = "ru_RU"; break;
    case LANG_SANSKRIT:                   lang = ""; break;
    case LANG_SERBIAN:
    {
      switch( subID )
      {
        case SUBLANG_SERBIAN_LATIN:       lang = ""; break;
        case SUBLANG_SERBIAN_CYRILLIC:    lang = "sr_YU"; break;
//    case LANG_CROATIAN:                   lang = "hr_HR"; break;
        default:                          lang = "hr_HR"; break;
      }
      break;
    }
    case LANG_SINDHI:                     lang = ""; break;
    case LANG_SLOVAK:                     lang = "sk_SK"; break;
    case LANG_SLOVENIAN:                  lang = "sl_SL"; break;
    case LANG_SPANISH:
    {
      switch( subID )
      {
        case SUBLANG_SPANISH:             lang = "es_ES"; break;
        case SUBLANG_SPANISH_MEXICAN:     lang = "es_MX"; break;
        case SUBLANG_SPANISH_MODERN:      lang = "es_ES"; break;
        case SUBLANG_SPANISH_GUATEMALA:   lang = "es_GT"; break;
        case SUBLANG_SPANISH_COSTA_RICA:  lang = "es_CR"; break;
        case SUBLANG_SPANISH_PANAMA:      lang = "es_PA"; break;
        case SUBLANG_SPANISH_DOMINICAN_REPUBLIC: lang = "es_DO"; break;
        case SUBLANG_SPANISH_VENEZUELA:   lang = "es_VE"; break;
        case SUBLANG_SPANISH_COLOMBIA:    lang = "es_CO"; break;
        case SUBLANG_SPANISH_PERU:        lang = "es_PE"; break;
        case SUBLANG_SPANISH_ARGENTINA:   lang = "es_AR"; break;
        case SUBLANG_SPANISH_ECUADOR:     lang = "es_EC"; break;
        case SUBLANG_SPANISH_CHILE:       lang = "es_CL"; break;
        case SUBLANG_SPANISH_URUGUAY:     lang = "es_UY"; break;
        case SUBLANG_SPANISH_PARAGUAY:    lang = "es_PY"; break;
        case SUBLANG_SPANISH_BOLIVIA:     lang = "es_BO"; break;
        case SUBLANG_SPANISH_EL_SALVADOR: lang = "es_SV"; break;
        case SUBLANG_SPANISH_HONDURAS:    lang = "es_HN"; break;
        case SUBLANG_SPANISH_NICARAGUA:   lang = "es_NI"; break;
        case SUBLANG_SPANISH_PUERTO_RICO: lang = "es_PR"; break;
        default:                          lang = "es_ES"; break;
      }
      break;
    }
    case LANG_SWAHILI:                    lang = ""; break;
    case LANG_SWEDISH:
    {
      switch( subID )
      {
        case SUBLANG_SWEDISH:             lang = "sv_SE"; break;
        case SUBLANG_SWEDISH_FINLAND:     lang = "sv_FI"; break;
        default:                          lang = "sv_SE"; break;
      }
      break;
    }
    case LANG_TAMIL:                      lang = "ta_IN"; break;
    case LANG_TATAR:                      lang = "tt_RU"; break;
    case LANG_TELUGU:                     lang = "te_IN"; break;
    case LANG_THAI:                       lang = "th_TH"; break;
    case LANG_TURKISH:                    lang = "tr_TR"; break;
    case LANG_UKRAINIAN:                  lang = "uk_UA"; break;
    case LANG_URDU:
    {
      switch( subID )
      {
        case SUBLANG_URDU_INDIA:          lang = "ur_IN"; break;
        case SUBLANG_URDU_PAKISTAN:       lang = "ur_PK"; break;
        default:                          lang = "ur_PK"; break;
      }
      break;
    }
    case LANG_UZBEK:
    {
      switch( subID )
      {
        case SUBLANG_UZBEK_CYRILLIC:      lang = "uz_UZ"; break;
        case SUBLANG_UZBEK_LATIN:         lang = "uz_UZ"; break;
        default:                          lang = "uz_UZ"; break;
      }
      break;
    }
    case LANG_VIETNAMESE:                 lang = "vi_VN"; break;
    default: break;
  }
  return lang;
#elif defined( __APPLE_CC__ ) // Mac OS X
  CFLocaleRef ref = CFLocaleCopyCurrent();
  CFStringRef str = (CFStringRef)CFLocaleGetIdentifier(ref);
  QString lang = CFStringGetCStringPtr( str, kCFStringEncodingUTF8 );
  return lang;
#else // Linux & Solaris
  QString lang;
  const char * l = getenv( "LANG" );
  if( NULL == l ) return lang;
  lang = l;
#if QT_VERSION >= 0x040000
  int index = lang.indexOf( '.' ); 
#else
  int index = lang.find( '.' ); 
#endif //  QT_VERSION >= 0x040000
  if( index < 0 ) return lang;
  return lang.left( index );
#endif
*/
}

QString QTLOCALE::getAltLang( const QString& lang )
{
  if( lang.length() < 2 ) return QString( "" ); 
  QString l = lang.left(2);

  if( l == "aa" && lang != "aa_ER" ) return QString( "aa_ER" );
  else if( l == "af" && lang != "af_ZA" ) return QString( "af_ZA" );
  else if( l == "am" && lang != "am_ET" ) return QString( "am_ET" );
  else if( l == "an" && lang != "an_ES" ) return QString( "an_ES" );
  else if( l == "be" && lang != "be_BY" ) return QString( "be_BY" );
  else if( l == "bg" && lang != "bg_BG" ) return QString( "bg_BG" );
  else if( l == "bn" && lang != "bn_IN" ) return QString( "bn_IN" );
  else if( l == "br" && lang != "br_FR" ) return QString( "br_FR" );
  else if( l == "bs" && lang != "bs_BA" ) return QString( "bs_BA" );
  else if( l == "ca" && lang != "ca_ES" ) return QString( "ca_ES" );
  else if( l == "cs" && lang != "cs_CZ" ) return QString( "cs_CZ" );
  else if( l == "cy" && lang != "cy_GB" ) return QString( "cy_GB" );
  else if( l == "da" && lang != "da_DK" ) return QString( "da_DK" );
  else if( l == "de" && lang != "de_DE" ) return QString( "de_DE" );
  else if( l == "el" && lang != "el_GR" ) return QString( "el_GR" );
  else if( l == "en" && lang != "en_US" ) return QString( "en_US" );
  else if( l == "es" && lang != "es_ES" ) return QString( "es_ES" );
  else if( l == "et" && lang != "et_EE" ) return QString( "et_EE" );
  else if( l == "eu" && lang != "eu_ES" ) return QString( "eu_ES" );
  else if( l == "fa" && lang != "fa_IR" ) return QString( "fa_IR" );
  else if( l == "fi" && lang != "fi_FI" ) return QString( "fi_FI" );
  else if( l == "fo" && lang != "fo_FO" ) return QString( "fo_FO" );
  else if( l == "fr" && lang != "fr_FR" ) return QString( "fr_FR" );
  else if( l == "ga" && lang != "ga_IE" ) return QString( "ga_IE" );
  else if( l == "gd" && lang != "gd_GB" ) return QString( "gd_GB" );
  else if( l == "gl" && lang != "gl_ES" ) return QString( "gl_ES" );
  else if( l == "gu" && lang != "gu_IN" ) return QString( "gu_IN" );
  else if( l == "gv" && lang != "gv_GB" ) return QString( "gv_GB" );
  else if( l == "he" && lang != "he_IL" ) return QString( "he_IL" );
  else if( l == "hi" && lang != "hi_IN" ) return QString( "hi_IN" );
  else if( l == "hr" && lang != "hr_HR" ) return QString( "hr_HR" );
  else if( l == "hu" && lang != "hu_HU" ) return QString( "hu_HU" );
  else if( l == "hy" && lang != "hy_AM" ) return QString( "hy_AM" );
  else if( l == "id" && lang != "id_ID" ) return QString( "id_ID" );
  else if( l == "is" && lang != "is_IS" ) return QString( "is_IS" );
  else if( l == "it" && lang != "it_IT" ) return QString( "it_IT" );
  else if( l == "iw" && lang != "iw_IL" ) return QString( "iw_IL" );
  else if( l == "ja" && lang != "ja_JP" ) return QString( "ja_JP" );
  else if( l == "ka" && lang != "ka_GE" ) return QString( "ka_GE" );
  else if( l == "kk" && lang != "kk_KZ" ) return QString( "kk_KZ" );
  else if( l == "kl" && lang != "kl_GL" ) return QString( "kl_GL" );
  else if( l == "kn" && lang != "kn_IN" ) return QString( "kn_IN" );
  else if( l == "ko" && lang != "ko_KR" ) return QString( "ko_KR" );
  else if( l == "kw" && lang != "kw_GB" ) return QString( "kw_GB" );
  else if( l == "la" && lang != "la_LN" ) return QString( "la_LN" );
  else if( l == "lg" && lang != "lg_UG" ) return QString( "lg_UG" );
  else if( l == "lo" && lang != "lo_LA" ) return QString( "lo_LA" );
  else if( l == "lt" && lang != "lt_LT" ) return QString( "lt_LT" );
  else if( l == "lv" && lang != "lv_LV" ) return QString( "lv_LV" );
  else if( l == "mi" && lang != "mi_NZ" ) return QString( "mi_NZ" );
  else if( l == "mk" && lang != "mk_MK" ) return QString( "mk_MK" );
  else if( l == "ml" && lang != "ml_IN" ) return QString( "ml_IN" );
  else if( l == "mn" && lang != "mn_MN" ) return QString( "mn_MN" );
  else if( l == "mr" && lang != "mr_IN" ) return QString( "mr_IN" );
  else if( l == "ms" && lang != "ms_MY" ) return QString( "ms_MY" );
  else if( l == "mt" && lang != "mt_MT" ) return QString( "mt_MT" );
  else if( l == "nb" && lang != "nb_NO" ) return QString( "nb_NO" );
  else if( l == "ne" && lang != "ne_NP" ) return QString( "ne_NP" );
  else if( l == "nl" && lang != "nl_NL" ) return QString( "nl_NL" );
  else if( l == "nn" && lang != "nn_NO" ) return QString( "nn_NO" );
  else if( l == "no" && lang != "no_NO" ) return QString( "no_NO" );
  else if( l == "oc" && lang != "oc_FR" ) return QString( "oc_FR" );
  else if( l == "om" && lang != "om_KE" ) return QString( "om_KE" );
  else if( l == "pa" && lang != "pa_IN" ) return QString( "pa_IN" );
  else if( l == "pl" && lang != "pl_PL" ) return QString( "pl_PL" );
  else if( l == "pt" && lang != "pt_PT" ) return QString( "pt_PT" );
  else if( l == "ro" && lang != "ro_RO" ) return QString( "ro_RO" );
  else if( l == "ru" && lang != "ru_RU" ) return QString( "ru_RU" );
  else if( l == "se" && lang != "se_NO" ) return QString( "se_NO" );
  else if( l == "sh" && lang != "sh_YU" ) return QString( "sh_YU" );
  else if( l == "sk" && lang != "sk_SK" ) return QString( "sk_SK" );
  else if( l == "sl" && lang != "sl_SI" ) return QString( "sl_SI" );
  else if( l == "so" && lang != "so_SO" ) return QString( "so_SO" );
  else if( l == "sq" && lang != "sq_AL" ) return QString( "sq_AL" );
  else if( l == "sr" && lang != "sr_YU" ) return QString( "sr_YU" );
  else if( l == "st" && lang != "st_ZA" ) return QString( "st_ZA" );
  else if( l == "sv" && lang != "sv_SE" ) return QString( "sv_SE" );
  else if( l == "ta" && lang != "ta_IN" ) return QString( "ta_IN" );
  else if( l == "te" && lang != "te_IN" ) return QString( "te_IN" );
  else if( l == "tg" && lang != "tg_TJ" ) return QString( "tg_TJ" );
  else if( l == "th" && lang != "th_TH" ) return QString( "th_TH" );
  else if( l == "ti" && lang != "ti_ER" ) return QString( "ti_ER" );
  else if( l == "tl" && lang != "tl_PH" ) return QString( "tl_PH" );
  else if( l == "tr" && lang != "tr_TR" ) return QString( "tr_TR" );
  else if( l == "tt" && lang != "tt_RU" ) return QString( "tt_RU" );
  else if( l == "uk" && lang != "uk_UA" ) return QString( "uk_UA" );
  else if( l == "ur" && lang != "ur_PK" ) return QString( "ur_PK" );
  else if( l == "uz" && lang != "uz_UZ" ) return QString( "uz_UZ" );
  else if( l == "vi" && lang != "vi_VN" ) return QString( "vi_VN" );
  else if( l == "wa" && lang != "wa_BE" ) return QString( "wa_BE" );
  else if( l == "xh" && lang != "xh_ZA" ) return QString( "xh_ZA" );
  else if( l == "yi" && lang != "yi_US" ) return QString( "yi_US" );

  if( lang.length() < 3 ) return QString( "" ); 
  l = lang.left(3);
  if( l == "byn" && lang != "byn_ER" ) return QString( "byn_ER" );
  else if( l == "gez" && lang != "gez_ER" ) return QString( "gez_ER" );
  else if( l == "sid" && lang != "sid_ET" ) return QString( "sid_ET" );
  else if( l == "tig" && lang != "tig_ER" ) return QString( "tig_ER" );

  return QString( "" );
}

