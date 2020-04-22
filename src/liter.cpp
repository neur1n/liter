#include "liter.h"

Liter::Liter()
  :ref_cnt(1)
{
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  this->UpdateDevice();

  // this->hr = CoCreateInstance(
  //     __uuidof(MMDeviceEnumerator),
  //     NULL,
  //     CLSCTX_ALL,
  //     __uuidof(IMMDeviceEnumerator),
  //     (void**)&this->device_enum);

  // this->hr = this->device_enum->GetDefaultAudioEndpoint(
  //     EDataFlow::eRender, ERole::eConsole, &this->device);

  // this->hr = this->device->Activate( 
  //     __uuidof(IAudioEndpointVolume), 
  //     CLSCTX_ALL, 
  //     NULL, 
  //     (void**)&this->endpoint);

  // IAudioSessionManager2 *mgr = NULL;

  // this->hr = this->device->Activate(
  //     __uuidof(IAudioSessionManager2), 
  //     CLSCTX_ALL, 
  //     NULL, 
  //     (void**)&mgr);

  // this->hr = mgr->GetAudioSessionControl(&GUID_NULL, FALSE, &this->session_ctrl);
  // this->hr = this->session_ctrl->RegisterAudioSessionNotification(this);
  // SAFE_RELEASE(mgr);

  // this->hr = this->endpoint->GetMasterVolumeLevelScalar(&this->volume);
}

Liter::~Liter()
{
  this->hr = this->session_ctrl->UnregisterAudioSessionNotification(this);
  SAFE_RELEASE(this->device_enum);
  SAFE_RELEASE(this->device);
  SAFE_RELEASE(this->session_ctrl);
  SAFE_RELEASE(this->endpoint);
}

float Liter::ParseAction(char action, float param)
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

HRESULT Liter::UpdateDevice()
{
  this->hr = CoCreateInstance(
      __uuidof(MMDeviceEnumerator),
      NULL,
      CLSCTX_ALL,
      __uuidof(IMMDeviceEnumerator),
      (void**)&this->device_enum);

  this->hr = this->device_enum->GetDefaultAudioEndpoint(
      EDataFlow::eRender, ERole::eConsole, &this->device);

  this->hr = this->device->Activate( 
      __uuidof(IAudioEndpointVolume), 
      CLSCTX_ALL, 
      NULL, 
      (void**)&this->endpoint);

  IAudioSessionManager2 *mgr = NULL;

  this->hr = this->device->Activate(
      __uuidof(IAudioSessionManager2), 
      CLSCTX_ALL, 
      NULL, 
      (void**)&mgr);

  this->hr = mgr->GetAudioSessionControl(&GUID_NULL, FALSE, &this->session_ctrl);
  this->hr = this->session_ctrl->RegisterAudioSessionNotification(this);
  SAFE_RELEASE(mgr);

  this->hr = this->endpoint->GetMasterVolumeLevelScalar(&this->volume);

  return this->hr;
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
  this->hr = this->endpoint->SetMute(mute == 0.0f, NULL);
  return this->Get();
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

// ================================================================= Overriding
HRESULT Liter::QueryInterface(REFIID iid, void **object)
{
  if (object == NULL)
  {
    return E_POINTER;
  }
  else if (iid == IID_IUnknown)
  {
    *object = static_cast<IUnknown*>(static_cast<IAudioSessionEvents*>(this));
    this->AddRef();
  }
  else if (iid == __uuidof(IAudioSessionEvents))
  {
    *object = static_cast<IAudioSessionEvents*>(this);
    this->AddRef();
  }
  else
  {
    return E_NOINTERFACE;
  }

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

HRESULT Liter::OnSessionDisconnected(AudioSessionDisconnectReason reason)
{
  this->hr = this->UpdateDevice();
  return this->hr;
}
