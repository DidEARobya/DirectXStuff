#pragma once

//Exception Helper Macros
#define GRAPHICS_THROW_FAILED(hrcall) if(FAILED(hResult = (hrcall))) throw Graphics::GraphicsException(__LINE__, __FILE__, hResult)

#if defined(_DEBUG)
#define GRAPHICS_EXCEPT(hResult) Graphics::GraphicsException( __LINE__,__FILE__,(hResult),_infoManager.GetMessages() )
#define GRAPHICS_THROW_INFO(hrcall) _infoManager.Set(); if( FAILED( hResult = (hrcall))) throw GRAPHICS_EXCEPT(hResult)
#define GRAPHICS_DEVICE_REMOVED_EXCEPT(hResult) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hResult),_infoManager.GetMessages() )
#define GRAPHICS_THROW_INFO_ONLY(call) _infoManager.Set(); (call); {auto messages = _infoManager.GetMessages(); if(!messages.empty()) {throw Graphics::InfoException(__LINE__,__FILE__, messages);}}
#else
#define GRAPHICS_EXCEPT(hResult) Graphics::GraphicsException( __LINE__,__FILE__,(hResult))
#define GRAPHICS_THROW_INFO(hrcall) GRAPHICS_THROW_NOINFO(hrcall)
#define GRAPHICS_DEVICE_REMOVED_EXCEPT(hResult) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hResult) )
#define GRAPHICS_THROW_INFO_ONLY(call) (call)
#endif

//Returns info about the Graphics into a local scope
#if defined(_DEBUG)
#define GRAPHICSINFO(graphics) HRESULT hResult; DXGIInfoManager& _infoManager =  GetInfoManager((graphics))
#else
#define GRAPHICSINFO(graphics) HRESULT hResult
#endif