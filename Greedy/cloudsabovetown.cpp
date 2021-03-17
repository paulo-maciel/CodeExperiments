#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

using namespace std;

vector<long> getCloudsAboveTownAtLoc(long townLoc, const vector<long>& cLocs,
                                           const vector<long>& cExts) {
  vector<long> cloudsAboveTown;
  long cloud = 0;
  for (auto cloudLoc : cLocs) {
    auto minCloudLoc = cloudLoc - cExts[cloud];
    auto maxCloudLoc = cloudLoc + cExts[cloud];
    if (townLoc >= minCloudLoc && townLoc <= maxCloudLoc) {
      cloudsAboveTown.push_back(cloudLoc);
    }
    ++cloud;
  }

  return cloudsAboveTown;
}

bool isOnlyCloud(const std::vector<long>& cloudsAboveTown, long cloudLocToCheck) {
  if (cloudsAboveTown.size() == 1 && cloudsAboveTown[0] == cloudLocToCheck) {
    return true;
  }

  return false;
}

long getSunnyPopWOCloud(long cloudLoc,
                const vector<long>& townLocs,
                const vector<long>& townPop,
                const unordered_map<long, vector<long>>& cloudsAboveTownLocMap) {

  long totPop = 0;
  for(size_t town = 0; town < townLocs.size(); ++town) {
    auto cloudsAboveTown = cloudsAboveTownLocMap.at(townLocs[town]);
    if (cloudsAboveTown.empty() ||
        isOnlyCloud(cloudsAboveTown, cloudLoc)) {
      cout << "town: " << town << " pop: " << townPop[town] << endl;
      totPop += townPop[town];
    }
  }
  return totPop;
}

// Complete the maximumPeople function below.
long maximumPeople(vector<long> townPop, vector<long> townLocs,
                   vector<long> cloudLocs, vector<long> cloudExts) {
  // Return the maximum number of people that will be in a sunny town after removing exactly one cloud.
  unordered_map<long, vector<long>> cloudsAboveTownLocMap;
  for(size_t townLoc : townLocs) {
    cloudsAboveTownLocMap[townLoc] = getCloudsAboveTownAtLoc(townLoc, cloudLocs, cloudExts);
  }

  // For all clouds, if 'cloud' is removed, get the population that don't have any
  // clouds above them.
  long maxPop = 0;
  for(auto cloudLoc : cloudLocs) {
    long sunnyPop = getSunnyPopWOCloud(cloudLoc, townLocs, townPop, cloudsAboveTownLocMap);
    maxPop = max(maxPop, sunnyPop);
  }

  return maxPop;
}
int main()
{
  glm::quat q{1,0,0,0};
  cout << "Quat: " << q.x << " " << q.y << " " << q.z << " " << q.w << endl;

  set<int> testSet = {1, 2, 2, 3, 3, 3, 4, 5, 5, 2, 3, 1, 6};

  cout << "Size of set: " << testSet.size() << endl;

  return 1;
}