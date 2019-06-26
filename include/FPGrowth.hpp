#pragma once

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
	vector<shared_ptr<FPTreeNode>> children;
};

struct branch {
	int branchCount;
	vector<string> itemList;
};

class FPGrowth {

private:
	string fileName;                                            //IN          - The name of the transactions csv file
	vector<vector<string> > transactions;                       //CALC & OUT  - All the transactions from the csv file
	map<string, int> frequencyOfItems;                          //CALC & OUT  - The frequency of the items
	shared_ptr<FPTreeNode> rootOfFPTree;                        //CALC & OUT  - The root of the fp tree
	map<string, vector<shared_ptr<FPTreeNode> > > headerTable;  //CALC & OUT  - The header table of the fp tree
	map<string, map<set<string>, int> > paternOfItems;          //CALC & OUT  - The freaquency pattern of the items
	int minSupportCount;                                        //CALC & CLAC - The minimum support count


	vector<vector<string> > conditionalPatternBase;             //CALC - It is use to hold the item's conditional pattern base
	map<string, int> frequencyOfItemsForCPB;                    //CALC - It is use to hold the frequency of items base on CPB
	shared_ptr<FPTreeNode> rootOfFPTreeForCPB;                  //CALC - it is use to hold the fp tree based on CPB


private:

	/*******************************************************************************
		DEFINITION:
			Read the transaction from the csv file.
		INPUT:
			string fileName - The csv file name.
		OUTPUT:
			vector<vector<int> > transactions - All the transactions in the csv file
	******************************************************************************/
	vector<vector<string> > readTransactions(string);


	/*******************************************************************************
		DEFINITION:
			Find the frequency of the items which are in the transactions.
		INPUT:
			vector<vector<int> > transactions - All the transactions in the csv file
		OUTPUT:
			map<string,int> frequencyOfItems - The freaquency of the items in transactions
	******************************************************************************/
	map<string, int> itemsFrequency(vector<vector<string> >);


	/*******************************************************************************
		DEFINITION:
			Sort the transactions according to frequency.
		INPUT:
			vector<vector<int> > transactions - All the transactions in the csv file
			map<string,int> frequencyOfItems  - The freaquency of the items in transactions
		OUTPUT:
			vector<vector<int> > transactions - Whole csv file of transactions after being sorted
	******************************************************************************/
	vector<vector<string> > sortRecordsByFrequency(vector<vector<string> >, map<string, int>);


	/*******************************************************************************
		DEFINITION:
			Build the fp tree.
		INPUT:
			vector<vector<int> > transactions - Whole csv file of transactions after being sorted
		OUTPUT:
			shared_ptr<FPTreeNode> rootOfFPTree;                        - The root of the fp tree
			map<string, vector<shared_ptr<FPTreeNode> > > headerTable;  - The header table of the fp tree
	******************************************************************************/
	void buildFPTree(vector<vector<string> >, shared_ptr<FPTreeNode>);

	/*******************************************************************************
		DEFINITION:
			Find frequency pattern according to the fp tree
		INPUT:
			shared_ptr<FPTreeNode> rootOfFPTree;                        - The root of the fp tree
			map<string, vector<shared_ptr<FPTreeNode> > > headerTable;  - The header table of the fp tree
			string item name;                                           - Which items we want to find its frequency pattern
		OUTPUT:
			map<string, map<set<string>, int> > paternOfItems;          - The freaquency pattern of the items
		PROCESSING:
			vector<vector<int> > conditionalPatternBase;                - It is use to hold the item's conditional pattern base
			map<string, int> frequencyOfItemsForCPB;                    - It is use to hold the frequency of items base on CPB
			shared_ptr<FPTreeNode> rootOfFPTreeForCPB;                  - it is use to hold the fp tree based on CPB
	******************************************************************************/
	void findFrequencyPattern(string);


private:
	/*
		Some help function for creating a FP-tree and header-table
	*/

	/*******************************************************************************
		DEFINITION:
			Find if there is a child node in the node that is related to the given item
		INPUT:
			shared_ptr<FPTreeNode> root - the node that we want to find it's children node
			string itemName             - the item that we want to search for
		OUTPUT:
			shared_ptr<FPTreeNode>      - the children node that we want to find...
										  (If this value is null, then it means that
										   there is no children node related to the
										   item.)
	*******************************************************************************/
	shared_ptr<FPTreeNode> findChildrenNodeRelatedToItem(shared_ptr<FPTreeNode>, string);


	/*******************************************************************************
		DEFINITION:
			Add one transaction to the fp-tree
		INPUT:
			shared_ptr<FPTreeNode> root - the fp-tree root
			vector<string> transaction  - the transaction that we want to add
		OUTPUT:
			shared_ptr<FPTreeNode> root - the fp-tree root
	*******************************************************************************/
	shared_ptr<FPTreeNode> addOneTransaction(shared_ptr<FPTreeNode>, vector<string>);



	/*******************************************************************************
		DEFINITION:
			Add the node to the fp-tree
		INPUT:
			shared_ptr<FPTreeNode> root  - The root node
			string itemName              - the item's name
		OUTPUT:
			shared_ptr<FPTreeNode> child - The node that we just add
	*******************************************************************************/
	shared_ptr<FPTreeNode> addChildNode(shared_ptr<FPTreeNode>, string);



	/*******************************************************************************
		DEFINITION:
			Add item into header table,(while we add transaction into fp-tree)
		INPUT:
			map<string, vector<shared_ptr<FPTreeNode> > > headerTable - The header table
			string itemName                                           - The item name
		OUTPUT:
			map<string, vector<shared_ptr<FPTreeNode> > > headerTable - The header table
	*******************************************************************************/
	map<string, vector<shared_ptr<FPTreeNode> > >
		addItemToHeaderTable(map<string, vector<shared_ptr<FPTreeNode> > > headerTable,
			shared_ptr<FPTreeNode> itemNode,
			string itemName);

private:
	/* About traverse the fp-tree*/





public:

	FPGrowth(string);



};