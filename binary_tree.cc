#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

template < class T > const T & MAX(const T & a, const T & b)
{
	return (a > b ? a : b);
}

struct BinaryTreeNode {
	int data;
	BinaryTreeNode *parent;
	BinaryTreeNode *left;
	BinaryTreeNode *right;
};

static BinaryTreeNode *New()
{
	return NULL;
}

static int Height(BinaryTreeNode * root)
{
	if (root)
		return 1 + MAX(Height(root->right), Height(root->left));
	else
		return 0;
}

static int Weight(BinaryTreeNode * root)
{
	if (root)
		return 1 + Weight(root->right) + Weight(root->left);
	else
		return 0;
}

static BinaryTreeNode *RotateLeft(BinaryTreeNode * root)
{
	if (root && root->right) {
		BinaryTreeNode *pivot = root->right;

		if (pivot->left)
			pivot->left->parent = root;
		pivot->parent = root->parent;
		root->parent = pivot;

		root->right = pivot->left;
		pivot->left = root;

		return pivot;
	}
	return root;
}

static BinaryTreeNode *RotateRight(BinaryTreeNode * root)
{
	if (root && root->left) {
		BinaryTreeNode *pivot = root->left;

		if (pivot->right)
			pivot->right->parent = root;
		pivot->parent = root->parent;
		root->parent = pivot;

		root->left = pivot->right;
		pivot->right = root;

		return pivot;
	}
	return root;
}

static int balance_factor(BinaryTreeNode * root)
{
	return Height(root->left) - Height(root->right);
}

static BinaryTreeNode *AVL_Rebalance(BinaryTreeNode * root)
{
	if (balance_factor(root) > 1) {
		if (balance_factor(root->left) < 0)
			root->left = RotateLeft(root->left);
		root = RotateRight(root);
	}
	if (balance_factor(root) < -1) {
		if (balance_factor(root->right) > 0)
			root->right = RotateRight(root->right);
		root = RotateLeft(root);
	}
	return root;
}

static BinaryTreeNode *Add(BinaryTreeNode * root, int a, bool AVL = false, BinaryTreeNode * parent = NULL)
{
	if (root == NULL) {
		root = (BinaryTreeNode *) malloc(sizeof(BinaryTreeNode));
		root->data = a;
		root->left = NULL;
		root->right = NULL;
		root->parent = parent;
	} else if (a > root->data)
		root->right = Add(root->right, a, AVL, root);
	else
		root->left = Add(root->left, a, AVL, root);

	if (AVL)
		root = AVL_Rebalance(root);

	return root;
}

static BinaryTreeNode *Min(BinaryTreeNode * root)
{
	if (root->left == NULL)
		return root;
	else
		return Min(root->left);
}

static BinaryTreeNode *Max(BinaryTreeNode * root)
{
	if (root->right == NULL)
		return root;
	else
		return Max(root->right);
}

static BinaryTreeNode *Delete(BinaryTreeNode * root, int a, bool AVL = false)
{
	if (root) {

		if (a > root->data)
			root->right = Delete(root->right, a, AVL);
		else if (a < root->data)
			root->left = Delete(root->left, a, AVL);
		else {/* if (a == root->data) */

			if (root->left && root->right) {
				BinaryTreeNode *sucessor = Min(root->right);
				root->data = sucessor->data;
				root->right = Delete(root->right, sucessor->data, AVL);
			} else if (root->left) {
				root->left->parent = root->parent;
				free(root);
				root = root->left;
			} else if (root->right) {
				root->right->parent = root->parent;
				free(root);
				root = root->right;
			} else {
				free(root);
				return NULL;
			}
		}

		if (AVL)
			root = AVL_Rebalance(root);

	}
	return root;

}

static void Print(BinaryTreeNode * root, int depth)
{
	if(root == NULL)
	{
		printf("\n");
		return;
	}

	Print(root->right,depth+1);
	for(int i=0; i < depth; i++)
	{
		printf("\t");
	}
	printf("%d\n", root->data);
	Print(root->left,depth+1);
}


static void PrintGraphViz(BinaryTreeNode * root)
{
	if (root) {
		PrintGraphViz(root->left);
		printf("p%016jx [label = %d]; ", (uintmax_t)root, root->data);
		if (root->left)
			printf("p%016jx -> p%016jx [color = green]; ", (uintmax_t)root,
			        (uintmax_t)root->left);
		if (root->right)
			printf("p%016jx -> p%016jx [color = red]; ",  (uintmax_t)root,
			        (uintmax_t)root->right);
		if (root->parent)
			printf
			    ("p%016jx -> p%016jx [color = blue] [constraint=false]; ",
			      (uintmax_t)root,  (uintmax_t)root->parent);
		PrintGraphViz(root->right);
	}
}

static void PrintGraphVizCommand(BinaryTreeNode *root, const char * prefix)
{
	static int frame;
	printf("echo \"digraph G { ");
	PrintGraphViz(root);
	printf("} \" | dot  -Tpng > %s%03d.png\n", prefix, frame++);
}

class BinaryTree {
protected:
	BinaryTreeNode *root;
public:
	BinaryTree() : root(::New()) {}
	void Add(int val) { root = ::Add(root, val); }
	void Delete(int val) { root = ::Delete(root, val); }
	void PrintGraphVizCommand(const char * prefix) { ::PrintGraphVizCommand(root,prefix); }
	void Print() { ::Print(root, 0); }
};

class AVLTree : public BinaryTree {
public:
	AVLTree() : BinaryTree() {}
	void Add(int val) { root = ::Add(root, val, true); }
	void Delete(int val) { root = ::Delete(root, val, true); }
};

int main()
{
	AVLTree avl_tree;
	BinaryTree binary_tree;

	srand(4);
	for (int i = 0; i < 10; ++i) {
		int j = rand() % 1000;
		binary_tree.Add(j);
		avl_tree.Add(j);
		binary_tree.PrintGraphVizCommand("baz/graph");
		avl_tree.PrintGraphVizCommand("baz/graph");
	}

	srand(4);
	for (int i = 0; i < 10; ++i) {
		int j = rand() % 1000;
		binary_tree.Delete(j);
		avl_tree.Delete(j);
		binary_tree.PrintGraphVizCommand("baz/graph");
	}

	return 0;
}
