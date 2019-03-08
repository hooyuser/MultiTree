#include <iostream>
#include "MultiTree.h"

int main()
{
	MultiTree<int> myTree = MultiTree<int>();
	TreeNode<int>* node[4];

	node[0] = myTree.insertAsRoot(1);
	node[1] = myTree.insertAsYC(node[0], 2);
	node[2] = myTree.insertAsYC(node[0], 3);

	return 0;
}