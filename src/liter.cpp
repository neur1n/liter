#include "liter.h"

Liter::Liter()
  :ref_cnt(1)
{
}

Liter::~Liter()
{
  SAFE_RELEASE(this->device_enum);
  SAFE_RELEASE(this->device);
  SAFE_RELEASE(this->endpoint);
}

// ===================================================================== Public
HRESULT Liter::Initialize()
{
  this->hr = CoCreateInstance(
      __uuidof(MMDeviceEnumerator),
      NULL,
      CLSCTX_ALL,
      __uuidof(IMMDeviceEnumerator),
      (void**)&this->device_enum);

  if (SUCCEEDED(this->hr))
  {
    this->hr = this->device_enum->RegisterEndpointNotificationCallback(this);
    if (SUCCEEDED(this->hr))
    {
      this->hr = this->AttachDefaultDevice();
    }
  }

  return this->hr;
}

void Liter::Dispose()
{
  this->DettachDefaultDevice();
  this->device_enum->UnregisterEndpointNotificationCallback(this);
}

float Liter::Action(char action, float param)
{
  switch (action)
  {
    case 'u':
      return this->Up();
      break;
    case 'd':
      return this->Down();
      break;
    case 'm':
      return this->Mute(param);
      break;
    case 's':
      return this->Set(param);
      break;
    case 'g':
      return this->Get();
      break;
    default:
      return this->Get();
      break;
  }
}

// ==================================================================== Private
HRESULT Liter::AttachDefaultDevice()
{
  this->hr = this->device_enum->GetDefaultAudioEndpoint(
      EDataFlow::eRender, ERole::eConsole, &this->device);

  if (SUCCEEDED(this->hr))
  {
    this->hr = this->device->Activate( 
        __uuidof(IAudioEndpointVolume), 
        CLSCTX_ALL, 
        NULL, 
        (void**)&this->endpoint);

    if (SUCCEEDED(this->hr))
    {
      this->hr = this->endpoint->RegisterControlChangeNotify(this);
    }
  }

  this->Get();

  return this->hr;
}

void Liter::DettachDefaultDevice()
{
  if (this->endpoint != NULL)
  {
    this->endpoint->UnregisterControlChangeNotify(this);
  }

  /* Should not release according to
     https://docs.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nn-mmdeviceapi-immnotificationclient
  */
  // this->endpoint->Release();

  // if (this->device != NULL)
  // {
  //   this->device->Release();
  // }

  // std::cout << "[Dettach]" << std::endl;
}

float Liter::Up()
{
  this->hr = this->endpoint->VolumeStepUp(NULL);
  return this->Get();
}

float Liter::Down()
{
  this->hr = this->endpoint->VolumeStepDown(NULL);
  return this->Get();
}

float Liter::Mute(float mute)
{
  if (mute)
  {
    this->hr = this->endpoint->SetMute(true, NULL);
    return 0.0f;
  }
  else
  {
    this->hr = this->endpoint->SetMute(false, NULL);
    return this->Get();
  }
}

float Liter::Set(float level)
{
  if (level < 0.0f || level > 1.0f)
  {
    return -1.0f;
  }

  this->hr = this->endpoint->SetMasterVolumeLevelScalar(level, NULL);
  this->volume = level;
  return this->volume;
}

float Liter::Get()
{
  this->hr = this->endpoint->GetMasterVolumeLevelScalar(&this->volume);
  return this->volume;
}

HRESULT Liter::PrintDeviceName(LPCWSTR pwstrDeviceId)
{
  IPropertyStore *prop = NULL;
  PROPVARIANT var;
  IMMDevice *dev = NULL;
  HRESULT hr = S_OK;

  PropVariantInit(&var);

  hr = this->device_enum->GetDevice(pwstrDeviceId, &dev);
  hr = dev->OpenPropertyStore(STGM_READ, &prop);
  hr = prop->GetValue(PKEY_Device_FriendlyName, &var);

  std::wcout << var.pwszVal << std::endl;

  PropVariantInit(&var);

  SAFE_RELEASE(prop);
  SAFE_RELEASE(dev);

  return hr;
}

// ================================================================= Overriding
HRESULT Liter::QueryInterface(REFIID iid, void **object)
{
  if (object == NULL)
  {
    return E_POINTER;
  }
  else if (iid == IID_IUnknown)
  {
    *object = static_cast<IUnknown*>(static_cast<IMMNotificationClient*>(this));
  }
  else if (iid == __uuidof(IMMNotificationClient))
  {
    *object = static_cast<IMMNotificationClient*>(this);
  }
  else if (iid == __uuidof(IAudioEndpointVolumeCallback))
  {
    *object = static_cast<IAudioEndpointVolumeCallback*>(this);
  }
  else
  {
    *object = NULL;
    return E_NOINTERFACE;
  }

  this->AddRef();
  return S_OK;
}

ULONG Liter::AddRef()
{
  return InterlockedIncrement(&this->ref_cnt);
}

ULONG Liter::Release()
{
  ULONG v = InterlockedDecrement(&this->ref_cnt);
  if (v == 0)
  {
    delete this;
  }

  return v;
}

/* This function will be called THREE times according to
  https://docs.microsoft.com/en-us/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immnotificationclient-ondefaultdevicechanged#remarks
*/
HRESULT Liter::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId)
{
  // std::cout << "[Changed] ";
  // this->PrintDeviceName(pwstrDefaultDeviceId);
  this->DettachDefaultDevice();
  this->hr = this->AttachDefaultDevice();
  std::cout << this->volume << std::endl;
  return this->hr;
}

HRESULT Liter::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA notify)
{
  if (notify->bMuted)
  {
    this->Mute(1.0f);
  }
  else
  {
    this->volume = this->Mute(0.0f);
  }

  return S_OK;
}
