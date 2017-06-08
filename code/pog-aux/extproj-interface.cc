#include "extproj-interface.h"
#include "tools.h"
#include "projectval.h"
#include "intconvquotes.h" 
#include "SPEC_uAST.h"

static ExtProjectInterface theExtProjectInterface;

ExtProjectInterface& GetExtProjectInterface()
{
  return theExtProjectInterface;
};

void ExtProjectInterface::setup()
{
  vdm_SPEC_uAST_setup();
}

int ExtProjectInterface::getNumberOfTypeDefs()
{
  return 0; //vdm_SPEC_uAST_GetNumberOfTypeDefs().GetValue();
}

int ExtProjectInterface::getNumberOfFnDefs()
{
  return 0; // vdm_SPEC_uAST_GetNumberOfFnDefs().GetValue();
}

std::wstring  ExtProjectInterface::getTypeDefName(int index)
{
  return std::wstring(); //Seq2Str(vdm_SPEC_uAST_GetTypeDefName(Int(index)));
}

std::wstring  ExtProjectInterface::getFnDefName(int index)
{
  return std::wstring(); //Seq2Str(vdm_SPEC_uAST_GetFnDefName(Int(index)));
}











