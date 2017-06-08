#include <math.h>
#include <string>
#include <fstream>
#include "mex.h"
#include "metaiv.h"
#include "corba_client.h"
#include <winuser.h>
#include <direct.h>

// Input arguments
#define INPUT prhs[0]

// Output arguments
#define OUTPUT plhs[0]

#define BUFSIZE   700
#define POINTSIZE 50

// The handles to the ORB and BOA are declared in corba_client.cc
// Declared as externals here if you need to fiddle directly with the
// ORB or BOA.
extern CORBA::ORB_var _the_orb;
extern CORBA::BOA_var _the_boa;

// The main handle to the VDM Toolbox:
VDMApplication_var app;
VDMInterpreter_var interp;
bool IsInitialised = false;
bool IsRead = false;

// The dimension of the return matrix
const int output_m = 4;
const int output_n = 6;


void createMatrix (double input[], long m, VDM::Sequence_var &output) {
  string strbuf;
  int i, j;
  char r[POINTSIZE];

  strbuf = "WorkSpace`RunControlCycle ([";
  for (i = 0; i < m-1; i++) {
    sprintf (r, "%d, ", (int)input[i]);
    strbuf += r;
  }
  sprintf (r, "%d])", (int)input[m-1]);
  strbuf += r;

  output = VDM::Sequence::_narrow(interp->EvalExpression (strbuf.c_str()));
}



void mexFunction ( int nlhs, mxArray *plhs[],
                   int nrhs, mxArray *prhs[] ) {
  double *input_p;
  double *output_p;
  VDM::Sequence_var output;

  if (!IsInitialised) {
    init_corba (0, NULL);
    if(!get_app(app, 0, NULL)){
      mexErrMsgTxt ("rst2groove could not find a VDM handle.");
    }
    IsInitialised = true;
  }

  try {
    if (!IsRead) {
      VDMProject_var prj = app->GetProject ();
      char p[1024], *res;
      string project;
      if ((res = getcwd(p, sizeof(p))) != NULL) {
        project = p;
        project += "\\safer.prj";
        prj->New();
        prj->Open (project.c_str());
        interp = app->GetInterpreter ();
        interp->Verbose (true);
        interp->Debug (true);
        interp->Init ();
        IsRead = true;
      } else
        mexErrMsgTxt ("safer.prj project could not be opened.");
    }

    if (nrhs != 1)
      mexErrMsgTxt ("saferinterf requires 1 input argument.");
    if (nlhs != 1)
      mexErrMsgTxt ("saferinterf requires 1 output argument.");

    int m_input = mxGetM (INPUT);
    int n_input = mxGetN (INPUT);
    if (m_input != 1)
      mexErrMsgTxt ("all inputs must be scalars.");
    if (n_input != 9)
      mexErrMsgTxt ("number of inputs must be 9.");

    input_p = mxGetPr (INPUT);
  
    createMatrix (input_p, n_input, output);

    OUTPUT = mxCreateDoubleMatrix (output_m, output_n, mxREAL);
    output_p = mxGetPr (OUTPUT);

    for (int j = 0; j < output_m; j++) {
      VDM::Sequence_var s = VDM::Sequence::_narrow (output->Index (j+1));
      for (int i = 0; i < output_n; i++) {
        VDM::Bool_var b = VDM::Bool::_narrow(s->Index(i+1));
        if (b->GetValue())
          output_p[i*output_m + j] = 1;
        else
          output_p[i*output_m + j] = 0;
        b->destroy ();
      }
      s->destroy ();
    }

    output->destroy ();
  }

  catch (APIError &ex) {
    MessageBox(NULL, (char*)ex.msg, "APIError", MB_OK|MB_ICONSTOP);
    OUTPUT = mxCreateDoubleMatrix (output_m, output_n, mxREAL);
    output_p = mxGetPr (OUTPUT);
    for (int j = 0; j < output_m; j++) {
      for (int i = 0; i < output_n; i++) {
        output_p[i*output_m + j] = 0;
      }
    }
  }
}