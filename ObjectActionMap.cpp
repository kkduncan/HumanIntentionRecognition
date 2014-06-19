#include <iostream>
#include <fstream>
#include <iomanip>
#include "ObjectActionMap.h"


namespace oar {


void ObjectActionMap::readMap(const std::string& fileName /* = "ObjectActionMap.map" */) {
	std::ifstream inFile;
	inFile.open(fileName.c_str(), std::ios::in);

	if (inFile.is_open()) {
		while (!inFile.eof()) {
			size_t actionIdx, objIndex;
			std::string actionName, objName;
			dai::Prob f(4);
			double values[4];
			double sum = 0.;

			inFile >> objIndex;
			inFile >> objName;
			inFile >> actionIdx;
			inFile >> actionName;

			inFile >> values[0];
			inFile >> values[1];
			inFile >> values[2];
			inFile >> values[3];
			
			sum = values[0] + values[1] + values[2] + values[3];
			//sum = 1.;

			f.set(0, values[0] / sum);
			f.set(1, values[1] / sum);
			f.set(2, values[2] / sum);
			f.set(3, values[3] / sum);

			ObjectActionProperty prop;
			prop.objectIdx = objIndex;
			prop.objectName = objName;
			prop.actionIdx = actionIdx;
			prop.actionName = actionName;
			prop.factor = f;

			if (!objName.empty() && !actionName.empty()) {
				map[objIndex][actionIdx] = prop;
			}
		}

		inFile.close();		

	} else {
		std::cerr << "ObjectActionMap: Unable to open file " << fileName << " for reading!\n";
	}
}


void ObjectActionMap::writeMap(const std::string& fileName /* = "ObjectActionMap.xml" */) {
	std::ofstream outFile;
	outFile.open(fileName.c_str(), std::ios::out);

	if (outFile.is_open()) {
		int lineNum = 0;
		
		for (size_t i = 0; i < this->NUM_OBJECTS; i++) {
			for (size_t j = 0; j < this->NUM_ACTIONS; j++) {
				if (!map[i][j].objectName.empty() && !map[i][j].actionName.empty()) {
					if (lineNum != 0) {
						outFile << std::endl;
					}

					double factorSum = 0.;

					for (size_t k = 0; k < 4; ++k) {
						factorSum += map[i][j].factor.get(k);
					}

					//factorSum = 1;

					outFile << std::setw(3) << map[i][j].objectIdx << " ";
					outFile << std::setw(10)<< map[i][j].objectName << " ";
					outFile << std::setw(3) << map[i][j].actionIdx << " ";
					outFile << std::setw(10)<< map[i][j].actionName << " ";
					outFile << std::setw(19)<< std::fixed << std::setprecision(16) << map[i][j].factor.get(0) / factorSum << " ";
					outFile << std::setw(19)<< std::fixed << std::setprecision(16) << map[i][j].factor.get(1) / factorSum << " ";
					outFile << std::setw(19)<< std::fixed << std::setprecision(16) << map[i][j].factor.get(2) / factorSum << " ";
					outFile << std::setw(19)<< std::fixed << std::setprecision(16) << map[i][j].factor.get(3) / factorSum << " ";

					lineNum++;
				}
			}
		}

		outFile.close();

	} else {
		std::cerr << "ObjectActionMap: Unable to open file " << fileName << " for writing!\n";
	}
}


void ObjectActionMap::resetToDefault() {
	for (size_t i = 0; i < this->NUM_OBJECTS; i++) {
		for (size_t j = 0; j < this->NUM_ACTIONS; j++) {
			if (!map[i][j].objectName.empty() && !map[i][j].actionName.empty()) {
				map[i][j].factor.set(0, 1.0);
				map[i][j].factor.set(1, 1.0);
				map[i][j].factor.set(2, 1.0);
				map[i][j].factor.set(3, 5.0);

			}
		}
	}
}


void ObjectActionMap::updateMap(const size_t& oIdx, const size_t& aIdx) {
	if (aIdx >= 0 && aIdx < this->NUM_ACTIONS && oIdx >= 0 && oIdx < this->NUM_OBJECTS) {
		double value = map[oIdx][aIdx].factor.get(3);
		map[oIdx][aIdx].factor.set(3, value + lambda);
		
	} else {
		std::cerr << "ObjectActionMap: Invalid indices provided for map update\n";
	}
}


double ObjectActionMap::getLambda() const {
	return lambda;
}


void ObjectActionMap::setLambda(const double& learningRate) {
	lambda = learningRate;
}

} // oar
