/*
Name: William Brown
Class: CS3110 - Data Structures and Algorithms
Due Date: February 20, 2016
Assignment: Programming Assignment 2
*/
//node.cpp

#include "node.h"

node::node(string n, double a, int pc, double pd, int p){
	name = n;
	area = a;
	popcount = pc;
	popden = pd;
	pos = p;
}