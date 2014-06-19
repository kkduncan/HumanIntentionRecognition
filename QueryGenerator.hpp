/*
 *	This file part of HIP (Human Intention Prediction)
 *	Copyright (c) 2013, Kester Duncan
 *
 */
/// \file HIPQueryGeneration.h
/// \brief Generates human-readable queries based on the object-action interactions of a Markov Network

#ifndef __HIPQUERYGENERATOR_H__
#define __HIPQUERYGENERATOR_H__

#include <cstdlib>
#include <cassert>
#include <ctime>
#include <exception>
#include <dai/factorgraph.h>
#include <dai/varset.h>

#include "Query.hpp"
#include "OARTypes.h"


namespace oar {

/// Represents a scoring type to associate node indices to marginal probabilities
typedef std::pair<size_t, double> NodeProbabilityPair;


/**
 * \brief Comparator class used for sorting Node Probability Pairs
 */
struct ProbComp {
	bool operator() (const NodeProbabilityPair& lhs, const NodeProbabilityPair& rhs) const {
		double epsilon = 0.0000001;
		return ((rhs.second - lhs.second) < epsilon);
	}
};


/**
 * \brief Comparator class used for sorting Query vectors
 */
struct QueryComp {
	bool operator() (const Query& lhs, const Query& rhs) const {
		srand(static_cast<unsigned int>(time(NULL)));
		bool shouldComeFirst = false;
		double epsilon = 0.00000001;

		if ((lhs.score - rhs.score) > epsilon){
			shouldComeFirst = true;

		} else if (fabs(lhs.score - rhs.score) < epsilon) {
			if (lhs.type == FULL_QUERY) {
				shouldComeFirst = true;

			} else if ((lhs.type == ACTION_QUERY || lhs.type == OBJECT_QUERY) &&
						(rhs.type == ACTION_QUERY || rhs.type == OBJECT_QUERY)) {
				int coinFlip = rand() % 1;
				
				if (coinFlip == 0) {
					shouldComeFirst = true;

				} else if (coinFlip == 1) {
					shouldComeFirst = false;

				}
			} else {
				shouldComeFirst = false;
			}
		}

		return shouldComeFirst;
	}
};


/**
 * \brief Generates queries based on the inherent probabilities of nodes and 
 * factors of an Object-Action Intention Network (Markov Network)
 */
struct QueryGenerator {
	dai::FactorGraph* network;
	dai::Factor distribution;
	dai::InfAlg* inferenceAlgo;
	std::vector<NodeProbabilityPair> objects;
	std::vector<NodeProbabilityPair> actions;
	std::vector<NodeProbabilityPair> relations;
	std::vector<Query> queries;
	
	/// Stores the names of all objects in the scene
	NameMap objectNames;

	/// Stores the names of all the actions in the scene
	NameMap actionNames;

	/// Stores the properties of all nodes of the graph
	NodePropertiesList allNodes;


	QueryGenerator() : network(0), inferenceAlgo(0) {}

	QueryGenerator(dai::FactorGraph& net) {
		init(net);
	}

	~QueryGenerator() {
		cleanUp();
	}

	void setObjectNames(const NameMap& objNames) {
		this->objectNames = objNames;
	}

	void setActionNames(const NameMap& actNames) {
		this->actionNames = actNames;
	}

	void setAllNodes(const NodePropertiesList& otherAllNodes) {
		this->allNodes = otherAllNodes;
	}

	/// Instantiate the generator
	void init (dai::FactorGraph& net) {
		network = net.clone();

		dai::PropertySet infProps;
		infProps.set("tol", 0.00001);
		infProps.set("logdomain", true);
		infProps.set("updates", std::string("SEQMAX"));
		infProps.set("inference", std::string("MAXPROD"));
		
		inferenceAlgo = newInfAlg("BP", *network, infProps );
		inferenceAlgo->init();
		inferenceAlgo->run();		
		getMarginalProbabilities();		

	}

	/// Clean up
	void cleanUp () {
		if (network) {
			delete network;
		}	

		if (inferenceAlgo) {
			delete inferenceAlgo;
		}
	}


	/**	 
	 * Gets the marginal probabilities of object and action nodes and their factors. 
	 */
	void getMarginalProbabilities() {
		std::vector<dai::Var> nodes = network->vars();
		std::vector<dai::Factor> factors = network->factors();

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
		
		//std::sort(actions.begin(), actions.end(), ProbComp());
		//std::sort(objects.begin(), objects.end(), ProbComp());		
		
		for (size_t k = 0; k < factors.size(); k++) {
			std::vector<dai::Var> vars = factors[k].vars().elements();
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
		//std::sort(relations.begin(), relations.end(), ProbComp());
	}


	/**	 
	 * Gets the joint probabilities of object and action nodesand their factors. 
	 * /note does not work at the moment 
	 */
	// FIXME fix or eliminate this
	void getJointProbabilities() {
		std::vector<dai::Var> nodes = network->vars();
		std::vector<dai::Factor> factors = network->factors();
		dai::VarSet actionVariables;
		dai::VarSet objectVariables;

		actions.clear();
		objects.clear();
		relations.clear();

		for (size_t i = 0; i < nodes.size(); i++) {
			if (nodes[i].type() == dai::ACTION) {
				actionVariables.insert(nodes[i]);									
				
			} else if (nodes[i].type() == dai::OBJECT) {
				objectVariables.insert(nodes[i]);				
			}
		}

		for (size_t i = 0; i < nodes.size(); i++) {
			if (nodes[i].type() == dai::ACTION) {
				std::map<dai::Var, size_t> states;
				size_t linearState;
				
				for (dai::VarSet::const_iterator v = actionVariables.begin(); v != actionVariables.end(); v++) {
					dai::Var var = *v;

					if (nodes[i].label() == var.label()) {
						states[var] = 1;
					} else {
						states[var] = 0;
					}
				}

				linearState = calcLinearState(actionVariables, states);
				
				double value = 0.5;
				NodeProbabilityPair s;
				s.first = i;

				try {
					value = inferenceAlgo->belief(actionVariables)[linearState];
				} catch (std::exception& e) {
					std::cout << "Exception getting action marginals: \n";
					std::cout << e.what();
				}
				
				s.second = value;				
				actions.push_back(s);				

			} else if (nodes[i].type() == dai::OBJECT) {
				std::map<dai::Var, size_t> states;
				size_t linearState;

				for (dai::VarSet::const_iterator v = objectVariables.begin(); v != objectVariables.end(); v++) {
					dai::Var var = *v;

					if (nodes[i].label() == var.label()) {
						states[var] = 1;
					} else {
						states[var] = 0;
					}
				}

				linearState = calcLinearState(objectVariables, states);
				
				double value = 0.5;
				NodeProbabilityPair s;
				s.first = i;

				try {
					value = inferenceAlgo->belief(objectVariables)[linearState];
				} catch (std::exception& e) {
					std::cout << "Exception getting object marginals: \n";
					std::cout << e.what();
				}

				s.second = value;				
				objects.push_back(s);

			}
		}	
		
		for (size_t k = 0; k < factors.size(); k++) {
			std::vector<dai::Var> vars = factors[k].vars().elements();
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


	/**
	 * \brief Generate the query set
	 */
	void generateQuerySet(bool sorted = true) {
		int queryIdx = 0;
		queries.clear();

		//int objectIndex = 0; // Needed to access the 'objects' vector

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
			//objectIndex++;
			queryIdx++;
			
		}				

		// Add the <object-action> queries to the set
		for (size_t i = 0; i < relations.size(); i++) {
			std::string objName, actionName;
			size_t objIdx, actionIdx;
			size_t factorIdx = relations[i].first;
			std::vector<dai::Var> nodes = network->factors()[factorIdx].vars().elements();

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


		// Sort queries by score
		if (sorted) {
			std::sort(queries.begin(), queries.end(), QueryComp());			
		}

	}


	/**
	 * \brief Print the underlying query set to the output stream
	 */
	friend std::ostream& operator<<(std::ostream &os, const QueryGenerator& qs) {
		os << "\nQuery Set [" << qs.queries.size() << " queries]\n\n";

		for (size_t i = 0; i < qs.queries.size(); i++) {
			os << qs.queries[i].index << ":\t " << qs.queries[i].question << " [ " << qs.queries[i].score << " ]" <<  std::endl;
		}		
		return os;
	}
	


};


} /* hip */

#endif /* __HIPQUERYGENERATOR_H__ */
