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
 *	\file ObjectActionMap.h
 *	\brief Defines the object-action map data structure
 *	\author	Kester Duncan
 */
#ifndef OBJECT_ACTION_MAP_H_
#define OBJECT_ACTION_MAP_H_


#include <cstdlib>
#include <dai/prob.h>


/**
 * \brief Namespace that encapsulates all of the functions and types relevant for human intention recognition
 */
namespace oar {


/// Represents the relationship between an object and an action
struct ObjectActionProperty {
	/// Object's index
	size_t objectIdx;

	/// Object's name
	std::string objectName;

	/// Action's index
	size_t actionIdx;

	/// Action's name
	std::string actionName;

	/// Action-Object Factor compatibilities
	dai::Prob factor;

	/// Default constructor
	ObjectActionProperty () : objectIdx(0), objectName(""), actionIdx(0), actionName(""),  factor(4) { }

	/// Copy constructor
	ObjectActionProperty (const ObjectActionProperty &other) {
		objectIdx = other.objectIdx;
		objectName = other.objectName;
		actionIdx = other.actionIdx;
		actionName = other.actionName;
		factor = other.factor;
	}

	/// Overloaded assignment operator
	ObjectActionProperty& operator= (const ObjectActionProperty& other) {
		objectIdx = other.objectIdx;
		objectName = other.objectName;
		actionIdx = other.actionIdx;
		actionName = other.actionName;
		factor = other.factor;

		return *this;
	}

	/// Gets the factor value coinciding with the index \c idx provided
	double operator() (const size_t& idx) {
		return factor.get(idx);
	}

	/// Sets the factor value at index \c idx to the value \c value
	void operator() (const size_t& idx, const double& value) {
		factor.set(idx, value);
	}
};


/**
 * \brief Template map of all possible object and action relationships and their properties
 */
class ObjectActionMap {
private:
	/// Number of possible objects
	static const unsigned int NUM_OBJECTS = 11;

	/// Number of possible actions
	static const unsigned int NUM_ACTIONS = 7;

	/// Learning rate \f$ \lambda \f$
	double lambda;

	/// Template factor map that is updated based on observations
	ObjectActionProperty map [NUM_OBJECTS][NUM_ACTIONS];
		

public:
	/// Default constructor
	ObjectActionMap () : lambda(1.0) {}
	
	/// Access the template object-action property at the location specified 
	ObjectActionProperty operator() (const size_t& oIdx, const size_t& aIdx) {
		return map[oIdx][aIdx];
	}
	
	/// Reads an object-action map from file
	void readMap (const std::string& fileName = "ObjectActionMap.map");

	/// Writes an object-action map to file
	void writeMap (const std::string& fileName = "ObjectActionMap.map");

	/// Updates the object-action map template factors according to observations
	void updateMap (const size_t& oIdx, const size_t& aIdx);

	/// Reset map to default probabilities
	void resetToDefault();

	/// Get the learning rate 
	double getLambda() const;

	/// Set the learning rate
	void setLambda(const double& learningRate);

	/// Get the number of objects
	unsigned int getNumOfObjects() {
		return NUM_OBJECTS;
	}

	/// Get the number of actions
	unsigned int getNumOfActions() {
		return NUM_ACTIONS;
	}

};

} // oar

#endif
