/*
 * \file Main.cpp
 * \brief Drives the human intention prediction module.
 */
#if 1


#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <string>
#include <ctime>
#include <vector>
#include <ctime>
#include <dai/varset.h>
#include <dai/factorgraph.h>
#include <dai/index.h>


#include "ObjectActionRecognizer.h"
#include "OARTestSequencer.h"
#include "OARUtils.hpp"

using namespace std;
using namespace dai;
using namespace oar;


/**
 * Randomly generate a complete scene 
 */
ObjectDistanceMap generateCompleteScene() {
	ObjectDistanceMap sceneObjects;
	string names[11] = {"Bottle", "Bowl", "Box", "Can", "Carton", "Cup", "Mug", "SprayCan", 
						"Tin", "Tube", "Tub"};
	int categoryCount[11] = {0};
	vector<string> categoryNames (names, names + sizeof(names) / sizeof(names[0]));

	int numCategoriesToSimulate = (rand() % 12) + 3;

	// Fixed objects
	int distance;
	
	distance = (int) ((rand() % 200) + 50);
	sceneObjects.insert(pair<string, double>("Carton", distance / 250.));

	distance = (int) ((rand() % 200) + 50);
	sceneObjects.insert(pair<string, double>("Box", distance / 250.));
	

	int i = 0;
	int count = 0;
	while (i < numCategoriesToSimulate && count < 500) {
		int catNum;
		catNum = (int) (rand() % NUM_OBJ_CATEGORIES);		
		distance = (int) ((rand() % 150) + 50);

		if (catNum != 2 && catNum != 4) {
			sceneObjects.insert(pair<string, double>(categoryNames[catNum], (double) (distance / 150.0)));
			++categoryCount[catNum];

			i++;
		}		
		count++;
	}
	
	return sceneObjects;
}

/**
 * An example of how to use the Object-Action Recognizer for Human Intention Recognition
 */
int main(int argc, char *argv[]) {
	srand(static_cast<unsigned int>(time(NULL)));
	string answer ("No");
	double learningRateToUse = 10;
	int inputVal = 0;
	int numSessionsToLearn = 10;
	bool randomlySelect = false;
	std::vector<int> interactionCount;

	std::cout << "-------------------OBJECT-ACTION RECOGNITION-------------------\n";

	((inputVal == 1) ? randomlySelect = true : randomlySelect = false);
	int sessionCount = 0;
	
	do {
		printf("..................................................................\n");
		OARUtils::changeTextColor(1);
		printf("                         SESSION %2d                               \n", ++sessionCount);
		OARUtils::resetTextColor();
		printf("..................................................................\n");
		
		/**
		 * Randomly generate a scene with objects and their distances from the camera
		 */
		ObjectDistanceMap sceneObjects = generateCompleteScene();


		/**
		 * Initialize the object-action recognizer
		 */
		ObjectActionRecognizer objectActionRecog("ObjectActionMap.map", learningRateToUse);



		/**
		 * Construct the network by initializing meta data variables, sort objects according
		 * to their distance from the camera, and create the nodes of the network
		 */
		objectActionRecog.constructNetwork(sceneObjects);


		/**
		 * Generate the query set to be used
		 */
		objectActionRecog.generateMarkovBasedQuerySet();
			
		int numInteractions = 0;
		bool choice = false;

		do {			
			objectActionRecog.selectQuery();			

			if (objectActionRecog.getQueries().size() == 0) {
				cout << "You have exhausted all possible actions based on your selections!\n";
				break;
			}
			
			cout << objectActionRecog.getCurrentQuery().question << ": ";

			int num;
			cin >> num;

			(num == 1) ? choice = true : choice = false;
			numInteractions++;		

		} while (!objectActionRecog.evaluate(choice));

		printf("+---------------------------------------+\n");

		OARUtils::changeTextColor(2);
		printf("  Session: %2d ==> [ %2d ] interactions \n", sessionCount, numInteractions);
		OARUtils::resetTextColor();

		printf("+---------------------------------------+\n");
		interactionCount.push_back(numInteractions);
				
		
		objectActionRecog.writeTemplates();
		sceneObjects.clear();			
	
	} while (sessionCount < numSessionsToLearn);


	printf(" Interaction counts -->\n ");
	for (size_t j = 0; j < interactionCount.size(); j++) {
		printf("%2ld", j + 1);
		if ((j + 1) != interactionCount.size()) {
			printf(", ");
		}
	}
	printf("\n[");
	OARUtils::changeTextColor(1);
	for (size_t j = 0; j < interactionCount.size(); j++) {
		printf("%2d", interactionCount[j]);
		if ((j + 1) != interactionCount.size()) {
			printf(", ");
		}
	}
	OARUtils::resetTextColor();
	printf("]\n");
	
	cout << "Done!\n";
	cin.get();
	

    return 0;
}


#endif
