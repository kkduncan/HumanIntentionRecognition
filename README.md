Scene-Dependent Human Intention Recognition
===========================================

This is a human ntention recognition framework that is appropriate
for persons with limited physical capabilities that determines and
learns human intentions based on scene objects and the actions
that can be performed on them, as well as past interaction history.
This framework is based on a Markov model formulation entitled
Object-Action Intention Networks.

NOTE
====
This code is dependent on a slightly modified version of libdai (Library for Discrete Approximate Inference - https://staff.fnwi.uva.nl/j.m.mooij/libDAI/)


SYNOPSIS
========
Given a list of object categories present in a scene and their features (in this case, distance from the camera), this code determines
the desired object and action of the user through rounds of interactions (sessions). This is done via a formulation of Markov Networks 
entitled Object-Action Intention Networks which are initialized using a file containing probability values (ObjectActionMap.map).
When the user makes a selection, these values are updated in an effort to learn the user's preferences over time.


