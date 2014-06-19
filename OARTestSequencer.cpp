/*
 * OARTestSequencer.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: Kester Duncan
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "OARTestSequencer.h"


namespace oar {

using namespace std;


OARTestSequencer::OARTestSequencer() {
	this->sceneList.reserve(4);

}

OARTestSequencer::~OARTestSequencer() {

}

//////////////////////////////////////////////////////////////////////////////

ObjectDistanceMap OARTestSequencer::generateFixedScene() {
	ObjectDistanceMap sceneObjects;

	// Fixed objects
	sceneObjects.insert(ObjectDistancePair("Tub", 0.521));
	sceneObjects.insert(ObjectDistancePair("Bowl", 0.593));
	sceneObjects.insert(ObjectDistancePair("Carton", 0.609));
	sceneObjects.insert(ObjectDistancePair("Mug", 0.623));
	sceneObjects.insert(ObjectDistancePair("Bottle", 0.712));
	sceneObjects.insert(ObjectDistancePair("Can", 0.792));
	sceneObjects.insert(ObjectDistancePair("Box", 0.817));
	sceneObjects.insert(ObjectDistancePair("Cup", 0.834));

	return sceneObjects;
}

//////////////////////////////////////////////////////////////////////////////

ObjectDistanceMap OARTestSequencer::generateFixedObjectsRandomPositionScene(const std::vector<std::string>& fixedObjs) {
	ObjectDistanceMap sceneObjects;
	int distance;

	for (std::vector<std::string>::const_iterator iter = fixedObjs.begin(); iter != fixedObjs.end(); ++iter) {
		distance = static_cast<int>(rand() % 200 + 50);
		sceneObjects.insert(ObjectDistancePair(*iter, distance / 250.));
	}

	/*
	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("SprayCan", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Tube", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Box", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Tin", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Bottle", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Carton", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Bowl", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Cup", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Tub", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Mug", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Bottle", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Tub", distance / 250.));

	distance = static_cast<int>(rand() % 200 + 50);
	sceneObjects.insert(ObjectDistancePair("Can", distance / 250.));
	*/

	return sceneObjects;
}

//////////////////////////////////////////////////////////////////////////////

/**
 * \brief Utility function to determine if an object name is found in the
 * vector provided
 */
static bool isInList(const vector<string>& objectList, const int& categoryNum) {
	ObjectName objectName;
	bool inList = false;

	vector<string>::const_iterator iter = objectList.begin();
	while (iter != objectList.end()) {
		string name = objectName(static_cast<Object>(categoryNum));
		if (name.compare(*iter) == 0) {
			inList = true;
			break;
		}
		++iter;
	}
	return inList;
}

//////////////////////////////////////////////////////////////////////////////

ObjectDistanceMap OARTestSequencer::generateMixedScene(const std::vector<std::string>& fixedObjs) {
	ObjectDistanceMap sceneObjects;
	ObjectName objectName;
	int distance;

	// Fixed objects (e.g. Carton, Box, Can, Cup)
	for (vector<string>::const_iterator iter = fixedObjs.begin(); iter != fixedObjs.end(); ++iter) {
		distance = static_cast<int>((rand() % 200) + 50);
		sceneObjects.insert(ObjectDistancePair(*iter, distance / 250.));
	}

	// Random objects
	int numCategoriesToSimulate = 8;

	int i = 0;
	int count = 0;
	while (i < numCategoriesToSimulate && count < 100) {
		int catNum;
		catNum = static_cast<int>(rand() % NUM_OBJ_CATEGORIES);
		distance = static_cast<int>((rand() % 200) + 50);

		if (!isInList(fixedObjs, catNum)) {
			sceneObjects.insert(ObjectDistancePair(objectName(static_cast<Object>(catNum)),
					static_cast<double>(distance / 250.)));
			i++;
		}
		count++;
	}

	return sceneObjects;
}

//////////////////////////////////////////////////////////////////////////////

ObjectDistanceMap OARTestSequencer::generateRandomScene(const std::vector<std::string>& fixedObjs) {
	ObjectDistanceMap sceneObjects;
	ObjectName objectName;
	int numCategoriesToSimulate = (rand() % 12) + 3;
	int distance;

	// Fixed objects (e.g. Carton, Bottle, Box)
	for (vector<string>::const_iterator iter = fixedObjs.begin(); iter != fixedObjs.end(); ++iter) {
		distance = static_cast<int>((rand() % 200) + 50);
		sceneObjects.insert(ObjectDistancePair(*iter, distance / 250.));
	}

	int i = 0;
	int count = 0;
	while (i < numCategoriesToSimulate && count < 500) {
		int catNum;
		catNum = static_cast<int>(rand() % NUM_OBJ_CATEGORIES);
		distance = static_cast<int>((rand() % 200) + 50);

		if (!isInList(fixedObjs, catNum)) {
			sceneObjects.insert(ObjectDistancePair(objectName(static_cast<Object>(catNum)),
					static_cast<double>(distance / 250.0)));
			i++;
		}
		count++;
	}

	return sceneObjects;
}

//////////////////////////////////////////////////////////////////////////////

ObjectDistanceMapList OARTestSequencer::generateAndSaveAllSceneTypes(const int& numSessionsEach) {
	ObjectDistanceMapList allScenes;
	/*
	 * TESTED INTENTIONS SEPARATED INTO GROUPS
	 *
	 * Group 1: Grasp-Box, Open-Box, Grasp-Carton, Open-Carton, Pour-from-Carton
	 * Group 2: Grasp-Can, Move-Can, Pour-from-Can, Drink-from-Cup
	 * Group 3: Grasp-Carton, Move-Carton, Pour-from-Carton, Drink-from-Cup
	 * Group 4: Grasp-Bottle, Move-Bottle, Pour-from-Bottle, Drink-from-Cup
	 */
	ObjectDistanceMapList group1ObjPosScenes, group2ObjPosScenes, group3ObjPosScenes, group4ObjPosScenes;
	ObjectDistanceMapList group1RandomScenes, group2RandomScenes, group3RandomScenes, group4RandomScenes;
	ObjectDistanceMapList groupLearningScenes;

	vector<string> fixedObjsForPosTests;
	fixedObjsForPosTests.push_back("SprayCan");
	fixedObjsForPosTests.push_back("Tube");
	fixedObjsForPosTests.push_back("Box");
	fixedObjsForPosTests.push_back("Tin");
	fixedObjsForPosTests.push_back("Bottle");
	fixedObjsForPosTests.push_back("Carton");
	fixedObjsForPosTests.push_back("Bowl");
	fixedObjsForPosTests.push_back("Cup");
	fixedObjsForPosTests.push_back("Tub");
	fixedObjsForPosTests.push_back("Mug");
	fixedObjsForPosTests.push_back("Bottle");
	fixedObjsForPosTests.push_back("Tub");
	fixedObjsForPosTests.push_back("Can");

	/*
	 * Generate 100 sessions for Group 1 Intentions for object position changes
	 */
	for (int i = 0; i < 100; i++) {
		group1ObjPosScenes.push_back(generateFixedObjectsRandomPositionScene(fixedObjsForPosTests));
	}

	// Save sessions
	saveListOfScenes("Group_01_Position_Test.txt", group1ObjPosScenes);

	/*
	 * Generate 80 sessions for all other groups for object position changes
	 */
	for (int i = 0; i < 80; i++) {
		group2ObjPosScenes.push_back(generateFixedObjectsRandomPositionScene(fixedObjsForPosTests));
		group3ObjPosScenes.push_back(generateFixedObjectsRandomPositionScene(fixedObjsForPosTests));
		group4ObjPosScenes.push_back(generateFixedObjectsRandomPositionScene(fixedObjsForPosTests));
	}

	// Save sessions
	saveListOfScenes("Group_02_Position_Test.txt", group2ObjPosScenes);
	saveListOfScenes("Group_03_Position_Test.txt", group3ObjPosScenes);
	saveListOfScenes("Group_04_Position_Test.txt", group4ObjPosScenes);


	vector<string> fixedObjsForRandomTests;
	fixedObjsForRandomTests.push_back("Box");
	fixedObjsForRandomTests.push_back("Carton");
	fixedObjsForRandomTests.push_back("Can");
	fixedObjsForRandomTests.push_back("Cup");
	fixedObjsForRandomTests.push_back("Bottle");

	/*
	 * Generate 100 sessions for Group 1 Intentions for random scene changes
	 */
	for (int i = 0; i < 100; i++) {
		group1RandomScenes.push_back(generateRandomScene(fixedObjsForRandomTests));
	}

	// Save sessions
	saveListOfScenes("Group_01_Objects_Test.txt", group1RandomScenes);


	/*
	 * Generate 80 sessions for all other groups for random scene changes
	 */
	for (int i = 0; i < 80; i++) {
		group2RandomScenes.push_back(generateRandomScene(fixedObjsForRandomTests));
		group3RandomScenes.push_back(generateRandomScene(fixedObjsForRandomTests));
		group4RandomScenes.push_back(generateRandomScene(fixedObjsForRandomTests));
	}

	// Save sessions
	saveListOfScenes("Group_02_Objects_Test.txt", group2RandomScenes);
	saveListOfScenes("Group_03_Objects_Test.txt", group3RandomScenes);
	saveListOfScenes("Group_04_Objects_Test.txt", group4RandomScenes);


	vector<string> fixedObjsForGroupTests;
	fixedObjsForGroupTests.push_back("Box");
	fixedObjsForGroupTests.push_back("Carton");

	/*
	 * Generate 50 sessions for learning a group of intentions
	 */
	for (int i = 0; i < 50; i++) {
		groupLearningScenes.push_back(generateRandomScene(fixedObjsForRandomTests));
	}

	// Save sessions
	saveListOfScenes("Group_Learning_Test.txt", groupLearningScenes);


	return allScenes;
}


void OARTestSequencer::saveListOfScenes(const string& fileName, const ObjectDistanceMapList& list) {
	int len = static_cast<int>(list.size());

	ofstream outputFile;
	outputFile.open(fileName.c_str(), ios::out | ios::binary);

	if (outputFile.is_open()) {
		// Output total number of sessions
		outputFile << len << endl;
		outputFile << endl;

		// For each session, out the list of objects present in session
		for (int i = 0; i < len; i++) {
			ObjectDistanceMap objDistMap = list[i];

			// Session number
			outputFile << i << endl;

			// Number of objects in the session
			outputFile << objDistMap.size() << endl;

			for (ObjectDistanceMap::const_iterator iter = objDistMap.begin(); iter != objDistMap.end(); ++iter) {
				outputFile << setw(10) << iter->first << " " << setprecision(5) << iter->second << endl;
			}

			outputFile << endl;
		}

		outputFile << endl;

	} else {
		fprintf(stderr, "OARTestSequencer Error: Unable to open file for writing!\n");
	}

	outputFile.close();
}


void OARTestSequencer::loadListOfScenes(const string& fileName) {
	int len = 0;
	ifstream inputFile;
	inputFile.open(fileName.c_str(), ios::in | ios::binary);

	if (inputFile.is_open()) {
		// Input total number of sessions
		inputFile >> len;

		if (len > 0) {
			sceneList.reserve(len);
		} else {
			fprintf(stderr, "OARTestSequencer Error: Invalid file format encountered during reading!"
					"Expected a positive number of sessions\n");
			return;
		}

		// For each session, input the list of objects present for session
		for (int i = 0; i < len; i++) {
			ObjectDistanceMap objDistMap;
			int sessionNum = 0;
			int numObjects = 0;

			inputFile >> sessionNum;
			inputFile >> numObjects;

			for (int j = 0; j < numObjects; j++) {
				string objName = "";
				double objDist = 0.;

				inputFile >> objName;
				inputFile >> objDist;

				if (!objName.empty()) {
					if (objDist > 0) {
						objDistMap.insert(ObjectDistancePair(objName, objDist));

					} else {
						fprintf(stderr, "OARTestSequencer Error: Invalid file format encountered during reading! "
								"Expected an object's distance.\n");
						return;
					}

				} else {
					fprintf(stderr, "OARTestSequencer Error: Invalid file format encountered during reading! "
							"Expected an object name.\n");
					return;
				}
			}

			sceneList.push_back(objDistMap);

		}

	} else {
		fprintf(stderr, "OARTestSequencer Error: Unable to open file for reading!\n");
	}

	inputFile.close();
}


} // oar
