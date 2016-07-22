#pragma once

#include <credentialprovider.h>
#include <Shlwapi.h>

#include <initguid.h>

// {0C71AF6D-89C4-4B2A-8CAE-B4BECA45DED2}
//IMPLEMENT_OLECREATE(<<class>>, <<external_name>>, 
DEFINE_GUID(CLSID_SSCredProvider, 0x0c71af6d, 0x89c4, 0x4b2a, 0x8c, 0xae, 0xb4, 0xbe, 0xca, 0x45, 0xde, 0xd2);

VOID SSWACredProvAddRef();
VOID SSWACredProvRelease();
