#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

// From https://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
// and http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3876.pdf
template <typename T>
inline void hash_combine (std::size_t& seed, const T& val) {
 seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// auxiliary generic functions to create a hash value using a seed
template <typename T, typename... Types>
void hash_combine (std::size_t& seed, const T& val, const Types&... args) {
  hash_combine(seed,val);
  hash_combine(seed,args...);
}

// optional auxiliary generic functions to support hash_key() without arguments
void hash_combine (std::size_t& seed) {
}

//  generic function to create a hash value out of a heterogeneous list of arguments
template <typename... Types>
std::size_t hash_key(const Types&... args) {
  std::size_t seed = 0;
  hash_combine (seed, args...);
  return seed;
}

namespace {
  constexpr char LOCAL_USER[] = "LOCAL_USER";
  constexpr char CANVAS1[] = "CANVAS1";
  constexpr char PEER1[] = "PEER1";
}

struct StrokeAttributes {
  std::string userID;
  std::string canvasID;
  glm::vec4 color;
  float width;
  int eraseMode;
};

int main()
{
  glm::quat q{1,0,0,0};
  std::cout << q.x << " " << q.y << " " << q.z << " " << q.w << std::endl;

  std::unordered_multimap<size_t, std::pair<bool, std::vector<glm::vec3>>> strokesSet;

  StrokeAttributes strokeID = {LOCAL_USER, CANVAS1, glm::vec4(1.0f), 0.1f, 0};
  auto strokeKey = hash_key<std::string, std::string, float, float, float, float, float, int>(
    strokeID.userID, strokeID.canvasID, strokeID.color.r, strokeID.color.g, strokeID.color.b, strokeID.color.a,
    strokeID.width, strokeID.eraseMode);

    StrokeAttributes strokeID2 = {PEER1, CANVAS1, glm::vec4(2.0f), 0.3f, 0};
  auto strokeKey2 = hash_key<std::string, std::string, float, float, float, float, float, int>(
    strokeID2.userID, strokeID2.canvasID, strokeID2.color.r, strokeID2.color.g, strokeID2.color.b, strokeID2.color.a,
    strokeID2.width, strokeID2.eraseMode);

  std::cout << "Hello hash is: " << strokeKey << std::endl;
  bool drawing = true;
  std::vector<glm::vec3> points = {glm::vec3(1), glm::vec3(2)};
  auto drawPoints = std::make_pair<bool, std::vector<glm::vec3>>(std::move(drawing), std::move(points));
  strokesSet.emplace(std::make_pair<size_t, std::pair<bool, std::vector<glm::vec3>>>(std::move(strokeKey), std::move(drawPoints)));

  auto otherPoints = std::make_pair<bool, std::vector<glm::vec3>>(false, std::move(points));
  strokesSet.emplace(std::make_pair<size_t, std::pair<bool, std::vector<glm::vec3>>>(std::move(strokeKey), std::move(otherPoints)));

  strokesSet.emplace(std::make_pair<size_t, std::pair<bool, std::vector<glm::vec3>>>(std::move(strokeKey2), std::move(drawPoints)));

  for(auto strokeSet : strokesSet) {

  }

  return 1;
}