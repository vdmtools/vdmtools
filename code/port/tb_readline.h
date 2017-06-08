#ifndef __tb_readline_h__
#define __tb_readline_h__

#include <string>
#include <iostream>

class TBReadLine
{
public:
  static std::string ReadLine(std::wostream & wos);

private:
  static int history_no;
};
#endif // __tb_readline_h__
