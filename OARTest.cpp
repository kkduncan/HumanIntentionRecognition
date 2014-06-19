/*
 * \file Main.cpp
 * \brief Drives the human intention prediction module.
 */
#if 0

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <string>
#include <ctime>
#include <vector>
#include <ctime>



#include "ObjectActionRecognizer.h"
#include "OARTestSequencer.h"
#include "OARUtils.hpp"

using namespace std;
using namespace dai;
using namespace oar;

ObjectDistanceMapList copyRangeOfMaps(const ObjectDistanceMapList& list,
		const unsigned int& start, const unsigned int& end) {

	ObjectDistanceMapList::const_iterator first = list.begin() + start;
	ObjectDistanceMapList::const_iterator last = list.begin() + end;
	ObjectDistanceMapList listRange(first, last);

	return listRange;

}


int main(int argc, char *argv[]) {
	OARTestSequencer seq;
	seq.loadListOfScenes("Tests/Group_Learning_Test.txt");

	//////////////////////////////////////////////////////////////////////////////
	// Change in order for the test data to coincide with the desired intention //
	//////////////////////////////////////////////////////////////////////////////

	string desiredObjectName 	= "Box1";
	string desiredActionName 	= "Grasp";
	unsigned int start 			= 40;
	unsigned int end 			= 50;
	string outputFileName		= "Results/Group_Learning_Counts_Results.csv";

	//////////////////////////////////////////////////////////////////////////////

	ObjectDistanceMapList mapList = copyRangeOfMaps(seq.getListOfScences(), start, end);
	std::vector<int> interactionCount;

	ofstream outputFile;
	outputFile.open(outputFileName.c_str(), ios::binary | ios::app);
	outputFile << desiredActionName << "-" << desiredObjectName << " ";
	cout << desiredActionName << "-" << desiredObjectName << " ";

	ObjectActionRecognizer objectActionRecog("TestObjectActionMap.map");

	for (unsigned int i = 0; i < mapList.size(); i++) {
		objectActionRecog.reinitialize();
		objectActionRecog.constructNetwork(mapList[i]);

		// FIXME: there is a bug in generateQuerySet, investigate
		objectActionRecog.generateCountBasedQuerySet();

		int numInteractions = 0;
		bool choose = false;

		outputFile << (i + 1) << " ";

		do {
			if (objectActionRecog.getQueries().size() == 0) {
				cout << "You have exhausted all possible actions based on your selections!\n";
				break;
			}

			objectActionRecog.selectQuery();
			Query query = objectActionRecog.getCurrentQuery();

			// Determine whether to respond +vely or -vely to this query
			if (query.type == FULL_QUERY) {
				(query.objectName.compare(desiredObjectName) == 0 && query.actionName.compare(desiredActionName) == 0) ? choose = true : choose = false;
			}
			else if (query.type == OBJECT_QUERY) {
				(query.objectName.compare(desiredObjectName) == 0) ? choose = true : choose = false;
			}
			else if (query.type == ACTION_QUERY) {
				(query.actionName.compare(desiredActionName) == 0) ? choose = true : choose = false;
			}
			else {
				cout << "Error encountered: UNKNOWN QUERY TYPE\n";

			}

			numInteractions++;

		} while (!objectActionRecog.evaluate(choose));

		interactionCount.push_back(numInteractions);
		//objectActionRecog.writeTemplates();

	}

	//objectActionRecog.resetTemplates();

	outputFile << endl << " ";
	for (unsigned int k = 0; k < interactionCount.size(); k++) {
		outputFile << interactionCount[k] << " ";
	}
	outputFile << endl;
	outputFile.close();
	cout << "--> Done\n";

    return 0;
}


#endif
