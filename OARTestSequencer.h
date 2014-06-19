/**
 * Software License Agreement (BSD License)
 *
 *  Object Action Recognition
 *  Copyright (c) 2014, Kester Duncan
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
 *	\file OARTestSequencer.h
 *	\brief Generates simulated scenes of objects and their distances
 *	\author	Kester Duncan
 */
#ifndef OARTESTSEQUENCER_H_
#define OARTESTSEQUENCER_H_

#include "OARTypes.h"

namespace oar {

class OARTestSequencer {
public:
	/// Constructor
	OARTestSequencer();

	/// Destructor
	virtual ~OARTestSequencer();

	/**
	 * \brief Generate a fixed scene
	 */
	ObjectDistanceMap generateFixedScene();

	/**
	 * \brief Generate a scene with the same objects at random positions
	 */
	ObjectDistanceMap generateFixedObjectsRandomPositionScene(const std::vector<std::string>& fixedObjs);

	/**
	 * \brief Generate a mixed (a few fixed objects interspersed with random ones) scene
	 */
	ObjectDistanceMap generateMixedScene(const std::vector<std::string>& fixedObjs);

	/**
	 * \brief Randomly generate a complete scene with some fixed objects
	 */
	ObjectDistanceMap generateRandomScene(const std::vector<std::string>& fixedObjs);

	/**
	 * \brief Generate the full range of scenes
	 */
	ObjectDistanceMapList generateAndSaveAllSceneTypes(const int& numSessionsEach = 20);

	/**
	 * \brief Get list of scenes
	 */
	ObjectDistanceMapList getListOfScences() {
		return this->sceneList;
	}

	/**
	 * \brief Save map to file
	 */
	void saveListOfScenes(const std::string& fileName, const ObjectDistanceMapList& list);

	/**
	 * \brief Load a list of scenes from file
	 */
	void loadListOfScenes(const std::string& fileName);


private:
	/// List of different types of scenes
	ObjectDistanceMapList sceneList;


};


}

#endif /* OARTESTSEQUENCER_H_ */
