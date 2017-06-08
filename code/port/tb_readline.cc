#include "tb_readline.h"

#ifndef _MSC_VER
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#endif // _MSC_VER

#define MAX_HISTORY_NO 20

int TBReadLine::history_no = 0;

std::string TBReadLine::ReadLine(std::wostream & wos)
{
#ifdef _MSC_VER
#ifdef VDMSL
  wos << L"vdm> " << std::flush;
#endif // VDMSL
#ifdef VDMPP
#ifdef VICE
  wos << L"vice> " << std::flush;
#else
  wos << L"vpp> " << std::flush;
#endif // VICE
#endif // VDPPM

  std::string st;
  char ch;
  ch = std::cin.get ();
  while (ch != '\n' && ch != '\f')
  {
    if (ch == (char)EOF) {
      wos << std::endl << std::flush;
      return "cquit";
    }
    else {
      st += ch;
      ch = std::cin.get ();
    }
  }
  return st;
#else
#ifdef VDMSL
  char *line = readline("vdm> ");
#endif // VDMSL
#ifdef VDMPP
#ifdef VICE
  char *line = readline("vice> ");
#else
  char *line = readline("vpp> ");
#endif // VICE
#endif // VDPPM
  if (line == NULL)
  {
    wos << std::endl << std::flush;
    return "cquit";
  }
  else
  {
    if (strlen(line)>0)
      add_history(line);

#if defined( __darwin__ ) && (MAJOR_VERSION <= 8)
#else
    if (++history_no > MAX_HISTORY_NO) {
      HIST_ENTRY *history = remove_history(0);
      free(history);
    }
#endif

    std::string st(line);
    free(line);
    return st;
  }
#endif // _MSC_VER
}
