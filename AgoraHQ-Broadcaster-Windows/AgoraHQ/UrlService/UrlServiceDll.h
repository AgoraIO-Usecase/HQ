#pragma once

/******************************************************************************/

#ifndef URLSERVICE_DLL
#define     URLSERVICE_API			__declspec( dllimport )
#define     URLSERVICE_CLASS		__declspec( dllimport )
#else
#define     URLSERVICE_API			__declspec( dllexport )
#define     URLSERVICE_CLASS		__declspec( dllexport )
#endif

/******************************************************************************/