#include "FPGrowth.hpp"


#include <fstream>
#include <sstream>



FPGrowth::FPGrowth(string fileName)
{
	this->fileName = fileName;

	transactions = readTransactions(fileName);
	frequencyOfItems = itemsFrequency(transactions);
	transactions = sortRecordsByFrequency(transactions, frequencyOfItems);
	buildFPTree(transactions, rootOfFPTree);


	for (map<string, int>::iterator it = frequencyOfItems.begin(); it != frequencyOfItems.end(); ++it) {
		findFrequencyPattern(it->first);
	}



}



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


vector<vector<string> > FPGrowth::sortRecordsByFrequency(vector<vector<string> > transactions, map<string, int> frequencyTable)
{


	for (auto &transaction : transactions)
	{
		for (auto &item : transaction)
		{
			for (auto itemComp = transaction.begin() + 1; itemComp < transaction.end(); itemComp++)
			{
				if (frequencyTable[item] < frequencyTable[*itemComp])
				{
					auto temp = item;
					item = *itemComp;
					*itemComp = item;
				}
			}
		}
	}

	return transactions;
}







void FPGrowth::buildFPTree(vector<vector<string> > transactions,
	shared_ptr<FPTreeNode> treeRoot)
{
	treeRoot = make_shared<FPTreeNode>();



	for (auto &transaction : transactions)
	{
		treeRoot = addOneTransaction(treeRoot, transaction);
	}


}






shared_ptr<FPTreeNode> FPGrowth::addOneTransaction(shared_ptr<FPTreeNode> root, vector<string> transaction)
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
		headerTable = addItemToHeaderTable(headerTable, nodeIter, itemName);
	}

	return root;
}


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


shared_ptr<FPTreeNode> FPGrowth::addChildNode(shared_ptr<FPTreeNode> root, string itemName)
{
	shared_ptr<FPTreeNode> child = make_shared<FPTreeNode>();

	child->itemName = itemName;
	child->count = 1;
	child->parent = root;

	root->children.push_back(child);

	return child;
}


map<string, vector<shared_ptr<FPTreeNode> > >
FPGrowth::addItemToHeaderTable(map<string, vector<shared_ptr<FPTreeNode> > > headerTable,
	shared_ptr<FPTreeNode> itemNode,
	string itemName)
{
	headerTable[itemName].push_back(itemNode);

	return headerTable;
}




void FPGrowth::findFrequencyPattern(string itemName)
{
	vector<branch> conditionalPatternBase;
	branch branchForItem;
	shared_ptr<FPTreeNode> nodeIter;


	/***********************************************************************
	* Process - This part is about that creating an conditional pattern tree
	************************************************************************/
	for (auto &nodeForItem : headerTable[itemName])
	{
		branchForItem.branchCount = nodeForItem->count;

		nodeIter = nodeForItem;
		while (nodeIter != NULL)
		{
			branchForItem.itemList.push_back(nodeIter->itemName);
			nodeIter = nodeIter->parent;
		}

	}







}