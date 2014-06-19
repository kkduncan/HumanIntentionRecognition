/**
* Software License Agreement (BSD License)
*
*  Object Action Recognition
*  Copyright (c) 2013, Kester Duncan
*
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
*	\file OARTypes.h
*	\brief Contains definitions of data types used by Object Action Recognition	
*	\author	Kester Duncan
*/
#ifndef OBJECT_ACTION_RECOGNITION_TYPES_H_
#define OBJECT_ACTION_RECOGNITION_TYPES_H_

#include <cstdlib>
#include <iostream>
#include <dai/var.h>
#include <dai/factor.h>
#include <dai/factorgraph.h>


// Classes used from libdai
//using dai::NodeType;
//using dai::Var;
//using dai::Factor;
//using dai::FactorGraph;

using namespace dai;

/**
 * \brief Information related to the variables in the Object-Action Intention Network
 */
typedef dai::Var NetworkNode ;
typedef std::vector<dai::Var> NetWorkNodeList;


/**
 * \brief Namespace that encapsulates all of the functions and types relevant for 
 * human intention recognsition
 */
namespace oar {


/**
 * \brief Represents the different object categories used in this work
 */
enum Object {
	BOTTLE,		// 0
	BOWL,		// 1
	BOX,		// 2
	CAN,		// 3
	CARTON,		// 4
	CUP,		// 5
	MUG,		// 6
	SPRAYCAN,	// 7
	TIN,		// 8
	TUBE,		// 9
	TUB			// 10
};


/**
 * \brief Represents the actions that can be performed on objects
 */
enum Action {
	DRINK,		// 0
	GRASP,		// 1
	MOVE,		// 2
	OPEN,		// 3
	POUR,		// 4
	PUSH,		// 5
	SQUEEZE		// 6
};


/**
 * \brief Represents a mapping of an object enum to its string representation
 */
struct ObjectName {
	std::string operator() (const Object& object) {
		static std::string objectName;

		switch(object) {
		case BOTTLE:
			objectName = "Bottle";
			break;
		case BOWL:
			objectName = "Bowl";
			break;
		case BOX:
			objectName = "Box";
			break;
		case CAN:
			objectName = "Can";
			break;
		case CARTON:
			objectName = "Carton";
			break;
		case CUP:
			objectName = "Cup";
			break;
		case MUG:
			objectName = "Mug";
			break;
		case SPRAYCAN:
			objectName = "SprayCan";
			break;
		case TIN:
			objectName = "Tin";
			break;
		case TUBE:
			objectName = "Tube";
			break;
		case TUB:
			objectName = "Tub";
			break;
		default:
			objectName = "Unknown";
			break;
		}

		return objectName;
	}
};


/**
 * \brief Represents a mapping of an action enum to its string representation
 */
struct ActionName {
	std::string operator() (const Action& action) {
		static std::string actionName;

		switch(action) {
		case DRINK:
			actionName = "Drink";
			break;
		case GRASP:
			actionName = "Grasp";
			break;
		case MOVE:
			actionName = "Move";
			break;
		case OPEN:
			actionName = "Open";
			break;
		case POUR:
			actionName = "Pour";
			break;
		case PUSH:
			actionName = "Push";
			break;
		case SQUEEZE:
			actionName = "Squeeze";
			break;
		default:
			actionName = "Unknown";
			break;
		}

		return actionName;
	}
};


/////////////////////////////////////////////////////////////////////////////////////



/**
 * \brief Represents an object-action pair
 */
struct ObjectActionPair {
	/// Object index
	int objectIndex;

	/// Action Index
	int actionIndex;

	/// Object name
	std::string objectName;

	/// Action Name
	std::string actionName;

	/// Default Constructor
	ObjectActionPair() : objectIndex(-1), actionIndex(-1), objectName(""), actionName("") {}
};

/////////////////////////////////////////////////////////////////////////////////////



/// Represents a pairing type to associate objects to their distances from the camera
typedef std::pair<std::string, double> ObjectDistancePair;

/// Represents a multimap data structure to associate objects to their distances from the camera
typedef std::multimap<std::string, double> ObjectDistanceMap;

/// Represents a sequence of ObjectDistanceMaps (list of scenes)
typedef std::vector<ObjectDistanceMap> ObjectDistanceMapList;

/**
 * \brief Writes an ObjectDistanceMapList to the output stream
 */
static std::ostream& operator << ( std::ostream& os, const ObjectDistanceMapList& mapList ) {
	if (!mapList.empty()) {
		for(unsigned int i = 0; i < mapList.size(); ++i) {
			ObjectDistanceMap map = mapList[i];

			os << "Scene " << i+1 << std::endl;
			for (ObjectDistanceMap::const_iterator iter = map.begin(); iter != map.end(); ++iter) {
				os << "\t< " << iter->first << ", " << iter->second << " >\n";
			}
			os << std::endl;
		}
		os << std::endl;
	}

	return os;
}


/// Comparator for object distances from the camera
struct DistanceComp {
	bool operator() (const ObjectDistancePair& lhs, const ObjectDistancePair& rhs) const {
		return (lhs.second < rhs.second);
	}
};

/////////////////////////////////////////////////////////////////////////////////////



/**
 * \brief Stores the properties of a node in the graph
 */
struct NodeProperties {
	/// Network node label
	size_t label;

	/// Network node name
	std::string name;

	/// Type of node, either object, action, or some feature (e.g. distance)
	NodeType type;

	/// Default constructor
	NodeProperties (const size_t& idx, const std::string& nm, const NodeType& t) : label(idx), name(nm), type(t) {}

};

/// List of Graph Node Properties
typedef std::vector<NodeProperties> NodePropertiesList;

/////////////////////////////////////////////////////////////////////////////////////



/**
 * \brief Represents a stored object category template
 */
struct ObjectTemplateProperties {
	size_t index;
	std::string name;
	std::string fileName;
	std::vector<Action> actions;
	dai::FactorGraph compats;

	ObjectTemplateProperties() : index(0), name(), fileName() {}

};

/// List of Object Template Properties objects
typedef std::vector<ObjectTemplateProperties> ObjectTemplatePropertiesList;


/// Represents a scoring type to associate node indices to marginal probabilities
typedef std::pair<size_t, double> NodeProbabilityPair;


/// Comparator class used for sorting Node Probability Pairs
struct ProbComp {
	bool operator() (const NodeProbabilityPair& lhs, const NodeProbabilityPair& rhs) const {
		double epsilon = 0.0000001;
		return ((rhs.second - lhs.second) < epsilon);
	}
};

/////////////////////////////////////////////////////////////////////////////////////



/// Represents a list of node probability pairs
typedef std::vector<NodeProbabilityPair> NodeProbabilityList;


/// Represents a list of unsigned integer lists
typedef std::vector< std::vector<size_t> > IndexList;


/// Maps a category to a list of factors
typedef std::map<size_t, std::vector<size_t> > ObjectFactorListMap;


/// Maps a scene action index to its template index
typedef std::map<size_t, size_t> ActionTemplateIndexMap;


/// Maps an object index to its template index
typedef std::map<size_t, size_t> ObjectTemplateIndexMap;


/// Map to assign an integer index to a name and vice versa
typedef std::map<size_t, std::string> NameMap;


/// List of Factors
typedef std::vector<dai::Factor> FactorList;



/// The number of object actions this system handles
const size_t NUM_ACTIONS = 7;			


/// The number of object categories handled
const size_t NUM_OBJ_CATEGORIES = 11;	


/// The number that would be added to the factor of a +ve <object, action>
const double LEARNING_RATE = 2.0;	


/////////////////////////////////////////////////////////////////////////////////////

} /* oar */


#endif /* OBJECT_ACTION_RECOGNITION_TYPES_H_ */
