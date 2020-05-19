#ifndef LITER
#define LITER

#include <iostream>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>  // Must put at last

#define SAFE_RELEASE(punk) \
          if ((punk) != NULL) \
          { \
            (punk)->Release(); \
            (punk) = NULL; \
          }

class Liter: public IMMNotificationClient, IAudioEndpointVolumeCallback
{
public:
  Liter();

  HRESULT Initialize();
  void Dispose();
  float Action(char action, float param);

  STDMETHOD(QueryInterface)(REFIID riid, void **object);
  STDMETHOD_(ULONG, AddRef)();
  STDMETHOD_(ULONG, Release)();

private:
  LONG ref_cnt;
  HRESULT hr;
  IMMDeviceEnumerator *device_enum = NULL;
  IMMDevice *device = NULL;

  IAudioEndpointVolume *endpoint = NULL;
  float volume = 0.0f;

  ~Liter();

  HRESULT AttachDefaultDevice();
  void DettachDefaultDevice();

  float Up();
  float Down();
  float Mute(float mute);
  float Set(float level);
  float Get();
  std::string DefaultDeviceName();

  HRESULT PrintDeviceName(LPCWSTR pwstrDeviceId);

  STDMETHOD(OnDefaultDeviceChanged)(EDataFlow, ERole, LPCWSTR);
  STDMETHOD(OnNotify)(PAUDIO_VOLUME_NOTIFICATION_DATA);

  STDMETHOD(OnDeviceAdded)(LPCWSTR) {return S_OK;};
  STDMETHOD(OnDeviceRemoved)(LPCWSTR) {return S_OK;};
  STDMETHOD(OnDeviceStateChanged)(LPCWSTR, DWORD) {return S_OK;};
  STDMETHOD(OnPropertyValueChanged)(LPCWSTR, const PROPERTYKEY) {return S_OK;};
};

#endif // LITER
