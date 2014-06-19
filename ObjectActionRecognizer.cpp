#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <exception>
#include <boost/foreach.hpp>
#include <dai/alldai.h>
#include <dai/factorgraph.h>
#include <dai/varset.h>
#include "ObjectActionRecognizer.h"

using namespace dai;

namespace oar {


ObjectActionRecognizer::ObjectActionRecognizer(const std::string& oaMapName, const double& learningRate) {
	srand(static_cast<unsigned int>(time(NULL)));

	if (!oaMapName.empty()) {
		this->objectActionMapFileName = oaMapName;
	} else {
		fprintf(stderr, "ObjectActionRecognizer Error: Map filename provided is invalid!\n");
		return;
	}

	objectActionMap.setLambda(learningRate);
	inferenceAlgo = NULL;
	applyTemplates();	

}


ObjectActionRecognizer::~ObjectActionRecognizer() {
	clean();
}


void ObjectActionRecognizer::clean() {
	templateCompats.clear();
	objectNames.clear();
	actionNames.clear();
	allNodes.clear();
	allFactors.clear();
	objectCategoryInstances.clear();
	objectActionFactors.clear();
	actionTemplateIndex.clear();	
	factorCount = 0;
	nodeCount = 0;
	lastFactorIndex = 0;

	if (inferenceAlgo) {
		delete inferenceAlgo;
	}	
}


void ObjectActionRecognizer::reinitialize() {
	clean();

	if (this->objectActionMapFileName.empty()) {
		fprintf(stderr, "ObjectActionRecognizer Error: Map filename is invalid!\n");
		return;
	}

	inferenceAlgo = NULL;

	if (templateCompats.empty() && objectCategoryInstances.empty()) {
		applyTemplates();
	}
}


void ObjectActionRecognizer::applyTemplates() {
	// Bottles
	ObjectTemplateProperties bottleProps;
	bottleProps.index = 0;
	bottleProps.name = "Bottle";
	bottleProps.actions.push_back(DRINK);
	bottleProps.actions.push_back(GRASP);
	bottleProps.actions.push_back(MOVE);
	bottleProps.actions.push_back(OPEN);
	bottleProps.actions.push_back(POUR);
	
	// Bowls
	ObjectTemplateProperties bowlProps;
	bowlProps.index = 1;
	bowlProps.name = "Bowl";
	bowlProps.actions.push_back(GRASP);
	bowlProps.actions.push_back(MOVE);
	bowlProps.actions.push_back(PUSH);
	
	// Boxes
	ObjectTemplateProperties boxProps;
	boxProps.index = 2;
	boxProps.name = "Box";
	boxProps.actions.push_back(GRASP);
	boxProps.actions.push_back(MOVE);
	boxProps.actions.push_back(OPEN);
	boxProps.actions.push_back(PUSH);
	
	// Cans
	ObjectTemplateProperties canProps;
	canProps.index = 3;
	canProps.name = "Can";
	canProps.actions.push_back(DRINK);
	canProps.actions.push_back(GRASP);
	canProps.actions.push_back(MOVE);
	canProps.actions.push_back(POUR);
	
	// Cartons
	ObjectTemplateProperties cartonProps;
	cartonProps.index = 4;
	cartonProps.name = "Carton";
	cartonProps.actions.push_back(GRASP);
	cartonProps.actions.push_back(MOVE);
	cartonProps.actions.push_back(OPEN);
	cartonProps.actions.push_back(POUR);
	
	// Cups
	ObjectTemplateProperties cupProps;
	cupProps.index = 5;
	cupProps.name = "Cup";
	cupProps.actions.push_back(DRINK);
	cupProps.actions.push_back(GRASP);
	cupProps.actions.push_back(MOVE);
	
	// Mugs
	ObjectTemplateProperties mugProps;
	mugProps.index = 6;
	mugProps.name = "Mug";
	mugProps.actions.push_back(DRINK);
	mugProps.actions.push_back(GRASP);
	mugProps.actions.push_back(MOVE);
	
	// SprayCans
	ObjectTemplateProperties sprayCanProps;
	sprayCanProps.index = 7;
	sprayCanProps.name = "SprayCan";
	sprayCanProps.actions.push_back(GRASP);
	
	// Tins
	ObjectTemplateProperties tinProps;
	tinProps.index = 8;
	tinProps.name = "Tin";
	tinProps.actions.push_back(GRASP);
	tinProps.actions.push_back(MOVE);
	tinProps.actions.push_back(OPEN);
	tinProps.actions.push_back(POUR);
	
	// Tubes
	ObjectTemplateProperties tubeProps;
	tubeProps.index = 9;
	tubeProps.name = "Tube";
	tubeProps.actions.push_back(GRASP);
	tubeProps.actions.push_back(SQUEEZE);
	
	// Tubs
	ObjectTemplateProperties tubContainerProps;
	tubContainerProps.index = 10;
	tubContainerProps.name = "Tub";
	tubContainerProps.actions.push_back(GRASP);
	tubContainerProps.actions.push_back(OPEN);
	tubContainerProps.actions.push_back(PUSH);
	

	/*
	 * Assign to template compatibities list
	 */
	templateCompats.push_back(bottleProps);
	templateCompats.push_back(bowlProps);
	templateCompats.push_back(boxProps);
	templateCompats.push_back(canProps);
	templateCompats.push_back(cartonProps);
	templateCompats.push_back(cupProps);
	templateCompats.push_back(mugProps);
	templateCompats.push_back(sprayCanProps);
	templateCompats.push_back(tinProps);
	templateCompats.push_back(tubeProps);
	templateCompats.push_back(tubContainerProps);

	
	/*
	 * Read object-action template map
	 */
	objectActionMap.readMap(objectActionMapFileName);
		
	for (size_t i = 0; i < NUM_OBJ_CATEGORIES; i++) {
		std::vector<size_t> objCategory;
		objectCategoryInstances.push_back(objCategory);
	}
}


NetworkNode ObjectActionRecognizer::createGraphNode(const std::string& nodeName, const NodeType& type, const size_t& indexInTemplate) {
	NetworkNode variableNode;
	assert(nodeCount >= 0);

	if (nodeCount >= 0 && !nodeName.empty()) {
		NodeType nodeType;				
		
		// Store the properties of each node in the appropriate global information variables
		if (type == dai::OBJECT) {
			// Store object category instance information
			std::string newObjectName = nodeName;
			int numCategoryInstances = objectCategoryInstances[indexInTemplate].size();
			
			newObjectName.append(boost::lexical_cast<std::string, size_t>(numCategoryInstances + 1));
			
			objectCategoryInstances[indexInTemplate].push_back(nodeCount);
			
			nodeType = dai::OBJECT;
			objectNames[nodeCount] = newObjectName;
			variableNode = NetworkNode(nodeCount, 2, newObjectName, type);				
	
			// Store the node information globally 
			objectTemplateIndex[nodeCount] = indexInTemplate;
			allNodes.push_back(NodeProperties(nodeCount, newObjectName, nodeType));
			
			// Update global node count
			nodeCount++;

		} else if (type == dai::ACTION) {
			/*
			 * Ensure that there are unique action nodes! If an
			 * action node has already been created, we reuse the index
			 * of that action
			 */
			size_t actionNodeIdx = nodeCount;
			bool actionNodeExists = false;

			for (std::map<size_t, std::string>::iterator it = actionNames.begin(); it != actionNames.end(); ++it) {
				if (it->second.compare(nodeName) == 0) {
					actionNodeIdx = it->first;
					actionNodeExists = true;
					break;
				}
			}

			actionNames[actionNodeIdx] = nodeName;
			nodeType = dai::ACTION;
			variableNode = NetworkNode(actionNodeIdx, 2, nodeName, type);

			// Update global node count
			if (!actionNodeExists) {
				// Store the node information globally 
				allNodes.push_back(NodeProperties(actionNodeIdx, nodeName, nodeType));
				actionTemplateIndex[actionNodeIdx] = indexInTemplate;
				
				nodeCount++;
			}

		} else if (type == dai::POSITION) {
			nodeType = dai::POSITION;
			variableNode = NetworkNode(nodeCount, 2, nodeName, type);
			
			// Store the node information globally 
			allNodes.push_back(NodeProperties(nodeCount, nodeName, nodeType));
			
			// Update global node count
			nodeCount++;

		} else {
			nodeType = dai::UNKNOWN;
			variableNode = NetworkNode(nodeCount, 2, nodeName, type);

			// Store the node information globally 
			allNodes.push_back(NodeProperties(nodeCount, nodeName, nodeType));
			
			// Update global node count
			nodeCount++;
		}				

	} else {
		DAI_THROWE(INTERNAL_ERROR, "createGraphNode(): nodeIndex or nodeName are invalid!");
	}

	return variableNode;
}


void ObjectActionRecognizer::addGraphFactor(FactorList& factorList, dai::Factor& newFactor) {
	factorList.push_back(newFactor);
	++factorCount;
	lastFactorIndex = factorCount - 1;	
}


FactorList ObjectActionRecognizer::instantiateObject(const std::string& objName, const bool& near, const double& distance) {
	std::vector<dai::Factor> objFactors;
	std::vector<size_t> objectActionFactorIndices;
	std::vector<NetworkNode> objectActionNodes;
	std::string distanceNodeName;
	size_t categoryIndex = 0;
			
	for (size_t i = 0; i < templateCompats.size(); i++) {
		if (objName.compare(templateCompats[i].name) == 0) {
			categoryIndex = i;
			break;
		}
	}

	// Get the category to which this scene object belongs
	ObjectTemplateProperties objectCategory = templateCompats[categoryIndex];
	
	/*
	 * Create the action nodes for this object FIRST, so that their labels precede the object's
	 * This is essence creates the first level of the graph, which is part of the ancillary
	 * layer.
	 */
	for (size_t i = 0; i < objectCategory.actions.size(); i++) {
		size_t templateActionIdx = 0;
		std::string actionName;
		
		switch (objectCategory.actions[i]) {
			case DRINK:
				templateActionIdx = 0;
				actionName = "Drink";
				break;
			case GRASP:
				templateActionIdx = 1;
				actionName = "Grasp";
				break;
			case MOVE:
				templateActionIdx = 2;
				actionName = "Move";
				break;
			case OPEN:
				templateActionIdx = 3;
				actionName = "Open";
				break;
			case POUR:
				templateActionIdx = 4;
				actionName = "Pour";
				break;
			case PUSH:
				templateActionIdx = 5;
				actionName = "Push";
				break;
			case SQUEEZE:
				templateActionIdx = 6;
				actionName = "Squeeze";
				break;
			default:
				templateActionIdx = 1;
				actionName = "Grasp";
				break;
		}
		
		NetworkNode actionNode = createGraphNode(actionName, dai::ACTION, templateActionIdx);
		objectActionNodes.push_back(actionNode);
	}
	
	/*
	 * Creates the object node.
	 */
	NetworkNode objectNode = createGraphNode(objName, dai::OBJECT, categoryIndex);
	
	/*
	 * Create the distance node name from the object instance's name.
	 */
	distanceNodeName = objectNode.name() + "\nDistance";	
	NetworkNode distanceNode = createGraphNode(distanceNodeName, dai::POSITION);
	
	// Create <object-action> factors
	for (size_t i = 0; i < objectActionNodes.size(); i++) {
		dai::Factor objActionCompat(dai::VarSet(objectActionNodes[i], objectNode));
		size_t templateActionIdx = actionTemplateIndex[objectActionNodes[i].label()];
		size_t templateObjectIdx = objectTemplateIndex[objectNode.label()];
		ObjectActionProperty prop = objectActionMap(templateObjectIdx, templateActionIdx);

		// Add to object-action pair count
		objectActionCountMap(templateObjectIdx, templateActionIdx);
		
		objActionCompat.set(0, prop(0));
		objActionCompat.set(1, prop(1));
		objActionCompat.set(2, prop(2));
		objActionCompat.set(3, prop(3));
		
		/// Adds an object-action compatibility to the list of factors for this object
		addGraphFactor(objFactors, objActionCompat);	

		// Add index of recently created <object, action> factor to the list of factors for this object instance
		objectActionFactorIndices.push_back(lastFactorIndex);

	}

	// Store objectActionFactors for this particular object instance
	objectActionFactors[objectNode.label()] = objectActionFactorIndices;

	
	// Create the object and position factors 
	// FIXME: the probability formulation needs to be adjusted
	// TODO: make note of this formulation in the paper
	dai::Factor objPositionCompat(dai::VarSet(objectNode, distanceNode));	
	if (near == true) {
		objPositionCompat.set(0, 1.0 - distance);	// FF
		objPositionCompat.set(1, distance);			// TF
		objPositionCompat.set(2, distance);			// FT
		objPositionCompat.set(3, 1.0 - distance);	// TT
	} else {
		objPositionCompat.set(0, distance);
		objPositionCompat.set(1, 1.0 - distance);
		objPositionCompat.set(2, 1.0 - distance);
		objPositionCompat.set(3, distance);
	}

	// Adds the object-position compatibility to the list of factors for this object
	addGraphFactor(objFactors, objPositionCompat);
	
	return objFactors;
}


void ObjectActionRecognizer::getMarginalProbabilities() {
	std::vector<NetworkNode> nodes = theNetwork.vars();
	std::vector<dai::Factor> factors = theNetwork.factors();

	actions.clear();
	objects.clear();
	relations.clear();

	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i].type() == dai::ACTION) {
			NodeProbabilityPair s;
			s.first = i;
			s.second = inferenceAlgo->belief(nodes[i])[1];
			actions.push_back(s);				

		} else if (nodes[i].type() == dai::OBJECT) {
			NodeProbabilityPair s;
			s.first = i;
			s.second = inferenceAlgo->belief(nodes[i])[1];
			objects.push_back(s);

		}
	}	

	for (size_t k = 0; k < factors.size(); k++) {
		std::vector<NetworkNode> vars = factors[k].vars().elements();
		bool relevant = false;

		if (vars.size() == 2) {
			if (vars[0].type() == dai::ACTION && vars[1].type() == dai::OBJECT) {
				relevant = true;					
			} else if (vars[1].type() == dai::ACTION && vars[0].type() == dai::OBJECT) {
				relevant = true;	
			}
		}	

		if (relevant == true) {
			NodeProbabilityPair s;
			s.first = k;
			s.second = inferenceAlgo->belief(factors[k].vars())[3];
			relations.push_back(s); 						
		}			
	}
	
}


void ObjectActionRecognizer::getNumericalProbabilities() {
	std::vector<NetworkNode> nodes = theNetwork.vars();
	std::vector<dai::Factor> factors = theNetwork.factors();

	actions.clear();
	objects.clear();
	relations.clear();

	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i].type() == dai::ACTION) {
			NodeProbabilityPair s;
			size_t actionIdx = actionTemplateIndex[nodes[i].label()];
			s.first = i;
			s.second = objectActionCountMap.getActionProbability(actionIdx);
			actions.push_back(s);				

		} else if (nodes[i].type() == dai::OBJECT) {
			NodeProbabilityPair s;
			size_t objectIdx = objectTemplateIndex[nodes[i].label()];
			s.first = i;
			s.second = objectActionCountMap.getObjectProbability(objectIdx);
			objects.push_back(s);

		}
	}	

	for (size_t k = 0; k < factors.size(); k++) {
		std::vector<NetworkNode> vars = factors[k].vars().elements();
		bool relevant = false;

		if (vars.size() == 2) {
			if (vars[0].type() == dai::ACTION && vars[1].type() == dai::OBJECT) {
				relevant = true;					
			} else if (vars[1].type() == dai::ACTION && vars[0].type() == dai::OBJECT) {
				relevant = true;	
			}
		}	

		if (relevant == true) {
			NodeProbabilityPair s;
			s.first = k;
			s.second = static_cast<double>(1. / objectActionCountMap.getTotalCount());
			relations.push_back(s); 						
		}			
	}

}


void ObjectActionRecognizer::constructNetwork(const ObjectDistanceMap& sceneObjects, const bool& useCounts /* = false */) {
	// Initialize global factor count
	factorCount = 0;

	// Initialize global variable node count
	nodeCount = 0;

	//int numberObjs = (int) sceneObjects.size();
	double maxDistance = std::numeric_limits<double>::min();
	double distThreshold = 0.;
		
	std::vector<ObjectDistancePair> objects;
		
	for (ObjectDistanceMap::const_iterator iter = sceneObjects.begin(); iter != sceneObjects.end(); ++iter) {
		std::string objName (iter->first);
		double distance = iter->second;

		ObjectDistancePair g;
		g.first = objName;
		g.second = distance;
		objects.push_back(g);

		if (distance > maxDistance) {
			maxDistance = distance;
			maxDistance += 0.02; // Adds two cm. to the max distance
		}
	}

	/* Sort the objects according to their distance from the camera */
	sort(objects.begin(), objects.end(), DistanceComp());

	/* Determine distance threshold */
	distThreshold = maxDistance / 2.0;

	/* Create network nodes; actions first, then objects and their features (distances in this case) */
	for (std::vector<ObjectDistancePair>::iterator iter = objects.begin(); iter != objects.end(); ++iter) {
		std::string objName = iter->first;
		double distance = iter->second;
		std::vector<dai::Factor> objFactors;
			
		if (distance < distThreshold) {
			/* For objects that are near to the camera */
			objFactors = instantiateObject(objName, true, (distance / maxDistance));
		} else {
			/* For objects that are far from the camera */
			objFactors = instantiateObject(objName, false, (distance / maxDistance));
		}
			
		allFactors.insert(allFactors.end(), objFactors.begin(), objFactors.end());			
	}	
	
	/*
	 * Create the object-action intention network
	 */
	theNetwork = dai::FactorGraph(allFactors);	

	
	/*
	 * Instantiate the inference algorithm
	 */
	dai::PropertySet infProps;
	infProps.set("tol", 0.00000001);
	infProps.set("logdomain", true);
	infProps.set("updates", std::string("SEQMAX"));
	infProps.set("inference", std::string("MAXPROD"));

	inferenceAlgo = newInfAlg("BP", theNetwork, infProps );
	inferenceAlgo->init();
	inferenceAlgo->run();

	if (!useCounts) {
		getMarginalProbabilities();
	} else {
		getNumericalProbabilities();
	}
	
}


void ObjectActionRecognizer::generateMarkovBasedQuerySet() {
	int queryIdx = 0;
	queries.clear();

	// Add the action queries to the set
	for (size_t i = 0; i < actions.size(); i++) {			
		Query q (queryIdx, actions[i].second, -1, actions[i].first, "", allNodes[actions[i].first].name);
		q.type = ACTION_QUERY;
		queries.push_back(q);
		queryIdx++;
			
	}

	// Add the object queries to the set
	for (size_t j = 0; j < objects.size(); j++) {
		Query q (queryIdx, objects[j].second, objects[j].first, -1, allNodes[objects[j].first].name, "");
		q.type = OBJECT_QUERY;
		queries.push_back(q);				
		queryIdx++;
			
	}				

	// Add the <object-action> queries to the set
	for (size_t i = 0; i < relations.size(); i++) {
		std::string objName, actionName;
		size_t objIdx, actionIdx;
		size_t factorIdx = relations[i].first;
		std::vector<NetworkNode> nodes = theNetwork.factors()[factorIdx].vars().elements();

		/*
		 * All factors ONLY involve two variables, therefore in this case one is an action
		 * and the other is an object. Thus, we handle these accordingly.
		 */
		assert(nodes.size() == 2);
		if (nodes[0].type() == dai::ACTION) {
			actionIdx = nodes[0].label();
			objIdx = nodes[1].label();
			actionName = actionNames[actionIdx];
			objName = objectNames[objIdx];
			
		} else if (nodes[0].type() == dai::OBJECT) {
			objIdx = nodes[0].label();
			actionIdx = nodes[1].label();
			objName = objectNames[objIdx];
			actionName = actionNames[actionIdx];				
		}

		Query q(queryIdx, relations[i].second, objIdx, actionIdx, objName, actionName);
		q.type = FULL_QUERY;
		queries.push_back(q);
		queryIdx++;
			
	}

	std::sort(queries.begin(), queries.end(), QueryComparator());
}


void ObjectActionRecognizer::generateCountBasedQuerySet() {
	int queryIdx = 0;
	queries.clear();

	// Add the action queries to the set
	for (size_t i = 0; i < actions.size(); i++) {
		Query q (queryIdx, actions[i].second, -1, actions[i].first, "", allNodes[actions[i].first].name);
		q.type = ACTION_QUERY;
		queries.push_back(q);
		queryIdx++;

	}

	// Add the object queries to the set
	for (size_t j = 0; j < objects.size(); j++) {
		Query q (queryIdx, objects[j].second, objects[j].first, -1, allNodes[objects[j].first].name, "");
		q.type = OBJECT_QUERY;
		queries.push_back(q);
		queryIdx++;

	}

	// Add the <object-action> queries to the set
	for (size_t i = 0; i < relations.size(); i++) {
		std::string objName, actionName;
		size_t objIdx, actionIdx;
		size_t factorIdx = relations[i].first;
		std::vector<NetworkNode> nodes = theNetwork.factors()[factorIdx].vars().elements();

		/*
		 * All factors ONLY involve two variables, therefore in this case one is an action
		 * and the other is an object. Thus, we handle these accordingly.
		 */
		assert(nodes.size() == 2);
		if (nodes[0].type() == dai::ACTION) {
			actionIdx = nodes[0].label();
			objIdx = nodes[1].label();
			actionName = actionNames[actionIdx];
			objName = objectNames[objIdx];

		} else if (nodes[0].type() == dai::OBJECT) {
			objIdx = nodes[0].label();
			actionIdx = nodes[1].label();
			objName = objectNames[objIdx];
			actionName = actionNames[actionIdx];
		}

		Query q(queryIdx, relations[i].second, objIdx, actionIdx, objName, actionName);
		q.type = FULL_QUERY;
		queries.push_back(q);
		queryIdx++;

	}

	std::sort(queries.begin(), queries.end(), CountsQueryComparator());

}


void ObjectActionRecognizer::generateRandomQuerySetBasedOnScene() {
	int queryIdx = 0;
	queries.clear();

	// Add the action queries to the set
	for (size_t i = 0; i < actions.size(); i++) {
		Query q (queryIdx, actions[i].second, -1, actions[i].first, "", allNodes[actions[i].first].name);
		q.type = ACTION_QUERY;
		queries.push_back(q);
		queryIdx++;

	}

	// Add the object queries to the set
	for (size_t j = 0; j < objects.size(); j++) {
		Query q (queryIdx, objects[j].second, objects[j].first, -1, allNodes[objects[j].first].name, "");
		q.type = OBJECT_QUERY;
		queries.push_back(q);
		queryIdx++;

	}

	// Add the <object-action> queries to the set
	for (size_t i = 0; i < relations.size(); i++) {
		std::string objName, actionName;
		size_t objIdx, actionIdx;
		size_t factorIdx = relations[i].first;
		std::vector<NetworkNode> nodes = theNetwork.factors()[factorIdx].vars().elements();

		/*
		 * All factors ONLY involve two variables, therefore in this case one is an action
		 * and the other is an object. Thus, we handle these accordingly.
		 */
		assert(nodes.size() == 2);
		if (nodes[0].type() == dai::ACTION) {
			actionIdx = nodes[0].label();
			objIdx = nodes[1].label();
			actionName = actionNames[actionIdx];
			objName = objectNames[objIdx];

		} else if (nodes[0].type() == dai::OBJECT) {
			objIdx = nodes[0].label();
			actionIdx = nodes[1].label();
			objName = objectNames[objIdx];
			actionName = actionNames[actionIdx];
		}

		Query q(queryIdx, relations[i].second, objIdx, actionIdx, objName, actionName);
		q.type = FULL_QUERY;
		queries.push_back(q);
		queryIdx++;
	}

	std::random_shuffle(queries.begin(), queries.end());

}


void ObjectActionRecognizer::generateRandomQuerySet() {
	int queryIdx = 0;
	queries.clear();

	for (size_t i = 0; i < objectActionMap.getNumOfObjects(); ++i) {
		ObjectActionProperty oap = objectActionMap(i, 0);
		if (!oap.objectName.empty()) {
			/*
			 * In the random case, we assume that there is only one
			 * instance of an object for simplicity.
			 */
			std::string objName = oap.objectName + "1";

			Query q (queryIdx, 1.0, i, -1, objName, "");
			q.type = OBJECT_QUERY;
			queries.push_back(q);
			queryIdx++;
		}
	}

	for (size_t j = 0; j < objectActionMap.getNumOfActions(); ++j) {
		ObjectActionProperty oap = objectActionMap(0, j);
		if (!oap.actionName.empty()) {
			std::string actionName = oap.actionName;
			Query q (queryIdx, 1.0, -1, j, "", actionName);
			q.type = ACTION_QUERY;
			queries.push_back(q);
			queryIdx++;
		}
	}

	for (size_t i = 0; i < objectActionMap.getNumOfObjects(); ++i) {
		for (size_t j = 0; j < objectActionMap.getNumOfActions(); ++j) {
			ObjectActionProperty oap = objectActionMap(i, j);
			if (!oap.objectName.empty() && !oap.actionName.empty()) {
				std::string objectName = oap.objectName + "1";
				std::string actionName = oap.actionName;

				Query q(queryIdx, 1.0, i, j, objectName, actionName);
				q.type = FULL_QUERY;
				queries.push_back(q);
				queryIdx++;
			}
		}
	}

	std::random_shuffle(queries.begin(), queries.end());

}


dai::FactorGraph ObjectActionRecognizer::getNetwork() const {
	return theNetwork;
}


void ObjectActionRecognizer::writeTemplates() {
	objectActionMap.writeMap(objectActionMapFileName);
}


void ObjectActionRecognizer::resetTemplates() {
	objectActionMap.resetToDefault();
	writeTemplates();
}


void ObjectActionRecognizer::updateTemplatesUsingObservations() {
	for (size_t i = 0; i < objectCategoryInstances.size(); ++i) {
		/*
		 * Get the number of object instances for the category i
		 */
		std::vector<size_t> instances = objectCategoryInstances[i];

		if (instances.size() > 0) {				
			std::vector< std::vector<dai::Factor> > allInstancesFactors;

			for (size_t j = 0; j < instances.size(); j++) {
				size_t instanceNodeIdx = instances[j];
				std::vector<size_t> instanceFactorIndices = objectActionFactors[instanceNodeIdx];
				std::vector<dai::Factor> instanceFactors; 

				/*
				 * Store the factors of each instance of this i'th category
				 */
				for (size_t k = 0; k < instanceFactorIndices.size(); k++) {
					instanceFactors.push_back(theNetwork.factor(instanceFactorIndices[k]));
				}

				allInstancesFactors.push_back(instanceFactors);
			}
				
			/*
			 * For each instance, get their respective factors and
			 * find the mean
			 */
			std::vector<dai::Factor> averageInstanceFactors;

			/*
			 * Get template factor values
			 */
			std::vector<dai::Factor> categoryFactors (templateCompats[i].compats.factors());
				
			/*
			 * Store the first instance's factors
			 */
			for (size_t k = 0; k < allInstancesFactors[0].size(); k++) {	
				averageInstanceFactors.push_back(allInstancesFactors[0][k]);	
			}				
							
			/*
			 * Sum all of the corresponding factors. Start from the index of the second instance (if it exists)
			 */
			for (size_t j = 1; j < allInstancesFactors.size(); j++) {			// For every instance
				for (size_t k = 0; k < allInstancesFactors[j].size(); k++) {	// For every factor of the current instance
					for (size_t s = 0; s < averageInstanceFactors[k].nrStates(); s++) {
						double origValue = averageInstanceFactors[k].get(s);

						// Add all the corresponding factors
						averageInstanceFactors[k].set(s, origValue + allInstancesFactors[j][k].get(s));		
					}						
				}				
			}
				
								
			/*
			 * Get average of factors for the instances
			 */
			for (size_t k = 0; k < averageInstanceFactors.size(); k++) {
				averageInstanceFactors[k] /= static_cast<double>(instances.size());
			}	


			for (size_t k = 0; k < averageInstanceFactors.size(); k++) {
				for (size_t s = 0; s < averageInstanceFactors[k].nrStates(); s++) {
					double instanceAvgValue = averageInstanceFactors[k].get(s);
					double templateValue = categoryFactors[k].get(s);
					double totalAvgValue = (instanceAvgValue + templateValue) / 2.0;

					// Add all the corresponding factors
					averageInstanceFactors[k].set(s, totalAvgValue);		
				}						
			}


			/*
			 * Store the updated factors
			 */
			dai::FactorGraph updatedTemplateFactors(averageInstanceFactors);
			updatedTemplateFactors.WriteToFile(templateCompats[i].fileName.c_str());	
		}
	}
}


void ObjectActionRecognizer::selectQuery(){
	/*
	 * For MAP-based queries, the suggested query is always at the top of
	 * the list because they are listed in descending order of probability
	 */
	currentQuery = queries[0];
	
}


std::vector<Query> ObjectActionRecognizer::getQueries() const {
	return queries;
}


Query ObjectActionRecognizer::getCurrentQuery() const {
	return currentQuery;
}


bool ObjectActionRecognizer::evaluate(const bool& wasSelected) {
	ObjectActionPair observedVars;
	bool intentionRecognized = false;
	
	if (wasSelected == true) {	
		/*
		 * Query Accepted
		 */
		if (currentQuery.type == FULL_QUERY) {
			/*
			 * If the user responds positively to a full query, which is a query involving
			 * an object and an action, we are done. There is no need to ask them any more
			 * questions. We simply 'perform' the chosen action on the chosen object.
			 *
			 */
			if (currentQuery.hasAction) {
				observedVars.actionIndex = currentQuery.actionIndex;					
			}
			if (currentQuery.hasObject) {
				observedVars.objectIndex = currentQuery.objectIndex;										
			}				
								
			intentionRecognized = true;			

		} else if (currentQuery.type == ACTION_QUERY) {
			/*
			 * If the user responds positively to an action query, which is a query 
			 * only involving an action, we prune the current query set to determine only
			 * those queries that involve the chosen action together with an object. We would 
			 * then suggest new queries which would now include the objects that afford the 
			 * selected action.
			 */
			std::vector<Query> pruned;
			for (size_t i = 0; i < queries.size(); i++) {
				if (queries[i].hasObject && queries[i].hasAction && queries[i].actionIndex == currentQuery.actionIndex) {
					pruned.push_back(queries[i]);
				}
			}
			
			queries.clear();
			queries = pruned;
			currentQuery = queries[0];

			if (currentQuery.hasAction) {
				observedVars.actionIndex = currentQuery.actionIndex;
			}			

		} else if (currentQuery.type == OBJECT_QUERY) {
			/*
			 * If the user responds positively to an object query, which is a query 
			 * only involving an object, we prune the current query set to determine only
			 * those queries that involve the chosen object together with an action. We would 
			 * then suggest new queries which would now include the actions that the selected
			 * object affords.
			 */
			std::vector<Query> pruned;
			for (size_t i = 0; i < queries.size(); i++) {
				if (queries[i].hasAction && queries[i].hasObject && queries[i].objectIndex == currentQuery.objectIndex) {
					pruned.push_back(queries[i]);
				}
			}
			queries.clear();
			queries = pruned;
			currentQuery = queries[0];

			if (currentQuery.hasObject) {
				observedVars.objectIndex = currentQuery.objectIndex;										
			}			
		}

	} else {
		/*
		 * Query Rejected
		 */
		if (currentQuery.type == FULL_QUERY) {
			/*
			 * Remove the rejected full query from the query list as well as orphaned
			 * object or action queries as a result of the rejection of this full query
			 */
			std::vector<Query> pruned;
			std::vector<int> orphans;

			for (size_t i = 0; i < queries.size(); i++) {
				if (queries[i].index != currentQuery.index) {
					if (queries[i].type == OBJECT_QUERY && queries[i].objectIndex == currentQuery.objectIndex) {							
						orphans.push_back(queries[i].index);
					}
					else if (queries[i].type == ACTION_QUERY && queries[i].actionIndex == currentQuery.actionIndex) {
						orphans.push_back(queries[i].index);
					}
				}
			}
			for (size_t j = 0; j < orphans.size(); j++) {
				for (size_t k = 0; k < queries.size(); k++) {
					if (queries[k].index == orphans[j]) {
						queries.erase(queries.begin() + k);
						break;
					}
				}
			}
				
			for (size_t i = 0; i < queries.size(); i++) {
				if (queries[i].index != currentQuery.index) {						
					pruned.push_back(queries[i]);
				}
			}
			queries.clear();
			queries = pruned;
			currentQuery = queries[0];

			if (currentQuery.hasAction) {
				observedVars.actionIndex = currentQuery.actionIndex;										
			}
			
			if (currentQuery.hasObject) {
				observedVars.objectIndex = currentQuery.objectIndex;										
			}
						

		} else if (currentQuery.type == ACTION_QUERY) {
			/*
			 * Remove the rejected action query, along with any other queries involving
			 * the action.
			 */
			std::vector<Query> pruned;
			for (size_t i = 0; i < queries.size(); i++) {
				if (queries[i].actionIndex != currentQuery.actionIndex) {
					pruned.push_back(queries[i]);
				}
			}
				
			queries.clear();
			queries = pruned;
			currentQuery = queries[0];
			

		} else if (currentQuery.type == OBJECT_QUERY) {
			/*
			 * Remove the rejected object query, along with any other queries involving
			 * the object.
			 */
			std::vector<Query> pruned;
			for (size_t i = 0; i < queries.size(); i++) {
				if (queries[i].objectIndex != currentQuery.objectIndex) {
					pruned.push_back(queries[i]);
				}
			}
				
			queries.clear();
			queries = pruned;
			currentQuery = queries[0];
			
		}
	}
	
	if (intentionRecognized == true) {
		size_t objTemplateIdx;
		size_t actTemplateIdx;

		/*
		 * Update the factor template that coincides with the recognized intention
		 */
		if (observedVars.objectIndex >= 0 && observedVars.actionIndex >= 0) {
			objTemplateIdx = objectTemplateIndex[observedVars.objectIndex];
			actTemplateIdx = actionTemplateIndex[observedVars.actionIndex];
			objectActionMap.updateMap(objTemplateIdx, actTemplateIdx);
		}		
	}

	return intentionRecognized;
}


void ObjectActionRecognizer::writeNetworkToFile() {
	theNetwork.WriteToFile("CurrentScene.fg");

	std::ofstream os;
	os.open("CurrentScene.dot");
	dai::GraphAL markov = theNetwork.MarkovGraph();		

	os << "graph GraphAL  {" << std::endl;
	os << "node[shape=circle,width=1.0,fixedsize=true];" << std::endl;
	for( size_t n = 0; n < markov.nrNodes(); n++ ) {
		os << "\t\"" << theNetwork.var(n).name() << "\";" << std::endl;
	}
	for( size_t n1 = 0; n1 < markov.nrNodes(); n1++ ) {
		BOOST_FOREACH( const dai::Neighbor &n2, markov.nb(n1) )
		if( n1 < n2 ) {
			os << "\t\"" << theNetwork.var(n1).name() << "\" -- \"" << theNetwork.var(n2).name() << "\";" << std::endl;
		}
	}
	os << "}" << std::endl;
}




} /* oar */
