#pragma once

/* WIN32 / D3D11 HEADER
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This header contains necessary libraries to be loaded for most the D3D11 dependencies 
that are used across the library. 

The define list at the beggining gets rid of some chunks of the code to not bloat the 
bult time, if those dependencies are needed just comment them from the list.
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

// target Windows 7 or later
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#pragma region Some defines you might wanna use
// The following #defines disable a bunch of unused windows stuff. If you 
// get weird errors when trying to do some windows stuff, try removing some
// (or all) of these defines (it will increase build time though).
// 
//#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
//#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#ifndef STRICT
#define STRICT
#endif
#pragma endregion

#include <Windows.h>
#include <WinUser.h>
#include <d3d11.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;