#include "stdafx.h"
#include <windows.h>

// windows.h includes WINGDI.h which
// defines GetObject as GetObjectA, breaking
// System::Resources::ResourceManager::GetObject.
// So, we undef here.
#undef GetObject

#include "NeodenCamera.h"
#include <time.h>

using namespace std;

CCyUSBDevice* USBDevice;
HANDLE hDevice;
CCyControlEndPoint* epControl;
CCyBulkEndPoint* epBulkIn;

int					QueueSize = 16;
const int			MAX_QUEUE_SZ = 64;
//static int			PPX = 8192;
static int			PPX = 1;
static int			TimeOut = 1500;

int open_camera = 0;

void openCameraID(int id)
{
    int n = USBDevice->DeviceCount();

    for (int i = 0; i < n; i++)
    {
        if (USBDevice->IsOpen() == true)
        {
            USBDevice->Close();
            open_camera = 0;
        }
        USBDevice->Open(i);
        // Is it the Neoden?
        if (USBDevice->VendorID != 0x52cb)
        {
            continue;
        }

        switch (USBDevice->Product[0])
        {
            case 'H':
            if (id == 1)
            {
                _RPT1(_CRT_WARN, "WE FOUND CAMERA ID:%d\n", id);
                open_camera = 1;
                return;
            }
            break;
            case 'B':
            if (id == 5)
            {
                _RPT1(_CRT_WARN, "WE FOUND CAMERA ID:%d\n", id);
                open_camera = 5;
                return;
            }
            break;
        }

    }
}

void switchCamera(int camera_id)
{
    if (camera_id != 1)
        if (camera_id != 5)
        {
            _RPT1(_CRT_WARN, "switchCamera(%d) is not 1 or 5\n", camera_id);
            return;
        }

    if (open_camera != camera_id)
    {
        // we need to open the right device
        openCameraID(camera_id);
    }
    else
        return;

    epControl = USBDevice->ControlEndPt;
    epBulkIn = USBDevice->BulkInEndPt;
}



BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        // Create the CyUSBDevice
        USBDevice = new CCyUSBDevice(0, CYUSBDRV_GUID, true);
    }
    break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    // Create the CyUSBDevice
    USBDevice = new CCyUSBDevice(0, CYUSBDRV_GUID, true);

    if (img_init() == 2)
    {
    }

    delete USBDevice;
    return 0;
}

int _cdecl img_init()
{
    _RPT0(_CRT_WARN, "IMG INIT\n");

    BOOL bXferCompleted = false;
    int n = USBDevice->DeviceCount();
    long len = 0; // Each xfer request will get PPX isoc packets

    LONG bytesToSend = 0;
    LONG rLen = 0;

    for (int i = 0; i < n; i++)
    {
        if (USBDevice->IsOpen() == true)
        {
            USBDevice->Close();
            open_camera = 0;
        }
        USBDevice->Open(i);
        // Is it the Neoden?
        if (USBDevice->VendorID != 0x52cb)
        {
            continue;
        }

        switch (USBDevice->Product[0])
        {
        case 'H':
            _RPT0(_CRT_WARN, "WE FOUND CAMERA DOWN\n");
            open_camera = 1;         
            break;
        case 'B':
            _RPT0(_CRT_WARN, "WE FOUND CAMERA UP\n");
            open_camera = 5;
            break;
        }

        hDevice = USBDevice->DeviceHandle();
        epControl = USBDevice->ControlEndPt;
        _RPT1(_CRT_WARN, "epcontrol init: 0x%x\n", epControl);
        epBulkIn = USBDevice->BulkInEndPt;

        len = epControl->MaxPktSize * PPX;
        epControl->SetXferSize(len);
    }

    return n;
}


int _cdecl img_readAsy ( int which_camera, unsigned char * pFrameBuffer, int a3, DWORD dwMilliseconds, char a5 )
{
    _RPT1(_CRT_WARN, "IMG CAPTURE: %d %d %d\n", which_camera, a3, a5);

    LONG bytesToSend = 0;
    LONG rLen = 0;
    unsigned char  buf2[1];
    BOOL bXferCompleted = false;
    int retVal = 0;

    switchCamera(which_camera);

    if ( pFrameBuffer == NULL ) {
        return 0;
    }

    epControl->Target = TGT_OTHER; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->Direction = DIR_TO_DEVICE;
    epControl->ReqCode = 0xb3; // byte 1
    epControl->Value = 0x0000; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5
    epControl->Write(buf2, bytesToSend);

    epControl->Target = TGT_INTFC; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb1; // byte 1
    epControl->Value = 0x0000; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5
    epControl->Write(buf2, bytesToSend);

    // ************************************************
    bytesToSend = a3;
    rLen = bytesToSend;
    epBulkIn->TimeOut = 1000;
    bXferCompleted = epBulkIn->XferData(pFrameBuffer, rLen);
    if (bXferCompleted)
    {
        retVal = 1;
    }

    return retVal;

}

BOOL _cdecl img_set_exp(int which_camera, int16_t exposure)
{
    _RPT1(_CRT_WARN, "IMG SET EXP: %d\n", which_camera);
    unsigned char  buf2[1];
    LONG bytesToSend = 0;  // 38 + 0 = 38

    switchCamera(which_camera);

    epControl->Target = TGT_ENDPT; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb4; // byte 1
    epControl->Direction = DIR_TO_DEVICE; // 0x40
    epControl->Value = exposure; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5

    return epControl->Write(buf2, bytesToSend);
}

BOOL _cdecl img_set_gain(int which_camera, int16_t gain)
{
    _RPT1(_CRT_WARN, "IMG SET GAIN: %d\n", which_camera);
    unsigned char  buf2[1];
    LONG bytesToSend = 0;  // 38 + 0 = 38

    switchCamera(which_camera);

    epControl->Target = TGT_ENDPT; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb5; // byte 1
    epControl->Direction = DIR_TO_DEVICE; // 0x40
    epControl->Value = gain; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5

    return epControl->Write(buf2, bytesToSend);
}

BOOL _cdecl img_set_lt(int which_camera, int16_t a2, int16_t a3)
{
    _RPT1(_CRT_WARN, "IMG SET LT: %d\n", which_camera);
    unsigned char  buf2[1];
    LONG bytesToSend = 0;  // 38 + 0 = 38

    switchCamera(which_camera);

    epControl->Target = TGT_ENDPT; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb6; // byte 1
    epControl->Direction = DIR_TO_DEVICE; // 0x40

    // ************************************************
    epControl->Value = ((a2 >> 1) << 1) + 12; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5
    return epControl->Write(buf2, bytesToSend);
}

BOOL _cdecl img_set_wh(int which_camera, int16_t w, int16_t h)
{
    _RPT1(_CRT_WARN, "IMG SET WH: %d\n", which_camera);
    unsigned char  buf2[1];
    LONG bytesToSend = 0;  // 38 + 0 = 38

    switchCamera(which_camera);

    epControl->Target = TGT_DEVICE; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb7; // byte 1
    epControl->Direction = DIR_TO_DEVICE; // 0x40

    // ************************************************
    epControl->Value = h; // byte 2,3
    epControl->Index = w; // byte 4,5
    return epControl->Write(buf2, bytesToSend);
}