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
 *	\file Query.hpp
 *	\brief Data structure to store object-action queries
 *	\author	Kester Duncan
 */
#ifndef __QUERY_H__
#define __QUERY_H__

#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include <dai/varset.h>


namespace oar {


/**
 * \brief Type of query
 *
 * A query can involve an action only, an object only or both
 */
enum QueryType {
	ACTION_QUERY,
	OBJECT_QUERY,
	FULL_QUERY,
	UNKNOWN
};


/**
 * \brief Represents a query that is used for intention prediction
 */
struct Query {
	int index;					///< The index of this query
	int objectIndex;			///< The object index in the graph
	int actionIndex;			///< The action index in the graph
	std::string objectName;		///< The name of this object, which is calculated
	std::string actionName;		///< The name of this action, which is calculated
	std::string question;		///< The human-readable question that would be posed to the user
	bool hasObject;				///< Indicates whether this query involves an object
	bool hasAction;				///< Indicates whether this query involves an action
	bool suggested;				///< Indicates whether this query was proposed to the user or not
	bool accepted;				///< Indicates whether this query was accepted
	double score;				///< Marginal probability of this query
	QueryType type;				///< Indicate the type of query
	dai::VarSet nodes;			///< Variables involved in this query


	/**
	 * \brief Default Constructor
	 */
	Query() : index(-1), objectIndex(-1), actionIndex(-1),
			objectName(), actionName(), question(),
			hasObject(false), hasAction(false), suggested(false), accepted(false),
			score(0.0), type(UNKNOWN), nodes() {}


	/**
	 * \brief Constructs a full query. 
	 */
	Query (int idx, double s, int objIdx = -1, int actionIdx = -1, std::string objName = "", std::string actName = "") {
		index = idx;
		score = s;
		objectIndex = objIdx;
		actionIndex = actionIdx;
		objectName = objName;
		actionName = actName;

		(objectIndex != -1) ? hasObject = true : hasObject = false;
		(actionIndex != -1) ? hasAction = true : hasAction = false;

		suggested = false;
		accepted = false;
		
		// Create the query based on the information provided
		create();
	}


	/**
	 * \brief Creates the human-readable query
	 */
	void create() {
		std::string actionPhrase = actionName;

		if (actionName.compare("Drink") == 0 || actionName.compare("Pour") == 0) {
			actionPhrase = actionName + " from";
		}

		if (hasObject && hasAction) {
			question = "Do you want to " + actionPhrase + " " + objectName + "?";		
		} 
		
		if (hasObject && !hasAction) {
			question = "Do you want to use the " + objectName + "?";		
		} 
		
		if (!hasObject && hasAction) {
			question = "Do you want to " + actionPhrase + " something?";
		}
	}


	/**
	 * \brief Prints the underlying query to the output stream
	 */
	friend std::ostream& operator<<(std::ostream &os, const Query& q) {
		os << "\n" << q.question << "\n";		
		return os;
	}

};



/**
 * \brief Comparator class used for sorting Query vectors based on probabilities
 */
struct QueryComparator {
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
				int coinFlip = rand() % 2;
				
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
 * \brief Comparator class used for sorting Query vectors by action, then object, then pairs
 */
struct CountsQueryComparator {
	bool operator() (const Query& lhs, const Query& rhs) const {
		bool shouldComeFirst = false;
		double epsilon = 0.00000001;
		if (lhs.type == rhs.type) {
			if ((lhs.score - rhs.score) > epsilon) {
				shouldComeFirst = true;
			} else {
				shouldComeFirst = false;
			}
		
		} else if (lhs.type == ACTION_QUERY) {
			shouldComeFirst = true;

		} else if (lhs.type == OBJECT_QUERY) {
			if (rhs.type == ACTION_QUERY) {
				shouldComeFirst = false;

			} else if (rhs.type == FULL_QUERY) {
				shouldComeFirst = true;

			}
			
		} else if (lhs.type == FULL_QUERY) {
			if (rhs.type == ACTION_QUERY) {
				shouldComeFirst = false;

			} else if (rhs.type == OBJECT_QUERY) {
				shouldComeFirst = false;
			}			
		}		

		return shouldComeFirst;
	}
};


} /* oar */

#endif /* __QUERY_H__ */
