/*
Name: William Brown
Class: CS3110 - Data Structures and Algorithms
Due Date: February 20, 2016
*/
//node.h

#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <string>

using namespace std;

class node{
	friend class dList;
	private:
		int pos;
		string name;
		double area, popden;
		int popcount;
		node* next;
		node* prev;
	public:
		node(string n, double a, int pc, double pd, int pos);
};

#endif