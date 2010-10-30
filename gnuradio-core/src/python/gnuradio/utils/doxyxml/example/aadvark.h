#include <iostream>

/*!
 * \brief Models the mammal Aadvark.
 *
 * Sadly the model is incomplete and cannot capture all aspects of an aadvark yet.
 *
 * This line is uninformative and is only to test line breaks in the comments.
 */
class Aadvark {
public:
  //! \brief Outputs the vital aadvark statistics.
  void print();
  //! \param aaness The aadvarkness of an aadvark is a measure of how aadvarky it is.
  Aadvark(int aaness);
  int get_aadvarkness();
private:
  int aadvarkness;
};

bool aadvarky_enough(Aadvark aad);

int main();
