#include <iostream>
#include "liter.h"

#define DEBUG false

int main(int argc, char *argv[])
{
  HRESULT hr;

  hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

  if (SUCCEEDED(hr))
  {
    char action = NULL;
    float param = 0.0f;
    Liter *liter = new Liter;

    hr = liter->Initialize();

    if (SUCCEEDED(hr))
    {
      while (action != 'x')
      {
#if DEBUG
        std::cout << "Action: ";
#endif
        std::cin.clear();
        std::cin >> action;
        if (action == 'm' || action == 's')
        {
#if DEBUG
          std::cout << "Param: ";
#endif
          std::cin >> param;
        }

        if (action == 'x')
        {
          liter->Dispose();
          liter->Release();
          CoUninitialize();
          return 0;
        }

        std::cout << liter->Action(action, param) << std::endl;
      }

      liter->Dispose();
      liter->Release();
      CoUninitialize();
    }
  }

  return 0;
}
