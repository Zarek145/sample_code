/*
Name: William Brown
Class: CS3110 - Data Structures and Algorithms
Due Date: February 20, 2016
Assignment: Programming Assignment 2
*/
//mainQuicksort.cpp

#include "node.h"
#include "dList.h"
#include <iostream>//cin/cout
#include <string>//string
#include <cstring>//atof/atoi
#include <fstream>
#include <stdlib.h>
#include <algorithm>//swap
#include <ctime>//clock

using namespace std;

//medianOfThree and quicksort came straight from class notes
double medianOfThree(double* a, int left, int right){
	int center = (left + right)/2;
	
	if(a[center] < a[left])
		swap(a[left], a[center]);
	if(a[right] < a[left])
		swap(a[left], a[right]);
	if(a[right] < a[center])
		swap(a[center], a[right]);
	
	swap(a[center], a[right-1]);
	return a[right-1];
}

void quicksort(double* a, int left, int right){
	if(left < right){
	
		double pivot = medianOfThree(a, left, right);
		
		int i = left, j = right-1;
		while(true){
			while(a[++i] < pivot){}
			while(pivot < a[--j]){}
			if(i<j){
				swap(a[i], a[j]);
			}
			else
				break;
		}
		
		swap(a[i], a[right-1]);
				
		
		quicksort(a, left, i-1);	
		quicksort(a, i+1, right);
	}
}

int main(){
	string s;
	int popcount;
	double area, popden;
	string buffer;
	FILE* writefile;
	char c;
	string temp1, temp2, temp3, temp4;
	char* pEnd;
	dList* list = new dList();
	clock_t t;
	
	//Parse the file line by line
 	while(getline(cin, buffer)){
		int found1 = buffer.find('|');
		int found2 = buffer.find("|", found1 + 1, 1);
		int found3 = buffer.find("|", found2+1, 1);
		temp1 = buffer.substr(0, found1);
		temp2 = buffer.substr(found1+1, found2-found1-1);
		temp3 = buffer.substr(found2+1, found3-found2-1);
		temp4 = buffer.substr(found3+1, buffer.length()-found3);
		
		//name is the string leading to the first |
		s = temp1;
		
		//area is the double leading to the second |
		area = atof(temp2.c_str());
		
		//popcount is the integer leading to the third |
		popcount = atoi(temp3.c_str());
		
		//popden is the double proceeding the third |
		popden = atof(temp4.c_str());
		
		//insert these values into the list
		list->insert(s, area, popcount, popden);
	}
	
	//find the size of the list, this will dictate how big of an array you need to allocate
	int array_size = list->getCount();
	
	//create an array the size of the list
	double* a = new double[array_size];
	
	//fill the array with the double popden values from each list entry
	a = list->makeArray();

	//these three lines are the sort algorithm and a method to "time" the sort algorithm
	t = clock();
	quicksort(a, 0, array_size);
	t = clock() - t;

	//redirects stdout to the file indicated
	freopen("pop_density_sorted.txt", "w", stdout);
	
	//print the # of clicks and the list in the right order
	freopen("pop_density_sorted.txt", "w", stdout);
	
	cout << "It took " << t << " clicks to sort using QuickSort." << endl;
	for(int i = array_size; i >= 0; i--){
		list->printAssociatedEntry(a[i]);
	}
	
	fclose(stdout);
	
	return 0;
}