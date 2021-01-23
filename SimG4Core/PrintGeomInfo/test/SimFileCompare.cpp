////////////////////////////////////////////////////////////////////////////////
//
//    Compares output files from PrintGeomInfo created using DDD and DD4Hep
//    inputs. Usage:
//
//    SimFileCompare infile1 infile2
//    infile1  (const char*)   File created using DDD
//    infile2  (const char*)   File created using DD4Hep
//    deug     (int)           Single digit number (0 minimum printout)
//
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

std::string reducedName(const std::string& name, int debug) {
  std::string nam(name);
  uint32_t first = ((name.find(":") == std::string::npos) ? 0 : (name.find(":") + 1));
  uint32_t last(name.size() + 1);
  uint32_t loc(first);
  while (1) {
    if (name.find("_", loc) == std::string::npos) break;
    if (((loc + 5) < name.size()) && (name.substr(loc, 5) == "shape")) {
      last = loc;
      break;
    }
    loc = name.find("_", loc) + 1;
    if (loc > name.size()) break;
  }
  nam = name.substr(first, last - first - 1);
  if ((last < name.size()) && (name.substr(name.size() - 5, 5) == "_refl"))
    nam += "_refl";
  if (debug > 0)
    std::cout << name << " col " << first << ":" << last << " " << nam << std::endl;
  return nam;
}

void CompareFiles(const char* fileDDD, const char* fileDD4Hep, int debug) {

  std::map<std::string, int> nameDDD, nameDD4Hep;
  std::string name;
  std::ifstream fInput1(fileDDD);
  if (!fInput1.good()) {
    std::cout << "Cannot open file " << fileDDD << std::endl;
  } else {
    while (1) {
      fInput1 >> name;
      if (!fInput1.good())
        break;
      auto it = nameDDD.find(name);
      if (it == nameDDD.end())
	nameDDD[name] = 1;
      else
	++(it->second);
    }
    fInput1.close();
  }
  std::ifstream fInput2(fileDD4Hep);
  if (!fInput2.good()) {
    std::cout << "Cannot open file " << fileDD4Hep << std::endl;
  } else {
    while (1) {
      fInput2 >> name;
      if (!fInput2.good())
        break;
      std::string name0 = reducedName(name, debug);
      auto it = nameDD4Hep.find(name0);
      if (it == nameDD4Hep.end())
	nameDD4Hep[name0] = 1;
      else
	++(it->second);
    }
    fInput1.close();
  }
  std::cout << "Reads " << nameDDD.size() << " names from " << fileDDD << " and " << nameDD4Hep.size() << " names from " << fileDD4Hep << std::endl;

  std::cout << "\nMore than one entry for a given name in " << fileDDD << std::endl;
  for (auto it : nameDDD) {
    if (it.second > 1)
      std::cout << it.first << " : " << it.second << std::endl;
  }
  std::cout << "\nMore than one entry for a given name in " << fileDD4Hep << std::endl;
  for (auto it : nameDD4Hep) {
    if (it.second > 1)
      std::cout << it.first << " : " << it.second << std::endl;
  }
  std::cout << "\nEntry in " << fileDDD << " not in " << fileDD4Hep << std::endl;
  for (auto it : nameDDD) {
    if (nameDD4Hep.find(it.first) == nameDD4Hep.end())
      std::cout << it.first << " appearing " << it.second << " times" << std::endl;
  }
  std::cout << "\nEntry in " << fileDD4Hep << " not in " << fileDDD << std::endl;
  for (auto it : nameDD4Hep) {
    if (nameDDD.find(it.first) == nameDDD.end())
      std::cout << it.first << " appearing " << it.second << " times" << std::endl;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Please give 3 arguments \n"
              << "input file name from the DDD run\n"
              << "input file name from the DD4Hep run\n"
	      << "debug flag (0 for minimum printout)\n"
	      << std::endl;
    return 0;
  }

  const char* infile1 = argv[1];
  const char* infile2 = argv[2];
  int debug = atoi(argv[3]);
  CompareFiles(infile1, infile2, debug);
  return 0;
}
