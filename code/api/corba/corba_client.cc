/***
*  * WHAT
*  *    The functions declared here are to be used by clients using the 
*  *    API of the Toolbox. They are _not_ necessary in order to write a
*  *    client, but they make the writing of a client a little easier.
*  *    
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/api/corba/corba_client.cc,v $
*  * VERSION
*  *    $Revision: 1.16 $
*  * DATE
*  *    $Date: 2005/12/21 06:05:24 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *    
*  * PROJECT
*  *    
*  * AUTHOR
*  *    Ole Storm + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include <iostream>
#include <string>
#include <sstream>
//using namespace std;

#include "corba_client.h"
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define HOME_ENV_NAME "USERPROFILE"
#else
#define HOME_ENV_NAME "HOME"
#endif // _MSC_VER

void VDMCorbaClient::init_corba(int argc, char *argv[])
{
  // 
  // CORBA Initialization stuff:
  //
  //int argc = 3;
  //argv[1] = CORBA::string_dup("-ORBInitRef");
  //argv[2] = CORBA::string_dup("NameService=corbaloc:iiop:1.0@localhost:2809/NameService");

  this->_the_orb = CORBA::ORB_init(argc, argv);
}

GetAppReturnCode VDMCorbaClient::_get_app(ToolboxAPI::VDMApplication_ptr & app, char *path,
                                          ToolboxAPI::ToolType toolType)
{
  //
  // Try to resolve the Application object from the NamingService
  //
  CORBA::Object_ptr objPtr = resolveApplicationObject (this->_the_orb, toolType);

  if (!CORBA::is_nil (objPtr))
  {
    app = ToolboxAPI::VDMApplication::_narrow(objPtr);
    if(!CORBA::is_nil(app))
      return CORBA_SUCCESS;
  }
  cerr << "Application was not registered in NameService" << endl;

  //
  // Could not resolve from naming service.
  // Let's see if there is a IOR-Ref-file somewhere
  //

  this->iorFileName = (toolType == ToolboxAPI::SL_TOOLBOX ? VDM_IOR_FILENAME : VPP_IOR_FILENAME);

  string prext;
  this->iorFileName += prext;

  string ior;
  bool success = false;
  if((path != NULL) && (strlen(path) > 0))
  {
    string iorfile_s = string(path) + "/" + this->iorFileName;

    cerr << "refFile: "  << iorfile_s << endl;

    // Check if ?_IOR_FILENAME file exists:
    // Go on and read the object string
    ifstream inf;
    inf.open(iorfile_s.c_str(), ios::in);
    if(inf.good()) {
      inf >> ior;  // Read one single line
      inf.close();
      success = true;
    }
  }
  if(!success)
  {
    // The file was not in the directory pointed to by path
    const char *location = getenv("VDM_OBJECT_LOCATION");
    if( NULL == location )
    {
      const char* homeDir = getenv( HOME_ENV_NAME );
      if( NULL != homeDir ) location = homeDir;
    }

    if(NULL != location)
    {
      string iorfile_s = string(location) + "/" + this->iorFileName;

      cerr << "refFile: " << iorfile_s << endl;

      // Check if object.string file exists:
//      FILE *exists = fopen(iorfile_s.c_str(), "r");
//      if(NULL == exists) {
//        return OBJECT_STRING_NON_EXISTING;
//      }
//      fclose(exists); // Clean up...

      // Go on and read the object string
      ifstream inf;
      inf.open(iorfile_s.c_str(), ios::in);
      //if(!inf.bad())
      if(inf.good())
      {
        inf >> ior;  // Read one single line
        inf.close();
      }
      else
        return OBJECT_STRING_NON_EXISTING;
    }
    else
      return VDM_OBJECT_LOCATION_NOT_SET;
  }

  CORBA::Object_var obj = this->_the_orb->string_to_object(ior.c_str());
  app = ToolboxAPI::VDMApplication::_narrow(obj);
  if(CORBA::is_nil(app))
    return CORBA_ERROR;

  return CORBA_SUCCESS;
}

GetAppReturnCode VDMCorbaClient::get_app(ToolboxAPI::VDMApplication_out app, char *path )
{
  //return _get_app ( app._data, path, ToolboxAPI::SL_TOOLBOX );
  return _get_app ( app._data, path, this->_toolType );
}

GetAppReturnCode VDMCorbaClient::get_app(ToolboxAPI::VDMApplication_out app, char *path, ToolboxAPI::ToolType toolType )
{
  return _get_app ( app._data, path, toolType );
}

//
// Following method is used for getting the VDMApplication object
// from the COS-NameService.
// Parameter toolType indicates, which object should be chosen
// (SL_TOOLBOX/PP_TOOLBOX)
//
CORBA::Object_ptr VDMCorbaClient::resolveApplicationObject (CORBA::ORB_ptr _the_orb, ToolboxAPI::ToolType toolType)
{
  CosNaming::NamingContext_var rootContext;
  try
  {
    CORBA::Object_ptr obj;
    obj = this->_the_orb->resolve_initial_references ("NameService");

    rootContext = CosNaming::NamingContext::_narrow (obj);
    if( CORBA::is_nil(rootContext) ) {
      cerr << "Failed to narrow the root naming context." << endl;
      return CORBA::Object::_nil();
    }
  }
  catch (...)
  {
    return CORBA::Object::_nil ();
  }

  CosNaming::Name name;
  name.length (2);
      
  name[0].id = (const char*)"VDMTOOLBOX";
  name[0].kind = (const char*)"TOOL";
  if (toolType == ToolboxAPI::SL_TOOLBOX)
    name[1].id = (const char*)"SL_TOOLBOX";
  else
    name[1].id = (const char*)"PP_TOOLBOX";
  name[1].kind = (const char*)"VDMApplication";

  try
  {
    CORBA::Object_ptr res = rootContext->resolve (name);
    return res;
  }
  catch (omniORB::fatalException& e)
  {
    cerr << L"OmniORB reported a fatal exception." << endl;
    return CORBA::Object::_nil ();
  }
  catch (...) // NotFound or COMM_FAILURE?
  {
    return CORBA::Object::_nil ();
  }
  return CORBA::Object::_nil ();
}


Generic VDMCorbaClient::GetCPPValue(VDM::VDMGeneric_ptr g_ptr)
  // Call this method to convert a MetaIV-IDL object reference to the
  // corresponding 'real' MetaIV value.
{
  //  The following three lines are omniORB specific and will
  //  consequently not work with other ORBs. 

  VDM::bytes * buf = g_ptr->GetCPPValue();
//  istringstream is((char *)buf->NP_data(), buf->length());
  istringstream is((char *)buf->NP_data());
  return ReadVal(is);
}


VDM::VDMGeneric_ptr VDMCorbaClient::FromCPPValue(VDM::ClientID id, Generic g, VDM::VDMFactory_ptr fact)
  // Call this method to convert a VDM C++ value (Generic) to a
  // MetaIV-IDL object reference that can be passed through the API.
{
  VDM::bytes *bts;
  ostringstream os;
  g.WriteVal(os);
  string str = os.str();
  bts = new VDM::bytes(str.size(), str.size(), (unsigned char *)str.c_str(), false);
  VDM::VDMGeneric_ptr p = fact->FromCPPValue(id, *bts);
  delete bts;
  return p;
}

string VDMCorbaClient::GetIORFileName()
{
  return this->iorFileName;
}

