#ifndef LITER
#define LITER

#include <iostream>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>  // Must put at last

#define EXIT_ON_ERROR(hres)  \
          if (FAILED(hres)) \
          { \
            std::cout << "HRESULT: 0x" << std::hex << hres << std::endl; \
            goto Exit; \
          }

#define SAFE_RELEASE(punk) \
          if ((punk) != NULL) \
          { \
            (punk)->Release(); \
            (punk) = NULL; \
          }

class Liter: public IAudioSessionEvents
{
public:
  Liter();
  ~Liter();

  float ParseAction(char action, float param);

  STDMETHOD(QueryInterface)(REFIID riid, void **object);
  STDMETHOD_(ULONG, AddRef)();
  STDMETHOD_(ULONG, Release)();

private:
  LONG ref_cnt;
  HRESULT hr;
  IMMDeviceEnumerator *device_enum = NULL;
  IMMDevice *device = NULL;
  IAudioSessionControl *session_ctrl = NULL;

  IAudioEndpointVolume *endpoint = NULL;
  float volume = 0.0f;

  HRESULT UpdateDevice();

  float Up();
  float Down();
  float Mute(float mute);
  float Set(float level);
  float Get();

  STDMETHOD(OnChannelVolumeChanged)(DWORD, float[], DWORD, LPCGUID) {return S_OK;};
  STDMETHOD(OnDisplayNameChanged)(LPCWSTR, LPCGUID) {return S_OK;};
  STDMETHOD(OnGroupingParamChanged)(LPCGUID, LPCGUID) {return S_OK;};
  STDMETHOD(OnIconPathChanged)(LPCWSTR, LPCGUID) {return S_OK;};
  STDMETHOD(OnSimpleVolumeChanged)(float, BOOL, LPCGUID) {return S_OK;};
  STDMETHOD(OnStateChanged)(AudioSessionState) {return S_OK;};
  STDMETHOD(OnSessionDisconnected)(AudioSessionDisconnectReason reason);
};

#endif // LITER
