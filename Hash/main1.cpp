#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>

using namespace std;


// From https://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
// and http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3876.pdf
template <typename T>
inline void hash_combine (std::size_t& seed, const T& val) {
 seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// auxiliary generic functions to create a hash value using a seed
template <typename T, typename... Types>
void hash_combine (std::size_t& seed,
 const T& val, const Types&... args) {
 hash_combine(seed,val);
 hash_combine(seed,args...);
}

// optional auxiliary generic functions to support hash_val() without arguments
void hash_combine (std::size_t& seed) {
}

//  generic function to create a hash value out of a heterogeneous list of arguments
template <typename... Types>
std::size_t hash_val(const Types&... args) {
 std::size_t seed = 0;
 hash_combine (seed, args...);
 return seed;
}

struct StrokeID {
  string userID;
  string canvasID;
  glm::vec3 point;
  float val;
};

int main()
{
  StrokeID strokeID;
  strokeID.userID = "Eu";
  strokeID.canvasID = "SomeCanvas";
  strokeID.point = {1,5,3};
  strokeID.val = 3.f;

  // create the hash function
  auto myval = hash_val<string, string, float>(strokeID.userID, strokeID.canvasID, strokeID.val);


  cout << "Hello hash is: " << myval << endl;

  return 1;
}