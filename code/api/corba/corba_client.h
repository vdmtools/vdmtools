//
// This include file is to be used only with the CORBA implementation
// omniORB 4.1.6 which is available at http://omniorb.sourceforge.net/
//

#ifndef __corba_client_h__
#define __corba_client_h__

#include "corba_api.hh"
#include "metaiv_idl.hh"
#include "metaiv.h"

#define VDM_IOR_FILENAME "vdmref.ior"
#define VPP_IOR_FILENAME "vppref.ior"

enum GetAppReturnCode {
  VDM_OBJECT_LOCATION_NOT_SET=0, // The environment variable was not set
  OBJECT_STRING_NON_EXISTING,  // The VDM Toolbox was not running
  CORBA_SUCCESS,
  CORBA_ERROR
};

class VDMCorbaClient
{
public:
  VDMCorbaClient() { this->_toolType = ToolboxAPI::SL_TOOLBOX; };

  VDMCorbaClient(ToolboxAPI::ToolType toolType) { this->_toolType = toolType; }
  
  void init_corba(int argc, char *argv[]);
  // Initialises the CORBA ORB and BOA. Call this function before using
  // any other CORBA related functions.

  GetAppReturnCode get_app(ToolboxAPI::VDMApplication_out app, char *path);

  GetAppReturnCode get_app(ToolboxAPI::VDMApplication_out app, char *path, ToolboxAPI::ToolType toolType);
  // The get_app method returns an VDMApplication object that provides the full
  // functionality of the toolbox to the CORBA-client program.
  // It tries to find the Object in the COS-NameService first (if there's one
  // up and running), and if this fails, it tries to read the IOR-Reference
  // string from the object.[vdm|vpp] file located at VDM_OBJECT_LOCATION
  // Depending on the ToolType parameter (SL_TOOLBOX or PP_TOOLBOX) it tries
  // to contact the vdm or the vpp server. SL_TOOLBOX is default.

  static Generic GetCPPValue(VDM::VDMGeneric_ptr g_ptr);
  // Call this method to convert a MetaIV-IDL object reference to the
  // corresponding 'real' MetaIV C++ value.

  static VDM::VDMGeneric_ptr FromCPPValue(VDM::ClientID id, Generic g, VDM::VDMFactory_ptr fact);
  // Use this function to convert a 'real' MetaIV C++ value to a
  // VDM::Generic CORBA object that can be passed in calls to the
  // server. Notice that you must pass to this function a handle to
  // the VDMFactory as well.

  string GetIORFileName();

private:
  CORBA::Object_ptr resolveApplicationObject (CORBA::ORB_ptr _the_orb, ToolboxAPI::ToolType toolType);
  GetAppReturnCode _get_app(ToolboxAPI::VDMApplication_ptr & app, char *path, ToolboxAPI::ToolType toolType);

private:
  string iorFileName;

  CORBA::ORB_var _the_orb;   // Must be declared somewhere in the client

  ToolboxAPI::ToolType _toolType;
};

#endif // __corba_client_h__

