/*
Name: William Brown
Class: CS3110 - Data Structures and Algorithms
Due Date: February 20, 2016
Assignment: Programming Assignment 2
*/
//dList.cpp

#include "dList.h"
#include "node.h"

node* n;
node* curr;

using namespace std;

dList::dList(){
	head = tail = NULL;
	count = 0;
}

void dList::insert(string na, double a, int pc, double pd){
	if(count == 0){
		n = new node(na, a, pc, pd, count);
		n->next = n->prev = NULL;
		head = tail = n;
		curr = n;
		count++;
	}
	else{
		n = new node(na, a, pc, pd, count);
		curr->next = tail = n;
		curr = n;
		count++;
	}
}

int dList::getCount(){
	return count;
}

void dList::print(){
	curr = head;
	cout.precision(12);
	for(int i = 0; i < count; i++){
		cout << "Name: " << curr->name << ", Area: " << fixed << curr->area << ", Pop. Count: " << curr->popcount << ", Pop. Den.: " << fixed << curr->popden << endl;
		curr = curr->next;
	}
	return;

}

void dList::printAssociatedEntry(double value){
	curr = head;
	int i = 0;
	cout.precision(10);
	while(true){
		if(curr->popden == value){
		cout << "Name: " << curr->name << ", Area: " << fixed << curr->area << ", Pop. Count: " << curr->popcount << ", Pop. Den.: " << fixed << curr->popden << endl;
		break;
		}
		else if(curr == tail)
			break;
		else
			curr = curr->next;
	}
}

double* dList::makeArray(){
	double* a = new double[count];
	curr = head;
	for(int i = 0; i < count; i++){
		a[i] = curr->popden;
		curr = curr->next;
	}
	return a;
}
