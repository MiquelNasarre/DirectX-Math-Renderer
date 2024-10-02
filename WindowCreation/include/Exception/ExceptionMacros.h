#pragma once

//	Window exception macros

#define CHWND_EXCEPT(hr)				Window::Exception( __LINE__,__FILE__,hr )
#define CHWND_LAST_EXCEPT()				Window::Exception( __LINE__,__FILE__,GetLastError() )
#define CHWND_NOGFX_EXCEPT()			Window::NoGfxException( __LINE__,__FILE__ )

//	Graphics exception macros

#define GFX_EXCEPT_NOINFO(hr)			Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall)		{ HRESULT hr; if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr ); }

#ifndef NDEBUG
#define GFX_EXCEPT(hr)					Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall)			{ HRESULT hr; infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr); }
#define GFX_DEVICE_REMOVED_EXCEPT(hr)	Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call)		infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(v[0]) throw Graphics::InfoException( __LINE__,__FILE__,v);else free(v);}
#else
#define GFX_EXCEPT(hr)					Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall)			GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr)	Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call)		(call)
#endif

//	Gets the info manager from the graphics

#ifdef NDEBUG
#define INFOMAN(gfx) 
#else
#define INFOMAN(gfx) DxgiInfoManager& infoManager = GetInfoManager((gfx))
#endif