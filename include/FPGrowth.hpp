#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <fstream>
#include <algorithm>

using namespace std;

struct FPTreeNode {
	string itemName;
	int count;
	shared_ptr<FPTreeNode> parent;
	vector<shared_ptr<FPTreeNode> > children;
};


struct CPB_Node {
int count;
vector<string> branch;
shared_ptr<CPB_Node> next;
};

 
struct FreqPatternStruct {
	int count;
	set<string> pattern;
};


 

class FPGrowth 
{

	private:
		string fileName;                                            //IN          - The name of the transactions csv file
		vector<vector<string> > transactions;                       //CALC & OUT  - All the transactions from the csv file
		map<string, int> frequencyOfItems;                          //CALC & OUT  - The frequency of the items
		shared_ptr<FPTreeNode> rootOfFPTree;                        //CALC & OUT  - The root of the fp tree
		map<string, vector<shared_ptr<FPTreeNode> > > headerTable;  //CALC & OUT  - The header table of the fp tree
		vector<FreqPatternStruct> frequencyPattern;                 //CALC & OUT  - The freaquency pattern of the items
		int minSupportCount;                                        //CALC & CLAC - The minimum support count



	private:


		vector<vector<string> > readTransactions(string);
		map<string, int> itemsFrequency(vector<vector<string> >);
		vector<vector<string> > sortRecordsByFrequency(vector<vector<string> >, map<string, int>);
		void buildFPTree(vector<vector<string> >, shared_ptr<FPTreeNode>&, map<string, vector<shared_ptr<FPTreeNode> > >&);
		void findFrequencyPattern(string, vector<FreqPatternStruct>&);


	private:
		/*
			Some help function for creating a FP-tree and header-table
		*/
		shared_ptr<FPTreeNode> findChildrenNodeRelatedToItem(shared_ptr<FPTreeNode>, string);
		shared_ptr<FPTreeNode> addOneTransaction(shared_ptr<FPTreeNode> &, vector<string>, map<string, vector<shared_ptr<FPTreeNode> > >&);
		shared_ptr<FPTreeNode> addChildNode(shared_ptr<FPTreeNode>&, string);
		map<string, vector<shared_ptr<FPTreeNode> > >
			addItemToHeaderTable(map<string, vector<shared_ptr<FPTreeNode> > > headerTable,
				shared_ptr<FPTreeNode> itemNode,
				string itemName);


	private:
		/*
			This function is about creating conditional pattern base
		*/
		void addCPB_Node(shared_ptr<CPB_Node>&, int, vector<string>);
		void crusingBack(shared_ptr<FPTreeNode>, vector<string>&);
		void setMinSupportForCPB(vector<vector<string> >&, map<string,int>);


		void getFrequencyPattern(string, vector<FreqPatternStruct> &, map<string, vector<shared_ptr<FPTreeNode> > >);
		void findPatternOutter(string, vector<FreqPatternStruct> &, shared_ptr<FPTreeNode> node);
		void findPatternInner(string,
							  shared_ptr<FPTreeNode> node,
			                  vector<FreqPatternStruct> &freqPattern) ;


	public:

		FPGrowth(string,int);
		void setMinSupportCount(int);

		/*******************************************************************************
		 *	These function is for testing reason		
		******************************************************************************/
		void displayTransactions()const;
		void displayFrequencyOfItem(map<string, int>)const;
		void displayNode(shared_ptr<FPTreeNode>)const;
		void displayCrusingBack(shared_ptr<FPTreeNode>)const;
		void displayHeaderTable(string, map<string, vector<shared_ptr<FPTreeNode> > >)const;
		void displayCPB(shared_ptr<CPB_Node>&)const;
		void displayFreqPattern()const;



};