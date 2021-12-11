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

CCyUSBDevice* USBDevice1;
CCyUSBDevice* USBDevice5;

static int			PPX = 1;

CCyBulkEndPoint* getBulkEp(int camera_id)
{
    if (camera_id != 1)
        if (camera_id != 5)
        {
            _RPT1(_CRT_WARN, "getControlEp(%d) is not 1 or 5\n", camera_id);
        }
    
    if (camera_id == 1)
    {
        USBDevice1->BulkInEndPt->SetXferSize(1024);
        return USBDevice1->BulkInEndPt;
    }
    if (camera_id == 5)
    {
        USBDevice5->BulkInEndPt->SetXferSize(1024);
        return USBDevice5->BulkInEndPt;
    }
    return NULL;
}


CCyControlEndPoint* getControlEp(int camera_id)
{
    if (camera_id != 1)
        if (camera_id != 5)
        {
            _RPT1(_CRT_WARN, "getControlEp(%d) is not 1 or 5\n", camera_id);
        }

    if (camera_id == 1)
        return USBDevice1->ControlEndPt;
    if (camera_id == 5)
        return USBDevice5->ControlEndPt;
    return NULL;
}

HANDLE hnd1;
HANDLE hnd5;


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        // Create the CyUSBDevices
        USBDevice1 = new CCyUSBDevice(hnd1, CYUSBDRV_GUID, true);
        USBDevice5 = new CCyUSBDevice(hnd5, CYUSBDRV_GUID, true);
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
    // Create the CyUSBDevices
    USBDevice1 = new CCyUSBDevice(hnd1, CYUSBDRV_GUID, true);
    USBDevice5 = new CCyUSBDevice(hnd5, CYUSBDRV_GUID, true);

    if (img_init() == 2)
    {
    }

    delete USBDevice1;
    delete USBDevice5;
    return 0;
}

BOOL _cdecl img_capture(int which_camera)
{
    _RPT0(_CRT_WARN, "IMG CAPTURE\n");
    return 0;
}

int _cdecl img_reset(int which_camera)
{
    _RPT0(_CRT_WARN, "IMG RESET\n");

    return 0;
/*
    LONG bytesToSend = 6;
    unsigned char  buf2[6] = { 0xB3, 0x00, 0x00, 0x00, 0x00, 0x00 };

    CCyControlEndPoint* epControl = getControlEp(which_camera);
    if (epControl == NULL)
        return -1;

    epControl->Target = TGT_DEVICE; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->Direction = DIR_TO_DEVICE;
    epControl->ReqCode = 0xb3; // byte 1
    epControl->Value = 0x0000; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5
    epControl->Write(buf2, bytesToSend);
    
    bytesToSend = 0;
    epControl->Target = TGT_ENDPT; // byte 0
    epControl->ReqType = REQ_STD; // byte 0
    epControl->Direction = DIR_TO_DEVICE;
    epControl->ReqCode = 0x01; // byte 1
    epControl->Value = 0x0000; // byte 2,3
    epControl->Index = 0x0082; // byte 4,5
    epControl->Write(buf2, bytesToSend);

    // ************************************************

    return 0;
    */
}

BOOL _cdecl img_led(int which_camera, short mode)
{
    _RPT0(_CRT_WARN, "IMG LED\n");
    return 0;
}

int _cdecl img_init()
{
    _RPT0(_CRT_WARN, "IMG INIT\n");

    BOOL bXferCompleted = false;
    int n = USBDevice1->DeviceCount();
    long len = 0; // Each xfer request will get PPX isoc packets

    LONG bytesToSend = 0;
    LONG rLen = 0;

    // Find the two camera index
    int index1 = -1;
    int index5 = -1;
    for (int i = 0; i < n; i++)
    {
        if (USBDevice1->IsOpen() == true)
        {
            USBDevice1->Close();
        }
        USBDevice1->Open(i);
        // Is it the Neoden?
        if (USBDevice1->VendorID != 0x52cb)
        {
            continue;
        }

        switch (USBDevice1->Product[0])
        {
            case 'H': //1 
                _RPT0(_CRT_WARN, "WE FOUND CAMERA DOWN\n");
                index1 = i;
                break;
            case 'B': //5
                _RPT0(_CRT_WARN, "WE FOUND CAMERA UP\n");
                index5 = i;
                break;
        }
    }

    n = 0;
    if (index5 != -1)
    {
        if (USBDevice5->IsOpen() == true)
        {
            USBDevice5->Close();
        }
        USBDevice5->Open(index5);
        len = USBDevice5->ControlEndPt->MaxPktSize * PPX;
        USBDevice5->ControlEndPt->SetXferSize(len);
        n++;
    }

    if (index1 != -1)
    {
        if (USBDevice1->IsOpen() == true)
            USBDevice1->Close();
        USBDevice1->Open(index1);
        len = USBDevice1->ControlEndPt->MaxPktSize * PPX;
        USBDevice1->ControlEndPt->SetXferSize(len);
        n++;
    }

    return n;
}

int _cdecl img_read(int which_camera, unsigned char* pFrameBuffer, int BytesToRead, int ms)
{
    _RPT1(_CRT_WARN, "IMG READ: %d %d %d\n", which_camera, BytesToRead, ms);
    return 0;
}

int _cdecl img_readAsy(int which_camera, unsigned char* pFrameBuffer, int BytesToRead, int ms)
{
    _RPT1(_CRT_WARN, "IMG READASY: %d %d %d\n", which_camera, BytesToRead, ms);

    LONG bytesToSend = 6;
    LONG rLen = 0;
    unsigned char  buf2[6] = { 0xB3, 0x00, 0x00, 0x00, 0x00, 0x00 };
    BOOL bXferCompleted = false;
    int retVal = 0;

    CCyControlEndPoint* epControl = getControlEp(which_camera);
    if (epControl == NULL)
        return -1;

    if ( pFrameBuffer == NULL ) {
        return 0;
    }

    CCyBulkEndPoint* epBulkIn = getBulkEp(which_camera);
    if (epBulkIn == NULL)
        return -1;

    epBulkIn->TimeOut = ms;

    epControl->Target = TGT_DEVICE; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->Direction = DIR_TO_DEVICE;
    epControl->ReqCode = 0xb3; // byte 1
    epControl->Value = 0x0000; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5
    epControl->Write(buf2, bytesToSend);

    buf2[0] = 0xB1;

    epControl->Target = TGT_DEVICE; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb1; // byte 1
    epControl->Value = 0x0000; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5
    epControl->Write(buf2, bytesToSend);

    // ************************************************
    rLen = BytesToRead;
    
    bXferCompleted = epBulkIn->XferData(pFrameBuffer, rLen);

    // Read 0 bytes, try again?
    if (rLen == 0)
    {
        epControl->Target = TGT_DEVICE; // byte 0
        epControl->ReqType = REQ_VENDOR; // byte 0
        epControl->Direction = DIR_TO_DEVICE;
        epControl->ReqCode = 0xb3; // byte 1
        epControl->Value = 0x0000; // byte 2,3
        epControl->Index = 0x0000; // byte 4,5
        epControl->Write(buf2, bytesToSend);

        buf2[0] = 0xB1;

        epControl->Target = TGT_DEVICE; // byte 0
        epControl->ReqType = REQ_VENDOR; // byte 0
        epControl->ReqCode = 0xb1; // byte 1
        epControl->Value = 0x0000; // byte 2,3
        epControl->Index = 0x0000; // byte 4,5
        epControl->Write(buf2, bytesToSend);

        rLen = BytesToRead;
        bXferCompleted = epBulkIn->XferData(pFrameBuffer, rLen);
    }

    if (bXferCompleted)
        retVal = 1;
    else
        retVal = 0;

    return retVal;

}

BOOL _cdecl img_set_exp(int which_camera, int16_t exposure)
{
    _RPT1(_CRT_WARN, "IMG SET EXP: %d\n", which_camera);
    unsigned char  buf2[6] = { 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00 };
    LONG bytesToSend = 6;  // 38 + 0 = 38

    CCyControlEndPoint* epControl = getControlEp(which_camera);
    if (epControl == NULL)
        return -1;

    epControl->Target = TGT_DEVICE; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb4; // byte 1
    epControl->Direction = DIR_TO_DEVICE; // 0x40
    epControl->Value = exposure; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5

    buf2[2] = epControl->Value & 0x00FF;
    buf2[3] = (epControl->Value & 0xFF00) >> 8;

    return epControl->Write(buf2, bytesToSend);
}

BOOL _cdecl img_set_gain(int which_camera, int16_t gain)
{
    _RPT1(_CRT_WARN, "IMG SET GAIN: %d\n", which_camera);
    unsigned char  buf2[6] = { 0xB5, 0x00, 0x00, 0x00, 0x00, 0x00 };
    LONG bytesToSend = 6;  // 38 + 0 = 38

    CCyControlEndPoint* epControl = getControlEp(which_camera);
    if (epControl == 0)
        return -1;

    epControl->Target = TGT_DEVICE; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb5; // byte 1
    epControl->Direction = DIR_TO_DEVICE; // 0x40
    epControl->Value = gain; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5

    buf2[2] = epControl->Value & 0x00FF;
    buf2[3] = (epControl->Value & 0xFF00) >> 8;

    return epControl->Write(buf2, bytesToSend);
}

BOOL _cdecl img_set_lt(int which_camera, int16_t a2, int16_t a3)
{
    _RPT1(_CRT_WARN, "IMG SET LT: %d\n", which_camera);
    unsigned char  buf2[6] = { 0xB6, 0x00, 0x00, 0x00, 0x00, 0x00 };
    LONG bytesToSend = 6;  // 38 + 0 = 38

    CCyControlEndPoint* epControl = getControlEp(which_camera);
    if (epControl == NULL)
        return -1;

    epControl->Target = TGT_DEVICE; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb6; // byte 1
    epControl->Direction = DIR_TO_DEVICE; // 0x40

    // ************************************************
    epControl->Value = ((a2 >> 1) << 1) + 12; // byte 2,3
    epControl->Index = 0x0000; // byte 4,5

    buf2[2] = epControl->Value & 0x00FF;
    buf2[3] = (epControl->Value & 0xFF00) >> 8;

    buf2[4] = epControl->Index & 0x00FF;
    buf2[5] = (epControl->Index & 0xFF00) >> 8;

    return epControl->Write(buf2, bytesToSend);
}

BOOL _cdecl img_set_wh(int which_camera, int16_t w, int16_t h)
{
    _RPT1(_CRT_WARN, "IMG SET WH: %d\n", which_camera);
    unsigned char  buf2[6] = { 0xB7, 0x00, 0x00, 0x00, 0x00, 0x00 };
    LONG bytesToSend = 6;  // 38 + 0 = 38

    CCyControlEndPoint* epControl = getControlEp(which_camera);
    if (epControl == NULL)
        return -1;

    epControl->Target = TGT_DEVICE; // byte 0
    epControl->ReqType = REQ_VENDOR; // byte 0
    epControl->ReqCode = 0xb7; // byte 1
    epControl->Direction = DIR_TO_DEVICE; // 0x40

    // ************************************************
    epControl->Value = h; // byte 2,3
    epControl->Index = w; // byte 4,5

    buf2[2] = epControl->Value & 0x00FF;
    buf2[3] = (epControl->Value & 0xFF00) >> 8;

    buf2[4] = epControl->Index & 0x00FF;
    buf2[5] = (epControl->Index & 0xFF00) >> 8;

    return epControl->Write(buf2, bytesToSend);
}