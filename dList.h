/*
Name: William Brown
Class: CS3110 - Data Structures and Algorithms
Due Date: February 20, 2016
Assignment: Programming Assignment 2
*/
//dList.h

#ifndef DLIST_H
#define DLIST_H

#include "node.h"
#include <string>

class dList{
	private:
		node* head;
		node* tail;
		int count;
	public:
		dList();
		void insert(std::string n, double a, int pc, double pd);
		void swap(int pos1, int pos2);
		int getCount();
		void print();
		double* makeArray();
		void printAssociatedEntry(double value);
};
#endif