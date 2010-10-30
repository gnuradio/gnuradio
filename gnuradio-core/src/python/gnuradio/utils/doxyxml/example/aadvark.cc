#include <iostream>
#include "aadvark.h"

void Aadvark::print() {
  std::cout << "aadvark is " << aadvarkness << "/10 aadvarky" << std::endl;
}

Aadvark::Aadvark(int aaness): aadvarkness(aaness) {}

bool aadvarky_enough(Aadvark aad) {
  if (aad.get_aadvarkness() > 6)
    return true;
  else
    return false;
}

int Aadvark::get_aadvarkness() {
  return aadvarkness;
}

int main() {
  Aadvark arold = Aadvark(6);
  arold.print();
  if (aadvarky_enough(arold))
    std::cout << "He is aadvarky enough" << std::endl;
  else
    std::cout << "He is not aadvarky enough" << std::endl;
}

