#pragma once
#include "Exception/Exception.h"
#include "Exception/_exDefault.h"
#include "DxgiInfoManager.h"

/* GRAPHICS EXCEPTION CLASSES
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
This header contains the graphics exception classes to be used when
making DXGI API calls, some macros are alos defined for convenience.

WHen on debug the DXGI Info Manager will be used.to get access to the 
error messages. Two classes are defined, the HrException, the macro 
takes in an HRESULT and if non-zero throws an exception.

And the device removed exception for specific cases where that can happen.
-------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------------------------------------
 Graphics Exception Macros
-------------------------------------------------------------------------------------------------------
*/

// Exception and throw test macros for both classes.
// In debug they use the info manager to get the error messages.
#ifdef _DEBUG
#define GFX_EXCEPT(hr)					HrException( __LINE__,__FILE__,(long)(hr), DxgiInfoManager::GetMessages() )
#define GFX_THROW_INFO(hrcall)			{ HRESULT hr; DxgiInfoManager::Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr); }
#define GFX_THROW_INFO_ONLY(call)		{ DxgiInfoManager::Set(); (call); { auto v = DxgiInfoManager::GetMessages(); if(v) throw INFO_EXCEPT(v); } }
#define GFX_DEVICE_REMOVED_EXCEPT(hr)	DeviceRemovedException( __LINE__,__FILE__,(hr),DxgiInfoManager::GetMessages() )
#else
#define GFX_EXCEPT(hr)					HrException( __LINE__,__FILE__,(long)(hr) )
#define GFX_THROW_INFO(hrcall)			{ HRESULT hr; if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr); }
#define GFX_THROW_INFO_ONLY(call)		(call);
#define GFX_DEVICE_REMOVED_EXCEPT(hr)	DeviceRemovedException( __LINE__,__FILE__,(hr) )
#endif

/*
-------------------------------------------------------------------------------------------------------
 Graphics Exception Classes
-------------------------------------------------------------------------------------------------------
*/

// HRESULT Exception class, stores the given HRESULT and an optional list of messages, when
// what() is called it retrieves the HRESULT information and adds the messages if they exist.
class HrException : public Exception
{
public:
	// Constructor, takes in a FAILED HRESULT and an optional list of
	// messages and stores it in memory for future what() call.
	HrException(int line, const char* file, long hr, const char** infoMsgs = nullptr) noexcept;

	// HResult Exception type override.
	const char* GetType() const noexcept override { return "Graphics HResult Exception"; }

private:
	// Stores the FAILED HRESULT
	long hr;
};

// Wrapper of an HRESULT Exception that has the same constructor but get the type
// Device Removed. To be called with HRESULT of pDevice->GetDeviceRemovedReason().
class DeviceRemovedException : public HrException
{
	// Using same constructor as HRESULT Exception.
	using HrException::HrException;
public:
	// [Device Removed] Exception type override.
	const char* GetType() const noexcept override { return "Graphics [Device Removed] Exception (DXGI_ERROR_DEVICE_REMOVED)"; }
};
