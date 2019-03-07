#pragma once

#include <algorithm>
#include <typeinfo.h>
#include "stdio.h"
#include "Stack.h"

///////////////////////////////////////////////////////////////////////////////////////////////
/*复杂结构释放器*/
///////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct Cleaner
{
	static void clean(T x)
	{ //相当于递归基
#ifdef _DEBUG
		static int n = 0;
		if (7 > strlen(typeid (T).name()))
		{ //复杂类型一概忽略，只输出基本类型
			printf("\t<%s>[%d]=", typeid (T).name(), ++n);
			//print(x);
			printf(" purged\n");
		}
#endif
	}
};

template <typename T>
struct Cleaner<T*>
{
	static void clean(T* x)
	{
		if (x)
		{
			delete x;  //如果其中包含指针，递归释放
		}
#ifdef _DEBUG                
		static int n = 0;
		printf("\t<%s>[%d] released\n", typeid (T*).name(), ++n);
#endif
	}

};

template <typename T>
void release(T x)
{
	Cleaner<T>::clean(x);
}


///////////////////////////////////////////////////////////////////////////////////////////////
/*节点模板类*/
///////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class TreeNode
{
public:
	Friend class MultiTree<T>;

	// 成员
	T data; //数据

	// 构造函数
	TreeNode() :
		parent(NULL), elder(NULL), younger(NULL), ch(NULL), height(0) { }

	TreeNode(T d, TreeNode<T>* p = NULL, TreeNode<T>* eld = NULL, TreeNode<T>* young = NULL, TreeNode<T>* ch = NULL) :
		data(d), parent(p), elder(eld), younger(young), child(ch) { }

private:
	// 成员
	TreeNode<T>* parent; //父节点
	TreeNode<T>* elder; //兄长
	TreeNode<T>* younger; //弟弟
	TreeNode<T>* child; //最小孩子

	// 操作接口
	TreeNode<T>* insertAsCh(T const&); //新建节点作为当前节点的最小孩子插入
	template <typename VST> void travPre(VST&); //子树先序遍历
};

///////////////////////////////////////////////////////////////////////////////////////////////
/*节点模板类实现*/
///////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
TreeNode<T>* TreeNode<T>::insertAsCh(T const & d)  //新建节点作为当前节点的最小孩子插入
{
	ch = new TreeNode(d, this, this->child);
	this->child->younger = ch;
	this->child = ch;
	return ch;
}

template<typename T>
template<typename VST>
void TreeNode<T>::travPre(VST & visit)  //子树先序遍历
{
	Stack<BinNodePosi(T)> S; //辅助栈
	TreeNode<T>* x = this;
	TreeNode<T>* temp = x->child;
	S.push(x);  //根节点入栈
	while (!S.empty())  //在栈变空之前反复循环
	{
		x = S.pop();
		visit(x->data);  //弹出并访问当前节点，其非空孩子的入栈次序为先右后左
		while (temp)
		{
			S.push(temp);
			temp = temp->elder;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
/*多叉树模板类*/
///////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
class MultiTree
{
protected:
	int _size; //规模
	TreeNode<T>* _root; //根节点指针

public:
	MultiTree() : _size(0), _root(NULL) { } //构造函数

	virtual ~BinTree() //析构函数
	{
		if (0 < _size)
			remove(_root);
	}

	bool empty() const  //判空
	{
		return !_root;
	}

	TreeNode<T>* root() const //树根
	{
		return _root;
	}

	int size() const  //规模
	{
		return _size;
	}

	TreeNode<T>* insertAsRoot(T const& e); //插入根节点

	TreeNode<T>* insertAsYC(TreeNode<T>* x, T const& e); //e作为x的最小孩子（原无）插入

	void remove(TreeNode<T>* x); //删除以位置x处节点为根的子树

	template <typename VST> //用操作器VST遍历
	void travPre(VST& visit)
	{
		if (_root)
			_root->travPre(visit);
	} //先序遍历

};

///////////////////////////////////////////////////////////////////////////////////////////////
/*多叉树模板实现*/
///////////////////////////////////////////////////////////////////////////////////////////////
template <typename T> //删除二叉树中位置x处的节点及其后代
void MultiTree<T>::remove(TreeNode<T>* x)
{ //assert: x为二叉树中的合法位置
	if (x->parent->child == x)
	{
		x->parent->child = (x->elder) ? x->elder : NULL; //切断来自父节点的指针
	}
	if (x->younger)
	{
		x->younger->elder = x->elder; //切断来自弟弟的指针
	}
	if (x->elder)
	{
		x->elder->younger = x->younger; //切断来自兄长的指针
	}
	removeAt(x);
}

template <typename T> //递归删除x处节点及其后代
static void removeAt(TreeNode<T>* x)
{
	if (x == NULL)
		return; //递归基
	TreeNode<T>* temp = x->child;
	while (x->child)
	{
		x->child = x->child->elder;
		removeAt(x->child->elder);
	}
	release(x->data); //release()负责释放复杂结构
	release(x);
}

template<typename T>
TreeNode<T>* MultiTree<T>::insertAsRoot(T const & e)
{
	_size = 1;
	return _root = new TreeNode<T>(e);
}

template <typename T>
TreeNode<T>* MultiTree<T>::insertAsYC(TreeNode<T>* x, T const& e)  //e插入为x的最小孩子
{
	_size++;
	x->insertAsCh(e);
	return x->rc;
}



