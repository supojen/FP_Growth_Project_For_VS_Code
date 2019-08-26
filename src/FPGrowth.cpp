#include "FPGrowth.hpp"


#include <fstream>
#include <sstream>



FPGrowth::FPGrowth(string fileName,int minSupportCount)
{
	this->fileName = fileName;
	this->minSupportCount = minSupportCount;


	//step 1 : get the input (get the transactions)
	transactions = readTransactions(fileName);
	//step 2 : find the items' frequency
	frequencyOfItems = itemsFrequency(transactions);
	//step 3 : reorder the transactions according to the frequency
	transactions = sortRecordsByFrequency(transactions, frequencyOfItems);
	//step 4 : build the frequency pattern tree
	buildFPTree(transactions, rootOfFPTree, this->headerTable);

	//step 5 : according to the frequency pattern tree, find the frequnecy pattern 
	findFrequencyPattern("E",this->frequencyPattern);

	
}

/*******************************************************************************
		DEFINITION:
			set min support count.
******************************************************************************/
void FPGrowth::setMinSupportCount(int minSupportCount)
{
	this->minSupportCount = minSupportCount;
}

/*******************************************************************************
		DEFINITION:
			Set the this->transactions.
		INPUT:
			vector<vector<string> > transactions - represeting the transactions
		OUTPUT:
			None
******************************************************************************/
void FPGrowth::getTransactionsByVectors(vector<vector<string> > transactions)
{
	this->transactions = transactions;
}

/*******************************************************************************
		DEFINITION:
			Read the transaction from the csv file.
		INPUT:
			string fileName - The csv file name.
		OUTPUT:
			vector<vector<int> > transactions - All the transactions in the csv file
******************************************************************************/
vector<vector<string> > FPGrowth::readTransactions(string fileName)
{

	fstream transFile;                     // CALC        - The file handler for CSV transaction file
	string transactionStr;                 // CALC        - The string which is used to hold the transaction 
	string item;                           // CALC        - The string whcih is used to hold the item in transaction
	vector<vector<string> > transactions;  // CALC & OUT  - This is the output for transactions


	/**************************************************************************
		PROCESSING - Bind the file to file stream
	**************************************************************************/
	transFile.open(fileName);


	/**************************************************************************
		PROCESSING - read all the transactions from the file and put it in a
					 container of vector.
					//These loop is for processing that getting the transaction
					 WHILE transFile.good() THEN
						CALC 1.Get string from file and assign it to transactionStr
							 2.Initialize the transStrStream with transactionStr
						//These loop is for processing that getting the items
						WHILE transStrStream.good() THEN
							CALC 1.Delimite the items from transStrStream
								 2.Push the items into tranaction vector
						END WHILE
							CALC 1.Push the transaction vector into transactions vector
					 END WHILE
	**************************************************************************/
	while (transFile.good())
	{

		istringstream transStrStream;  //CALC       - Use to delimitate the string, and abstract the items in transactions
		vector<string> transaction;    //CALC & OUT - All of the transactions 

		getline(transFile, transactionStr);
		transStrStream.str(transactionStr);

		while (transStrStream.good())
		{
			getline(transStrStream, item, ',');
			transaction.push_back(item);
		}

		transactions.push_back(transaction);
	}

	/**************************************************************************
	PROCESSING - release the resourse from the file stream
	**************************************************************************/
	transFile.close();

	return transactions;

}



/*******************************************************************************
		DEFINITION:
			Find the frequency of the items which are in the transactions.
		INPUT:
			vector<vector<int> > transactions - All the transactions in the csv file
		OUTPUT:
			map<string,int> frequencyOfItems - The freaquency of the items in transactions
******************************************************************************/
map<string, int> FPGrowth::itemsFrequency(vector<vector<string> > transactions)
{
	map<string, int> frequencyTable;

	for (auto &transaction : transactions)
	{
		for (auto &item : transaction)
		{
			frequencyTable[item]++;
		}
	}

	return  frequencyTable;
}


/*******************************************************************************
		DEFINITION:
			Sort the transactions according to frequency.
		INPUT:
			vector<vector<int> > transactions - All the transactions in the csv file
			map<string,int> frequencyOfItems  - The freaquency of the items in transactions
		OUTPUT:
			vector<vector<int> > transactions - Whole csv file of transactions after being sorted
	******************************************************************************/
vector<vector<string> > FPGrowth::sortRecordsByFrequency(vector<vector<string> > transactions, 
														 map<string, int> frequencyTable)
{

	/**************************************************************************
	 * 	Every transzction, we use sort the items according to their Frequency
	**************************************************************************/
	for (auto &transaction : transactions)
	{
		for(auto iter = transaction.begin(); iter < transaction.end(); iter++)
		{	
			for(auto iterToCmp = iter+1; iterToCmp < transaction.end(); iterToCmp++)
			{
				if( (frequencyTable[*iter] < frequencyTable[*iterToCmp]) ||   
					(frequencyTable[*iter] == frequencyTable[*iterToCmp] && *iter > *iterToCmp))
				{
					auto temp = *iter;
					*iter = *iterToCmp;
					*iterToCmp = temp;
				}
				

			}
		}
		
	}

	return transactions;
}






/*******************************************************************************
		DEFINITION:
			Build the fp tree.
		INPUT:
			vector<vector<int> > transactions - Whole csv file of transactions after being sorted
			bool HeaderTableOrLeafNodeTable   - if True,  then means we want to create headerTable
												if flase, then means we want to create leaf node table 
		OUTPUT:
			shared_ptr<FPTreeNode> rootOfFPTree;                        - The root of the fp tree
			map<string, vector<shared_ptr<FPTreeNode> > > headerTable;  - The header table of the fp tree
	******************************************************************************/
void FPGrowth::buildFPTree(
			vector<vector<string> > transactions,
			shared_ptr<FPTreeNode> &treeRoot,
			map<string, vector<shared_ptr<FPTreeNode> > >& header)
{

	treeRoot = make_shared<FPTreeNode>();

	for (auto &transaction : transactions)
	{
		addOneTransaction(treeRoot, transaction, header);
	}
	
}





/*******************************************************************************
	DEFINITION:
		Add one transaction to the fp-tree
	INPUT:
		shared_ptr<FPTreeNode> root 		- the fp-tree root
		vector<string> transaction  		- the transaction that we want to add
		bool HeaderTableOrLeafNodeTable     - if True,  then means we want to create headerTable
											  if flase, then means we want to create leaf node table
	OUTPUT:
		shared_ptr<FPTreeNode> root - the fp-tree root
*******************************************************************************/
void FPGrowth::addOneTransaction(
							shared_ptr<FPTreeNode> &root, 
							vector<string> transaction,
							map<string, vector<shared_ptr<FPTreeNode> > > & header)
{
	auto nodeIter = root;
	shared_ptr<FPTreeNode> childNodeForItem;

	for (auto &itemName : transaction)
	{
		childNodeForItem = findChildrenNodeRelatedToItem(nodeIter, itemName);
		if (childNodeForItem == NULL)
		{

			nodeIter = addChildNode(nodeIter, itemName);
		}
		else
		{
			childNodeForItem->count++;
			nodeIter = childNodeForItem;
		}

		//add the item node to header table
		header = addItemToHeaderTable(header, nodeIter, itemName);
		
	}

}

/*******************************************************************************
	DEFINITION:
		Find if there is a child node in the node that is related to the given item
	INPUT:
		shared_ptr<FPTreeNode> root - the node that we want to find it's children node
		string itemName             - the item that we want to search for
	OUTPUT:s
		shared_ptr<FPTreeNode>      - the children node that we want to find...
										(If this value is null, then it means that
										there is no children node related to the
										item.)
*******************************************************************************/
shared_ptr<FPTreeNode> FPGrowth::findChildrenNodeRelatedToItem(shared_ptr<FPTreeNode> root, string itemName)
{
	shared_ptr<FPTreeNode> nodeRealtedToItem; // return variable

	nodeRealtedToItem = NULL;

	for (auto &childNodeIter : root->children)
	{
		if (childNodeIter->itemName == itemName)
			nodeRealtedToItem = childNodeIter;
	}

	return nodeRealtedToItem;
}

/*******************************************************************************
	DEFINITION:
		Add the node to the fp-tree
	INPUT:
		shared_ptr<FPTreeNode> root  - The root node
		string itemName              - the item's name
	OUTPUT:
		shared_ptr<FPTreeNode> child - The node that we just add
*******************************************************************************/
shared_ptr<FPTreeNode> FPGrowth::addChildNode(shared_ptr<FPTreeNode>& root, string itemName)
{
	shared_ptr<FPTreeNode> child = make_shared<FPTreeNode>();

	child->itemName = itemName;
	child->count = 1;
	child->parent = root;

	root->children.push_back(child);

	return child;
}


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
FPGrowth::addItemToHeaderTable(
	map<string, vector<shared_ptr<FPTreeNode> > > headerTable,
	shared_ptr<FPTreeNode> itemNode,
	string itemName)
{
	bool alreadyInHeaderTable;

	//check if the node is already in the header table or not.
	alreadyInHeaderTable = false;
	for(auto itemNodeIter : headerTable[itemName])
	{
		if(itemNodeIter == itemNode)
			alreadyInHeaderTable = true;
	}

	//if the node is not exit in the header table, add it into it.
	if(!alreadyInHeaderTable)
		headerTable[itemName].push_back(itemNode);

	return headerTable;
}



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
void FPGrowth::findFrequencyPattern(string itemName, vector<FreqPatternStruct> &frequencyPattern)
{


	shared_ptr<CPB_Node> conditionalPatternBase;                  //CALC & CALC - The conditional pattern base
	vector<string> branch;                    					  //CALC & CALC - The branch of the pattern
	shared_ptr<CPB_Node> iterForCPB;                			  //CALC & CALC - This variable is used to iterate through all the partten in the Conditional Partten Base
	vector<vector<string> > branches;                             //CALC & CALC - All of the branches in the Conditional Partten Base
	map<string, int> itemFreqCPB;                                 //CALC & CALC - An frequncy table according to the Conditional Partten Base
	shared_ptr<FPTreeNode> FPTreeForCPB;                          //CALC & CALC - This is the little Frequncy Partten Tree related to a specific item
	map<string, vector<shared_ptr<FPTreeNode> > > leafNodeTable;  //CALC & CALC - This is used to make FPTree cruising back more convenient






	// Step1 : abtain the conditinal pattearn base
	for(auto iterNode : headerTable[itemName])
	{
		branch = crusingBack(iterNode);
		addCPB_Node(conditionalPatternBase, iterNode->count, branch);	
		branch.clear();	
	}

	// Step2 : find the freaquncy of items based on conditional pattern base
	// using the function --> map<string, int> itemsFrequency(vector<vector<string> >)

	// Step 2-1 : obtain all branch from conditionalPatternBase and store them in variable branches
	iterForCPB = conditionalPatternBase;
	while(iterForCPB != NULL)
	{
		for(int count = 0; count < iterForCPB->count; count++)
		{
			branches.push_back(iterForCPB->branch);
		}
		iterForCPB = iterForCPB->next;
	}
	
	// Step2-2  : find the freaquency!! come on 
	itemFreqCPB = itemsFrequency(branches);


	// Step 3 : use the following function to sort the branches with frequency
	//vector<vector<string> > sortRecordsByFrequency(vector<vector<string> >, map<string, int>);
	branches = sortRecordsByFrequency(branches,itemFreqCPB);

	
	// Step 4 : check if there are any item's frequency lower than minSupportCount
	setMinSupportForCPB(branches, itemFreqCPB);




	// Step 5 : build FPTree for CPB
	// using the function that
	// 	void FPGrowth::buildFPTree(
	//			vector<vector<string> > transactions,
	//			shared_ptr<FPTreeNode> &treeRoot,
	//			map<string, vector<shared_ptr<FPTreeNode> > > &header)
	buildFPTree(branches,FPTreeForCPB,leafNodeTable);



	//Step 6: get frequency pattern
	getFrequencyPattern(itemName,frequencyPattern,leafNodeTable);


	//************* testing block ****************//
	displayFreqPattern();
	//************* testing block ****************//
}


/*******************************************************************************
	DEFINITION:
		This function help us add the new node to the Conditaional pattern base.
	INPUT:
		int count - represeting in how many times the partten exist
		vector<string> branch - representing all the items in the pattern 
	OUTPUT:
		A Conditional partten base that conatain a new partten(branch + count)
******************************************************************************/
void FPGrowth::addCPB_Node(shared_ptr<CPB_Node> &node, int count, vector<string> branch)
{
	shared_ptr<CPB_Node> newNode = make_shared<CPB_Node>();

	newNode->count = count;
	newNode->branch = branch;

	if(node == NULL)
	{
		newNode->next = NULL;
	}
	else
	{
		newNode->next = node;
	}

	node = newNode;
}


/*******************************************************************************
	DEFINITION:
		This function crusing back from the fp tree node, and then store the 
		path to the branch.
	INPUT:
		shared_ptr<FPTreeNode> itemNode - representing the starting node that we want to 
										  cruising backs
	OUTPUT:
		vector<string> branch - this branch contain all the node that in the road when 
								 we crusiing back
******************************************************************************/
vector<string> FPGrowth::crusingBack(shared_ptr<FPTreeNode> itemNode)
{
	vector<string> branch;
	shared_ptr<FPTreeNode> iterNode;
	
	iterNode = itemNode->parent;
	
	while(iterNode != NULL && !iterNode->itemName.empty())
	{
		branch.push_back(iterNode->itemName);
		iterNode = iterNode->parent;
	}

	return branch;
}





/*******************************************************************************
	DEFINITION:
		This function get the frequency pattern form the CPB FP tree.
	OUTPUT:
		int countForPattern                  - representing the count for the frequency pattern
		vector<set<string> > patternsForItem - representing the frequncy patterns relating to item
******************************************************************************/
void FPGrowth::getFrequencyPattern(string itemName, 
						vector<FreqPatternStruct> & freqPattern, 
						map<string, vector<shared_ptr<FPTreeNode> > > header)
{
	for(auto &itemNode : header)
	{	
		for(auto &iterNode : itemNode.second)
		{	
			if(iterNode->children.empty())
			{								  
				findPatternOutter(itemName, freqPattern, iterNode);
			}
		}
	}
}




/*******************************************************************************
	DEFINITION:
		Find the frequncy pattern by one branch. 
		However after adding the pattern in the pattern array,
		we delete the leaf node and do it again
	INPUT:
		string itemName             - the item name which we want to obtained
		shared_ptr<FPTreeNode> node - the leaf node of the branch that we want to obtained the pattern
	OUTPUT:
		vector<FreqPatternStruct> &freqPattern  - the array of FreqPatternStruct that include the newly added pattern
******************************************************************************/
void FPGrowth::findPatternOutter(string itemName, 
							     vector<FreqPatternStruct> &freqPattern,
							     shared_ptr<FPTreeNode> node)
{
	shared_ptr<FPTreeNode> iterNode;
	
	iterNode = node;
	while(!iterNode->itemName.empty())
	{
		findPatternInner(itemName,iterNode,freqPattern);
		iterNode = iterNode->parent;
	}

}


/*******************************************************************************
	DEFINITION:
		Find the frequncy pattern by one branch
	INPUT:
		string itemName             - the item name which we want to obtained
		shared_ptr<FPTreeNode> node - the leaf node of the branch that we want to obtained the pattern
	OUTPUT:
		vector<FreqPatternStruct> &freqPattern  - the array of FreqPatternStruct that include the newly added pattern
******************************************************************************/
void FPGrowth::findPatternInner(string itemName,
								shared_ptr<FPTreeNode> node,
								vector<FreqPatternStruct> &freqPattern)
{
	FreqPatternStruct FPS;           // CALC & CALC - This is the temporary holder for the new pattrn
	set<string> patternHolder;       // CALC & CALC - This is used to hold the branch for the pattern we obtained
	shared_ptr<FPTreeNode> iterNode; // CALC & CALC - Used to iter through the little FPTree
	int countForPattern;             // CALC & CALC - This is used to hoild the count for the pattern
	bool isPatternExit;              // CALC & CALC - This is used to check if the pattern exist or not

	//Step 1: we need insert the item which we want to find its frequncy pattern(branch) in the pattern
	patternHolder.insert(itemName);

	//Step 2: Initialize some variable before the loop.
	//        Set the iterNode to the first node we want to iter(The leaf dnode)
	//        Set the pattern count to the leaf node's count
	iterNode = node;
	countForPattern = iterNode->count;
	//Step 3: crusing back from the leaf node, and do some process
	while(!iterNode->itemName.empty())
	{
		//insert all the item we go through into the pattern(branch)
		patternHolder.insert(iterNode->itemName);
		//whenever we insert one item into the pattern, we create a pattern for the item
		FPS.count = countForPattern;
		FPS.pattern = patternHolder;


		//check if the pattern already exist or not
		isPatternExit = false;
		for(auto &patternIter : freqPattern)
		{
			if(patternIter.pattern == FPS.pattern)
			{
				isPatternExit = true;
			}
		}
		//if the pattern is not exit, 
		//than add it
		//else add the count
		if(!isPatternExit)
		{
			freqPattern.push_back(FPS);
		}
		
		//loop through the next node
		iterNode = iterNode->parent;
	}
		
}



/*******************************************************************************
	DEFINITION:
		This function will check if there is any item's frequency that lower than 
		minSupportCount.
	INPUT:
		map<string,int> itemFreq - representing the freqency of item based on Conditional Partten Base
	OUTPUT:
		vector<vector<string> > branches - an new branches without an item that it's frequncy lower than 
										   minSupport
******************************************************************************/
void FPGrowth::setMinSupportForCPB(vector<vector<string> > &branches, map<string,int> itemFreq)
{
	for(auto &freq : itemFreq)
	{
		if(freq.second < this->minSupportCount)
		{
			for(auto & branch : branches)
			{
				auto iter = find(branch.begin(), branch.end(), freq.first);
				if(iter != branch.end())
				{
					branch.erase(iter);
				}
			}
		}
	}
}









/*******************************************************************************
	DEFINITION:
		This function will display the transactions.
******************************************************************************/
void FPGrowth::displayTransactions()const
{
	cout << "----- Display Transactions -----" << endl;

	for(auto &transaction : this->transactions)
	{
		for(auto item : transaction)
		{
			cout << item << ",";
		}
		cout << endl;
	}
	cout << "---------------------------------------" << endl;
}

/*******************************************************************************
	DEFINITION:
		This function will display all the items' frequency.
******************************************************************************/
void FPGrowth::displayFrequencyOfItem(map<string, int> itemFreq) const
{
	cout << endl << "----- Display freaquency of items -----" << endl;
	for(auto &item : itemFreq)
	{
		cout << item.first << ":" << item.second << endl; 
	}
	cout << "---------------------------------------" << endl;
}

/*******************************************************************************
	DEFINITION:
		This function will display frequency pattern tree's noed's information.
******************************************************************************/
void FPGrowth::displayNode(shared_ptr<FPTreeNode> node) const
{
	cout << endl << "--------- display FP Tree Node ---------" << endl;
	if(node != NULL)
	{
		cout << "The item is: " << node->itemName << endl;
		cout << "The count is: " << node->count << endl;
		
		
		cout << "The chilren node is: ";
		for(auto iter = node->children.begin(); iter < node->children.end(); iter++)
		{
			cout << (*iter)->itemName << ":" << (*iter)->count << " , ";
		}
		
		cout << endl;
	}
	else
	{
		cout << "It is NULL" << endl;
	}
	
}

/*******************************************************************************
	DEFINITION:
		This function will display fp-tree node's route in back forth direction.
******************************************************************************/
void FPGrowth::displayCrusingBack(shared_ptr<FPTreeNode> node)const
{
	shared_ptr<FPTreeNode> iterNode;

	iterNode = node;
	cout << endl << "--- Crusing Back ---" << endl;
	while(iterNode != NULL)
	{
		cout << iterNode->itemName << ":" << iterNode->count << endl;
		iterNode = iterNode->parent;
	}
}


/*******************************************************************************
	DEFINITION:
		This function will display the header table.
******************************************************************************/
void FPGrowth::displayHeaderTable(string item, map<string, vector<shared_ptr<FPTreeNode> > > header)const
{
	cout << endl << "---------- Header table for " << item << " ----------" << endl;
	for(auto &node : header[item])
	{
		displayCrusingBack(node);
	}
}


/*******************************************************************************
	DEFINITION:
		This function will display the Conditional Pattern Base.
******************************************************************************/
void FPGrowth::displayCPB(shared_ptr<CPB_Node>& node)const
{
	/***************************
	struct CPB_Node {
	int count;
	vector<string> branch;
	shared_ptr<CPB_Node> next;
	};
	***************************/
	cout << endl << "--- Conditional Pattern Base ---" << endl;
	
	while(node != NULL)
	{
		cout << node->count << ":(";
		for(auto iterForBranch = node->branch.begin(); iterForBranch < node->branch.end(); iterForBranch++)
		{
			cout << *iterForBranch << ",";
		}
		cout << ")" << endl;

		node = node->next;
	}

}


/*******************************************************************************
	DEFINITION:
		This function will display the Frequency Pattern.
******************************************************************************/
void FPGrowth::displayFreqPattern()const
{
	//vector<FreqPatternStruct> frequencyPattern;


	cout << endl << "---------- Display Freq Pattern ----------" << endl;
	for(auto &pattern : this->frequencyPattern)
	{
		cout << "Freq = " << pattern.count << " -> ";
		cout << "pattern: ";
		for(auto &item : pattern.pattern)
		{
			cout << item << ",";
		}
		cout << endl;
	}
}