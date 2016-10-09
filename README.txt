Each of the 3 requires programs for the 3 different methods uses node.h/.cpp and dList.h/.cpp
When compiling, I used the command:
	g++ main*SortRoutine*.cpp node.cpp dList.cpp
where *SortRoutine* is either Merge Sort, Quick Sort or QuickInsertionsort

Edit: This was a class project to write programs utilizing three different sort algorithms.

One is strictly Merge Sort, the other is strictly Quick Sort and the third is Quick Sort for
inputs with n > 20 and Insertion Sort for inputs with n <= 20