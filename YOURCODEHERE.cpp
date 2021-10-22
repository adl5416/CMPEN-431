#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <math.h>
#include <fcntl.h>
#include <vector>
#include <iterator>

#include "431project.h"

using namespace std;

/*
 * Enter your PSU IDs here to select the appropriate scanning order.
 */
#define PSU_ID_SUM (916184516 + 000000000)

/*
 * Some global variables to track heuristic progress.
 * 
 * Feel free to create more global variables to track progress of your
 * heuristic.
 */
unsigned int currentlyExploringDim = 0;
bool currentDimDone = false;
bool isDSEComplete = false;

/*
 * Given a half-baked configuration containing cache properties, generate
 * latency parameters in configuration string. You will need information about
 * how different cache paramters affect access latency.
 * 
 * Returns a string similar to "1 1 1"
 */
std::string generateCacheLatencyParams(string halfBackedConfig) {

  string latencySettings;

	//
	//YOUR CODE BEGINS HERE
	//
  int il1 = getil1size(std::string configuration);
  int dl1 = getdl1size(std::string configuration);
  int ul2 = getl2size(std::string configuration);
  int il1at;
  int dl1at;
  int ul2lat;
  int twoKB = 2048;
  int fourKB = 4096;
  int eightKB = 8192;
  int sixteenKB = 16384;
  int thirtytwoKB = 32768;
  int sixtyfourKB = 65536;
  int onetwentyeightKB = 131072;
  int twofiftysixKB = 262144;
  int fivetwelveKB = 524288;
  int oneMB = 1048576;
  
  // il1 check
  if (il1 == twoKB) {
    il1at = 1;
  }
  if (il1 == fourKB) {
    il1at = 2;
  }
  if (il1 == eightKB) {
    il1at = 3;
  }
  if (il1 == sixteenKB) {
    il1at = 4;
  }
  if (il1 == thirtytwoKB) {
    il1at = 5;
  }
  if (il1 == sixtyfourKB) {
    il1at = 6;
  }
  // dl1 check
  if (dl1 == twoKB) {
    dl1at = 1;
  }
  if (dl1 == fourKB) {
    dl1at = 2;
  }
  if (dl1 == eightKB) {
    dl1at = 3;
  }
  if (dl1 == sixteenKB) {
    dl1at = 4;
  }
  if (dl1 == thirtytwoKB) {
    dl1at = 5;
  }
  if (dl1 == sixtyfourKB) {
    dl1at = 6;
  }
  // ul2 check
  if (ul2 == thirtytwoKB) {
    ul2lat = 5;
  }
  if (ul2 == sixtyfourKB) {
    ul2lat = 6;
  }
  if (ul2 == onetwentyeight) {
    ul2lat = 7;
  }
  if (ul2 == twofiftysixKB) {
    ul2lat = 8;
  }
  if (ul2 == fivetwelveKB) {
    ul2lat = 9;
  }
  if (ul2 == oneMB) {
    ul2lat = 10;
  }
  std::string s = std::to_string(il1) + " " + std::to_string(dl1) + " " + std::to_string(ul2);
  latencySettings = s;

	//
	//YOUR CODE ENDS HERE
	//

	 
   return latencySettings;
}

/*
 * Returns 1 if configuration is valid, else 0
 */
int validateConfiguration(std::string configuration) {

	// FIXME - YOUR CODE HERE
  int il1 = getil1size(std::string configuration);
  int dl1 = getdl1size(std::string configuration);
  int ul2 = getl2size(std::string configuration);
  int minl1size = 2048;   // 2KB
  int maxl1size = 65536;  // 64KB
  int minl2size = 32769;  // 32KB
  int maxl2size = 1048576;// 1MB

	// The below is a necessary, but insufficient condition for validating a
	// configuration.
  if (il1 != ifg) {
    return 0;
  }
  else if (dl1 != il1) {
    return 0;
  }
  else if (il1 < minl1size) {
    return 0;
  }
  else if (il1 > maxl1siz) {
    return 0;
  }
  else if (dl1 < minl1size) {
    return 0;
  }
  else if (dl1 > maxl1size) {
    return 0;
  }
  else if (ul2 < 2(il1 + dl1)) {
    return 0;
  }
  else if (ul2 < minl2size) {
    return 0;
  }
  else if (ul2 > maxl2size) {
    return 0;
  }
  else if (isNumDimConfiguration(configuration) == 0) {
    return 0;
  }
  else {
    return 1;
  }
}

/*
 * Given the current best known configuration, the current configuration,
 * and the globally visible map of all previously investigated configurations,
 * suggest a previously unexplored design point. You will only be allowed to
 * investigate 1000 design points in a particular run, so choose wisely.
 *
 * In the current implementation, we start from the leftmost dimension and
 * explore all possible options for this dimension and then go to the next
 * dimension until the rightmost dimension.
 */
std::string generateNextConfigurationProposal(std::string currentconfiguration,
		std::string bestEXECconfiguration, std::string bestEDPconfiguration,
		int optimizeforEXEC, int optimizeforEDP) {

	//
	// Some interesting variables in 431project.h include:
	//
	// 1. GLOB_dimensioncardinality
	// 2. GLOB_baseline
	// 3. NUM_DIMS
	// 4. NUM_DIMS_DEPENDENT
	// 5. GLOB_seen_configurations

	std::string nextconfiguration = currentconfiguration;
	// Continue if proposed configuration is invalid or has been seen/checked before.
	while (!validateConfiguration(nextconfiguration) ||
		GLOB_seen_configurations[nextconfiguration]) {

		// Check if DSE has been completed before and return current
		// configuration.
		if(isDSEComplete) {
			return currentconfiguration;
		}

		std::stringstream ss;

		string bestConfig;
		if (optimizeforEXEC == 1)
			bestConfig = bestEXECconfiguration;

		if (optimizeforEDP == 1)
			bestConfig = bestEDPconfiguration;

		// Fill in the dimensions already-scanned with the already-selected best
		// value.
		for (int dim = 0; dim < currentlyExploringDim; ++dim) {
			ss << extractConfigPararm(bestConfig, dim) << " ";
		}

		// Handling for currently exploring dimension. This is a very dumb
		// implementation.
		int nextValue = extractConfigPararm(nextconfiguration,
				currentlyExploringDim) + 1;

		if (nextValue >= GLOB_dimensioncardinality[currentlyExploringDim]) {
			nextValue = GLOB_dimensioncardinality[currentlyExploringDim] - 1;
			currentDimDone = true;
		}

		ss << nextValue << " ";

		// Fill in remaining independent params with 0.
		for (int dim = (currentlyExploringDim + 1);
				dim < (NUM_DIMS - NUM_DIMS_DEPENDENT); ++dim) {
			ss << "0 ";
		}

		//
		// Last NUM_DIMS_DEPENDENT3 configuration parameters are not independent.
		// They depend on one or more parameters already set. Determine the
		// remaining parameters based on already decided independent ones.
		//
		string configSoFar = ss.str();

		// Populate this object using corresponding parameters from config.
		ss << generateCacheLatencyParams(configSoFar);

		// Configuration is ready now.
		nextconfiguration = ss.str();

		// Make sure we start exploring next dimension in next iteration.
		if (currentDimDone) {
			currentlyExploringDim++;
			currentDimDone = false;
		}

		// Signal that DSE is complete after this configuration.
		if (currentlyExploringDim == (NUM_DIMS - NUM_DIMS_DEPENDENT))
			isDSEComplete = true;
	}
	return nextconfiguration;
}

