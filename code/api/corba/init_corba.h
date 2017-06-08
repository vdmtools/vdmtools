/***
*  * WHAT
*  *    Functions used to initialize and terminate the CORBA API.
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/api/corba/init_corba.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 2006/04/18 07:45:41 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    INFORMA and Toolbox
*  * AUTHOR
*  *    Ole Storm + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef _INIT_CORBA_H
#define _INIT_CORBA_H

#include "corba_api.h"
#include "corba_api.hh"
#include <string>

class VDMCORBA
{
private:
  static VDMApplication_i *toolbox_app;
  static CORBA::ORB_var orb;
  static PortableServer::POA_var poa;
  static ToolboxAPI::ToolType toolType;

public:
  static PortableServer::POA_var & GetPOAvar() { return VDMCORBA::poa; };
  static bool init_corba_api(int argc, char *argv[], std::wstring &err);
  static void terminate_corba_api();

private:
  static std::string getIorFileLocation( const ToolboxAPI::ToolType & type );
  static bool bindObjectToName (CORBA::ORB_ptr orb, CORBA::Object_ptr appObj);
  static bool unbindObjectFromName (CORBA::ORB_ptr orb);
};

#endif // _INIT_CORBA_H
