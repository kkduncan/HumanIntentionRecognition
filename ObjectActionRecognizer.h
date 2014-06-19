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
 *	\file ObjectActionRecognizer.h
 *	\brief Administers the full object-action recognition process
 *	\author	Kester Duncan
 */
#ifndef OBJECT_ACTION_RECOGNIZER_H_
#define OBJECT_ACTION_RECOGNIZER_H_


#include <cstdlib>
#include <dai/daialg.h>
#include "OARTypes.h"
#include "Query.hpp"
#include "ObjectActionMap.h"
#include "ObjectActionCountMap.hpp"



/**
 * \brief Namespace that encapsulates all of the functions and types relevant for object
 * action recognition / human intention recognition
 */
namespace oar {


/**
 * \brief Class to perform scene-dependent human intention recognition
 * 
 * Determines which object that is most likely to be manipulated and the
 * action most likely to be perform on the object using a graphical model.
 *
 * This
 */
class ObjectActionRecognizer {

public:
	/// Constructs using the provided template map file name with the specified learning rate or the default values
	ObjectActionRecognizer(const std::string& oaMapName = "ObjectActionMap.map", const double& learningRate = 1.0f);
	
	/// Destructor
	~ObjectActionRecognizer();

	/// Reinitialize variables and data
	void reinitialize();


	/**
	 * \name Construction
	 * @{
	 */

	/**
	 * \brief Constructs an object-action-sensitive graph based on a scene object list
	 * \ingroup Construction
	 */
	void constructNetwork(const ObjectDistanceMap& sceneObjects, const bool& useCounts = false);

	/**
	 * \brief Generate a query set based on Markov Networks and Recursive Bayesian Learning information
	 */
	void generateMarkovBasedQuerySet();

	/**
	 * \brief Generate a query set based on frequency counts of current objects and actions in the scene
	 */
	void generateCountBasedQuerySet();

	/**
	 * \brief Generate a random query set based on all possible objects and actions
	 */
	void generateRandomQuerySet();

	/**
	 * \brief Generate a random query set based on objects and actions given by the scene
	 */
	void generateRandomQuerySetBasedOnScene();


	/** @} */

	/**
	 * \name Interaction    
	 * @{
	 */ 
	
	/**
	 * \brief Select the most probable object and action in the form of a query
	 * \ingroup Interaction
	 */
	void selectQuery();

	/**
	 * \brief Gets the current query list
	 * \ingroup Interaction
	 */
	std::vector<Query> getQueries() const;

	/**
	 * \brief Get the highest probable query
	 * \ingroup Interaction
	 */
	Query getCurrentQuery() const;

	/**
	 * \brief Evaluates the choice a user makes
	 * \ingroup Interaction
	 */
	bool evaluate(const bool& wasSelected);

	/** @} */


	/**
	 * \name Learning
	 * @{
	 */
	
	/**
	 * \brief Performs a pure update of template compatibilities with a learning rate where applicable
	 * using Bayesian Incremental Learning
	 * \ingroup Learning
	 */
	void writeTemplates();

	/**
	 * \brief Reset templates to default probabilities
	 * \ingroup Learning
	 */
	void resetTemplates();

	/**
	 * \brief Updates template compatibilities based on observed information from the Object-Action Intention Network
	 */
	void updateTemplatesUsingObservations();
	
	/** @} */



	/**
	 * \brief Write network to file
	 */
	void writeNetworkToFile();

	/**
	 * \brief Return a copy of the constructed network.
	 */
	dai::FactorGraph getNetwork() const;
	

private:
	/**
	 * \name Book Keeping 
	 * @{
	 */

	/// Object-Action Compatibility Templates 
	/// \ingroup Book Keeping
	ObjectTemplatePropertiesList templateCompats;

	/// Stores the names of all objects in the scene
	/// \ingroup Book Keeping
	NameMap objectNames;

	/// Stores the names of all the actions in the scene
	/// \ingroup Book Keeping
	NameMap actionNames;

	/// Lists object nodes along with their marginal probabilities
	/// \ingroup Book Keeping
	NodeProbabilityList objects;

	/// Lists action nodes along with their marginal probabilities
	/// \ingroup Book Keeping
	NodeProbabilityList actions;

	/// Lists object and action relationships nodes along with their marginal probabilities
	/// \ingroup Book Keeping
	NodeProbabilityList relations;

	/// Stores the properties of all nodes of the graph
	/// \ingroup Book Keeping
	NodePropertiesList allNodes;

	/// Stores all of the factors represented in the graph
	/// \ingroup Book Keeping
	FactorList allFactors;

	/// Stores the indices of instances of object categories
	/// \ingroup Book Keeping
	IndexList objectCategoryInstances;

	/// Stores the indices of the object-action factors that the scene object instances are involved in
	/// \ingroup Book Keeping
	ObjectFactorListMap objectActionFactors;

	/// Map an action's network node index to its template action index
	/// \ingroup Book Keeping
	ActionTemplateIndexMap actionTemplateIndex;

	/// Map an object's network node index to its template object index
	/// \ingroup Book Keeping
	ObjectTemplateIndexMap objectTemplateIndex;

	/// Map an object-action pair to their template potentials
	/// \ingroup Book Keeping
	ObjectActionMap objectActionMap;

	/// Maintain a count of scene object-action pairs
	/// \ingroup Book Keeping
	ObjectActionCountMap objectActionCountMap;

	/// File name for object-action map
	/// \ingroup Book Keeping
	std::string objectActionMapFileName;

	/// Counter for the number of variables nodes used in the current network
	/// \ingroup Book Keeping
	size_t nodeCount;

	/// Counter for the number of factors used in the current network
	/// \ingroup Book Keeping
	size_t factorCount;

	/// Index of last factor that was added to the network
	/// \ingroup Book Keeping
	size_t lastFactorIndex;

	/** @} */
	
	/**
	 * \name Network Administration
	 */
	/// The Object-Action Sensitive Network
	dai::FactorGraph theNetwork;

	/// The joint distribution of the object-action intention network represented by \c theNetwork
	dai::Factor distribution;

	/// The inference algorithm used for belief updating
	dai::InfAlg* inferenceAlgo;

	/// The current list of object-action queries for the network \c theNetwork
	std::vector<Query> queries;

	/// The current query being proposed to the user
	Query currentQuery;


	/// Clear variables and data
	void clean();

	/**
	 * \brief Initializes all template compatibilities for all object categories
	 * \remarks This function depends on the variable [templateCompats] being set
	 */
	void applyTemplates();

	/**
	 * \brief Create a node of the graph
	 */
	NetworkNode createGraphNode(const std::string& nodeName, const NodeType& type = dai::UNKNOWN, const size_t& indexInTemplate = 0);

	/**
	 * \brief Adds a newly created factor to the list of factors
	 */
	void addGraphFactor(FactorList& factorList, dai::Factor& newFactor);

	/**
	 * \brief Creates an object variable along with all its related factors
	 */
	FactorList instantiateObject(const std::string& objName, const bool& near, const double& distance);

	/**	 
	 * \brief Gets the marginal probabilities of object and action nodes and their factors. 
	 */
	void getMarginalProbabilities();	

	/**	 
	 * \brief Gets the  probabilities of object and action nodes and their factors based on scene content. 
	 */
	void getNumericalProbabilities();	


};


} /* oar */


#endif /* OBJECT_ACTION_RECOGNIZER_H_ */
