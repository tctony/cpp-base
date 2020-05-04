#include <chrono>
#include <iostream>
#include <string>

#include "log_case.h"

std::string optimizeOptions() {
  std::string options;
#ifdef LIGHT_WEIGHT_STRING_ENABLED
  options += " 'light_weight_string'";
#endif
  return options.size() ? options : " none";
}

int main(int argc, char const* argv[]) {
  LogCase* lc = GetLogCase();

  auto start = std::chrono::steady_clock::now();
  lc->run();
  auto end = std::chrono::steady_clock::now();
  auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                  .count();

  std::cout << "running log case '" << lc->name()
            << "' with optimize options:" << optimizeOptions() << ", cost "
            << cost << " ms" << std::endl;

  return 0;
}
