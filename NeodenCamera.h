#pragma once

#pragma once

__declspec (dllexport) int _cdecl img_init();

__declspec (dllexport) BOOL _cdecl img_capture(int which_camera);

__declspec (dllexport) int _cdecl img_reset(int which_camera);

__declspec (dllexport) BOOL _cdecl img_led(int which_camera, short mode);

__declspec (dllexport) int _cdecl img_read(int which_camera, unsigned char* pFrameBuffer, int BytesToRead, int ms);

__declspec (dllexport) int _cdecl img_readAsy(int which_camera, unsigned char* pFrameBuffer, int BytesToRead, int ms);

__declspec (dllexport) BOOL _cdecl img_set_exp(int which_camera, int16_t exposure);

__declspec (dllexport) BOOL _cdecl img_set_gain(int which_camera, int16_t gain);

__declspec (dllexport) BOOL _cdecl img_set_lt(int which_camera, int16_t a2, int16_t a3);

__declspec (dllexport) BOOL _cdecl img_set_wh(int which_camera, int16_t w, int16_t h);