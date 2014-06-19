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
 *	\file ObjectActionCountMap.h
 *	\brief Maintains counts on the number of action and object pairs in a scene
 *	\author	Kester Duncan
 */
#ifndef OBJECT_ACTION_COUNT_MAP_H_
#define OBJECT_ACTION_COUNT_MAP_H_

#include <cstdlib>

/**
 * \brief Namespace that encapsulates all of the functions and types relevant for human intention recognition
 */
namespace oar {

/**
 * \brief Map for storing object and action counts for a scene
 */
class ObjectActionCountMap {
private:
	/// Number of possible objects
	static const unsigned int NUM_OBJECTS = 11;

	/// Number of possible actions
	static const unsigned int NUM_ACTIONS = 7;

	/// Count map
	size_t map[NUM_OBJECTS][NUM_ACTIONS];
		

public:
	/// Default Constructor
	ObjectActionCountMap() {
		for (size_t i = 0; i < NUM_OBJECTS; i++) {
			for (size_t j = 0; j < NUM_ACTIONS; j++) {
				map[i][j] = 0;
			}			
		}
	}

	~ObjectActionCountMap() {}

	/// Get the number of objects associated with the action at \c actionIdx
	size_t getActionCount(const size_t& actionIdx) {
		size_t sum = 0;
		for (size_t i = 0; i < NUM_OBJECTS; i++) {
			sum += map[i][actionIdx];
		}
		return sum;
	}

	/// Get the number of actions associated with the object at 
	size_t getObjectCount(const size_t& objectIdx) {
		size_t sum = 0;
		for (size_t i = 0; i < NUM_ACTIONS; i++) {
			sum += map[objectIdx][i];
		}
		return sum;
	}

	/// Get the total number of objects and action relationships
	size_t getTotalCount() {
		size_t sum = 0;
		for (size_t i = 0; i < NUM_OBJECTS; i++) {
			for (size_t j = 0; j < NUM_ACTIONS; j++) {
				sum += map[i][j];
			}			
		}
		return sum;
	}

	/// Get the probability of the object at \c objectIdx
	double getObjectProbability(const size_t& objectIdx) {
		double objectCount = static_cast<double>(getObjectCount(objectIdx));
		double totalCount = static_cast<double>(getTotalCount());

		return (objectCount / totalCount);
	}

	/// Get the probability of the action at \c actionIdx
	double getActionProbability(const size_t& actionIdx) {
		double actionCount = static_cast<double>(getActionCount(actionIdx));
		double totalCount = static_cast<double>(getTotalCount());

		return (actionCount / totalCount);
	}

	/// Update the map to indicate the presence of a particular object-action pair
	void operator() (const size_t& objectIdx, const size_t& actionIdx) {
		map[objectIdx][actionIdx] += 1;
	}
	

};

} // oar

#endif
