#include "metaiv.h"
#include "tb_wstring.h"
#include <string>
using namespace std;

bool ToHexquad( istream & ifs, ostream & ofs )
{
  bool res = true;
  while ( !ifs.eof() )
  {
    string line;
    getline(ifs,line);

    line = TBWSTR::convertCrToNl( line );
    line += '\n';

    try
    {
      string converted (TBWSTR::ConvertToHexquad(TBWSTR::mbstr2wstring(line)));
      int len_converted = converted.length();
      string res;
      for (int idx = 0; idx < len_converted; idx++)
      {
        if (converted[idx] == '\\')
          res += '\\';
        res += converted[idx];
      }
      ofs.write( res.c_str(), res.length() );
    }
    catch (...)
    {
      res = false;
      break;
    }
  }
  return res;
}

int main (int argc, const char * argv[])
{
  switch (argc) {
    case 1: {
      ToHexquad(cin, cout);
      cout << flush;
      break;
    }
    case 2: {
      string ifilenm (argv[1]);  
      ifstream ifs (ifilenm.c_str()); 
      ToHexquad(ifs, cout);
      cout << flush;
      ifs.close();
      break;
    }
    case 3: {
      string ifilenm (argv[1]);  
      ifstream ifs (ifilenm.c_str()); 
      string ofilenm (argv[2]);  
      ofstream ofs (ofilenm.c_str()); 
      ToHexquad(ifs, ofs);
      ofs << flush;
      ifs.close();
      ofs.close();
      break;
    }
    default: {
      break;
    }
  }
  return 0;
}

