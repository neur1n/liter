#include <iostream>
#include "liter.h"

#define DEBUG false

int main(int argc, char *argv[])
{
  char action = NULL;
  float param = 0.0;
  Liter liter;

  while (action != 'x')
  {
#if DEBUG
    std::cout << "Action: ";
#endif
    std::cin.clear();
    std::cin >> action;
    if (action == 's')
    {
#if DEBUG
      std::cout << "Param: ";
#endif
      std::cin >> param;
    }

    std::cout << liter.ParseAction(action, param) << std::endl;  // Feedback volume level.
  }

  return 0;
}
