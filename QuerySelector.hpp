/*
 *	This file part of HIP (Human Intention Prediction)
 *	Copyright (c) 2013, Kester Duncan
 *
 */
/// \file HIPQuerySelector.h
/// \brief Selects queries to present to a user based on a MAP / Mutual Information criterion

#ifndef __HIPQUERYSELECTOR_H__
#define __HIPQUERYSELECTOR_H__

#include "OARTypes.h"
#include "NetworkUpdater.hpp"
#include "Query.hpp"
#include "QueryGenerator.hpp"


namespace oar {

/// Selects queries to present to a user using inference
/** The queries are selected by either using the maximum a posteriori probability
 *	of the nodes involved in the query or by the mutual information of the nodes
 *	involved in the query.
 */
struct QuerySelector {
	dai::FactorGraph network;
	QueryGenerator generator;
	bool useMutualInfo;
	size_t currentIdx;
	Query currentQuery;


	QuerySelector(dai::FactorGraph& net, bool useMI = true) : network(net), useMutualInfo(useMI), currentIdx(0) {
		generator.init(network);		
	}

	void select(bool reset = false) {
		/*
		 * By regenerating the query set, we run the risk of having the same query being 
		 * asked over and over again in an infinite loop
		 */
		if (reset) {
			generator.cleanUp();
			generator.init(network);
			generator.generateQuerySet();
		}

		if (!useMutualInfo) {
			/*
			 * For MAP-based queries, the suggested query is always at the top of
			 * the list because they are listed in descending order of probability
			 */
			currentIdx = 0;	
			currentQuery = generator.queries[currentIdx];

		} else  {
			/*
			 * For mutual information-based queries, the suggested query is the
			 * one that has the maximum mutual information which would be at the
			 * top if the list is sorted accordingly
			 */
			std::map<size_t, size_t> observedVars;

			for (size_t i = 0; i < generator.queries.size(); i++) {
				Query q = generator.queries[i];
				
				if (q.hasAction) {
					observedVars[q.actionIndex] = 1;
				}

				if (q.hasObject) {
					observedVars[q.objectIndex] = 1;
				}
				double infoGain = 0.0;
				
				infoGain = NetworkUpdater::calculateInfoGain(*generator.network, observedVars);
				generator.queries[i].score = infoGain;

			}

			std::sort(generator.queries.begin(), generator.queries.end(), QueryComp());
			currentIdx = 0;
			currentQuery = generator.queries[currentIdx];

		}
	}


	/// Randomly select a query from the query set
	void randomlySelect(bool reset = false) {
		srand(static_cast<unsigned int>(time(NULL)));
		currentIdx = rand() % generator.queries.size();	

		if (currentIdx >= 0) {
			currentQuery = generator.queries[currentIdx];
		} else {
			printf("QuerySelector Warning :: Unable to select a valid random number!\n");
			currentQuery = generator.queries[0];
		}		
	}


	/// Evaluate the choice that the user makes in order to determine the suggest the next query
	bool evaluateChoice(bool answer, ObjectActionPair& observedVars = ObjectActionPair()) {
		bool selectionComplete = false;
		//std::map<size_t, size_t> observedVars;
				
		if (answer == true) {								
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
								
				selectionComplete = true;
				//NetworkUpdater::observe(*generator.network, observedVars);
				//NetworkUpdater::updateFactorGraph(*generator.network, observedVars);

			} else if (currentQuery.type == ACTION_QUERY) {
				/*
				 * If the user responds positively to an action query, which is a query 
				 * only involving an action, we prune the current query set to determine only
				 * those queries that involve the chosen action together with an object. We would 
				 * then suggest new queries which would now include the objects that afford the 
				 * selected action.
				 */
				std::vector<Query> pruned;
				for (size_t i = 0; i < generator.queries.size(); i++) {
					if (generator.queries[i].hasObject && generator.queries[i].hasAction && generator.queries[i].actionIndex == currentQuery.actionIndex) {
						pruned.push_back(generator.queries[i]);
					}
				}
				generator.queries.clear();
				generator.queries = pruned;
				currentQuery = generator.queries[0];

				if (currentQuery.hasAction) {
					observedVars.actionIndex = currentQuery.actionIndex;
				}

				//NetworkUpdater::observe(*generator.network, observedVars);

			} else if (currentQuery.type == OBJECT_QUERY) {
				/*
				 * If the user responds positively to an object query, which is a query 
				 * only involving an object, we prune the current query set to determine only
				 * those queries that involve the chosen object together with an action. We would 
				 * then suggest new queries which would now include the actions that the selected
				 * object affords.
				 */
				std::vector<Query> pruned;
				for (size_t i = 0; i < generator.queries.size(); i++) {
					if (generator.queries[i].hasAction && generator.queries[i].hasObject && generator.queries[i].objectIndex == currentQuery.objectIndex) {
						pruned.push_back(generator.queries[i]);
					}
				}
				generator.queries.clear();
				generator.queries = pruned;
				currentQuery = generator.queries[0];

				if (currentQuery.hasObject) {
					observedVars.objectIndex = currentQuery.objectIndex;										
				}

				//NetworkUpdater::observe(*generator.network, observedVars);
			}

		} else {
			/*
			 * With this design, the network is never updated based on a negative answer.
			 * FIXME: Should we observe negative feedback in the network???
			 */
			if (currentQuery.type == FULL_QUERY) {
				/*
				 * Remove the rejected full query from the query list as well as orphaned
				 * object or action queries as a result of the rejection of this full query
				 */
				std::vector<Query> pruned;
				std::vector<size_t> orphans;

				for (size_t i = 0; i < generator.queries.size(); i++) {
					if (generator.queries[i].index != currentQuery.index) {
						if (generator.queries[i].type == OBJECT_QUERY && generator.queries[i].objectIndex == currentQuery.objectIndex) {							
							orphans.push_back(generator.queries[i].index);
						}
						else if (generator.queries[i].type == ACTION_QUERY && generator.queries[i].actionIndex == currentQuery.actionIndex) {
							orphans.push_back(generator.queries[i].index);
						}
					}
				}
				for (size_t j = 0; j < orphans.size(); j++) {
					for (size_t k = 0; k < generator.queries.size(); k++) {
						if (generator.queries[k].index == orphans[j]) {
							generator.queries.erase(generator.queries.begin() + k);
							break;
						}
					}
				}
				
				for (size_t i = 0; i < generator.queries.size(); i++) {
					if (generator.queries[i].index != currentQuery.index) {						
						pruned.push_back(generator.queries[i]);
					}
				}
				generator.queries.clear();
				generator.queries = pruned;
				currentQuery = generator.queries[0];

				if (currentQuery.hasAction) {
					observedVars.actionIndex = currentQuery.actionIndex;										
				}
				if (currentQuery.hasObject) {
					observedVars.objectIndex = currentQuery.objectIndex;										
				}

				//NetworkUpdater::observe(*generator.network, observedVars);

			} else if (currentQuery.type == ACTION_QUERY) {
				/*
				 * Remove the rejected action query, along with any other queries involving
				 * the action.
				 */
				std::vector<Query> pruned;
				for (size_t i = 0; i < generator.queries.size(); i++) {
					if (/* generator.queries[i].hasAction && */ generator.queries[i].actionIndex != currentQuery.actionIndex) {
						//generator.queries.erase(generator.queries.begin() + i);
						pruned.push_back(generator.queries[i]);
					}
				}
				
				generator.queries.clear();
				generator.queries = pruned;
				currentQuery = generator.queries[0];

				//if (currentQuery.hasAction) {
				//	observedVars.actionIndex = currentQuery.actionIndex;										
				//}		

				//NetworkUpdater::observe(*generator.network, observedVars);

			} else if (currentQuery.type == OBJECT_QUERY) {
				/*
				 * Remove the rejected object query, along with any other queries involving
				 * the object.
				 */
				std::vector<Query> pruned;
				for (size_t i = 0; i < generator.queries.size(); i++) {
					if (/* generator.queries[i].hasObject && */ generator.queries[i].objectIndex != currentQuery.objectIndex) {
						//generator.queries.erase(generator.queries.begin() + i);
						pruned.push_back(generator.queries[i]);
					}
				}
				
				generator.queries.clear();
				generator.queries = pruned;
				currentQuery = generator.queries[0];

				//if (currentQuery.hasObject) {
				//	observedVars.objectIndex = currentQuery.objectIndex;										
				//}

				//NetworkUpdater::observe(*generator.network, observedVars);
			}

		}
		
		this->network = *generator.network->clone();
		//observedVars.clear();
		return (selectionComplete);
	}
	
};


} /* oar */


#endif /* __HIPQUERYSELECTOR_H__ */