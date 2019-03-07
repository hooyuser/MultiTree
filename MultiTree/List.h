#ifndef LIST_H_
#define LIST_H_

typedef int Rank;  //秩
#define ListNodePosi(T) ListNode<T> *  //列表节点位置


/* 列表节点模板结构 */
template <typename T>
struct ListNode 
{
	// 成员
	T data;  //数值
	ListNodePosi(T) pred;  //前驱
	ListNodePosi(T) succ;  //后继

// 构造函数
	ListNode() {}  //针对header和trailer的构造
	ListNode(T e, ListNodePosi(T) p = nullptr, ListNodePosi(T) s = nullptr)
		: data(e), pred(p), succ(s) {}  //默认构造器

// 操作接口
	ListNodePosi(T) insertAsPred(T const& e);  //紧靠当前节点之前插入新节点
	ListNodePosi(T) insertAsSucc(T const& e);  //紧随当前节点之后插入新节点
};




/* 列表模板类 */
template <typename T>
class List  
{
private:
	int _size;  //规模
	ListNodePosi(T) header;  //头哨兵
	ListNodePosi(T) trailer;  //尾哨兵

protected:
	void init();  //列表创建时的初始化

	int clear();  //清除所有节点

	void copyNodes(ListNodePosi(T) p, int n);  //复制列表中自位置p起的n项

	void merge(ListNodePosi(T)& pp, int n, List<T>& L, ListNodePosi(T) q, int m);  //归并

	void mergeSort(ListNodePosi(T)& p, int n);  //对从p开始连续的n个节点归并排序

	void selectionSort(ListNodePosi(T) p, int n);  //对从p开始连续的n个节点选择排序

	void insertionSort(ListNodePosi(T) p, int n);  //对从p开始连续的n个节点插入排序

public:
	// 构造函数
	List() //默认构造函数
	{
		init();
	}

	List(List<T> const& L)  //整体复制列表L
	{
		copyNodes(L.first(), L.size());
	}

	List(List<T> const& L, Rank r, int n);  //复制列表L中自第r项起的n项

	List(ListNodePosi(T) p, int n)  //复制列表中自位置p起的n项
	{
		copyNodes(p, n);
	}

	// 析构函数
	~List();  //释放（包含头、尾哨兵在内的）所有节点

// 只读访问接口
	Rank size() const  //规模
	{
		return _size;
	}

	bool empty() const  //判空
	{
		return _size <= 0;
	}

	T& operator[] (Rank r) const;  //运算符[]重载，支持循秩访问（效率低）

	ListNodePosi(T) first() const  //首节点位置
	{
		return header->succ;
	}

	ListNodePosi(T) last() const  //末节点位置
	{
		return trailer->pred;
	}

	bool valid(ListNodePosi(T) p)  //判断位置p是否对外合法
	{
		return p && (trailer != p) && (header != p);
	}  //将头、尾节点等同于nullptr

	int disordered() const;  //判断列表是否已排序

	ListNodePosi(T) find(T const& e) const  //无序列表查找
	{
		return find(e, _size, trailer);
	}

	ListNodePosi(T) find(T const& e, int n, ListNodePosi(T) p) const;  //无序区间查找

	ListNodePosi(T) search(T const& e) const  //有序列表查找
	{
		return search(e, _size, trailer);
	}
	ListNodePosi(T) search(T const& e, int n, ListNodePosi(T) p) const;  //有序区间查找

	ListNodePosi(T) selectMax(ListNodePosi(T) p, int n);  //在p及其n-1个后继中选出最大者

	ListNodePosi(T) selectMax()
	{
		return selectMax(header->succ, _size);
	}  //整体最大者

// 可写访问接口
	ListNodePosi(T) insertAsFirst(T const& e);  //将e当作首节点插入

	ListNodePosi(T) insertAsLast(T const& e);  //将e当作末节点插入

	ListNodePosi(T) insertA(ListNodePosi(T) p, T const& e);  //将e当作p的后继插入（After）

	ListNodePosi(T) insertB(ListNodePosi(T) p, T const& e);  //将e当作p的前驱插入（Before）

	T remove(ListNodePosi(T) p);  //删除合法位置p处的节点,返回被删除节点

	void merge(List<T>& L)   //全列表归并
	{
		merge(header->succ, _size, L, L.header->succ, L._size);
	}

	void sort(ListNodePosi(T) p, int n);  //列表区间排序

	void sort()   //列表整体排序
	{
		sort(first(), _size);
	}

	int deduplicate();  //无序去重

	int uniquify();  //有序去重

	void reverse();  //前后倒置

	// 遍历
	void traverse(void(*visit) (T&));  //遍历，依次实施visit操作（函数指针，只读或局部性修改）

	template <typename VST>  //操作器
	void traverse(VST& visit);  //遍历，依次实施visit操作（函数对象，可全局性修改）
};  





/* 节点模板结构实现 */
template <typename T>
ListNodePosi(T) ListNode<T>::insertAsPred(T const& e)  //将e紧靠当前节点之前插入于当前节点所属列表（设有哨兵头节点header）
{
	ListNodePosi(T) x = new ListNode(e, pred, this);  //创建新节点
	pred->succ = x;
	pred = x;  //设置正向链接
	return x;  //返回新节点的位置
}


template <typename T>
ListNodePosi(T) ListNode<T>::insertAsSucc(T const& e)  //将e紧随当前节点之后插入于当前节点所属列表（设有哨兵尾节点trailer）
{
	ListNodePosi(T) x = new ListNode(e, this, succ);  //创建新节点
	succ->pred = x;
	succ = x;  //设置逆向链接
	return x;  //返回新节点的位置
}




/* 列表模板类实现 */
template <typename T>
void List<T>::copyNodes(ListNodePosi(T) p, int n)  //列表内部方法：复制列表中自位置p起的n项
{  //p合法，且至少有n-1个真后继节点
	init();  //创建头、尾哨兵节点并做初始化
	while (n--)
	{
		insertAsLast(p->data);  //将起自p的n项依次作为末节点插入
		p = p->succ;
	}
}


template <typename T> //列表的归并排序算法：对起始于位置p的n个元素排序
void List<T>::mergeSort(ListNodePosi(T)& p, int n)
{
	//valid(p) && rank(p) + n <= size
	if (n < 2) return; //若待排序范围已足够小，则直接返回；否则...
	int m = n >> 1; //以中点为界
	ListNodePosi(T) q = p;
	for (int i = 0; i < m; i++)
		q = q->succ; //均分列表
	mergeSort(p, m);
	mergeSort(q, n - m); //对前、后子列表分别排序
	merge(p, m, *this, q, n - m); //归并

} //注意：排序后，p依然指向归并后区间的（新）起点


template <typename T> //有序列表的归并：当前列表中自p起的n个元素，与列表L中自q起的m个元素归并
void List<T>::merge(ListNodePosi(T)& pp, int n, List<T>& L, ListNodePosi(T) q, int m)
{
	// assert:  this.valid(p) && rank(p) + n <= size && this.sorted(p, n)
	//          L.valid(q) && rank(q) + m <= L._size && L.sorted(q, m)
	// 注意：在归并排序之类的场合，有可能 this == L && rank(p) + n = rank(q)
	ListNodePosi(T) p = pp;
	ListNodePosi(T) pr = pp->pred;  //借助前驱（可能是header），以便返回前 ...
	while (0 < m) //在q尚未移出区间之前
		if ((0 < n) && (p->data <= q->data)) //若p仍在区间内且v(p) <= v(q)，则
		{
			if (q == (p = p->succ)) break; n--;
		} //p归入合并的列表，并替换为其直接后继
		else //若p已超出右界或v(q) < v(p)，则
		{
			insertB(p, L.remove((q = q->succ)->pred)); m--;
		} //将q转移至p之前
	pp = pr->succ; //确定归并后区间的（新）起点
}


template <typename T>
void List<T>::init()  //列表初始化，在创建列表对象时统一调用
{
	header = new ListNode<T>;  //创建头哨兵节点
	trailer = new ListNode<T>;  //创建尾哨兵节点
	header->succ = trailer; header->pred = nullptr;
	trailer->pred = header; trailer->succ = nullptr;
	_size = 0;  //记录规模
}


template <typename T>
List<T>::List(List<T> const& L, Rank r, int n)
{
	ListNodePosi(T) p = L.first();  //从首节点出发
	while (0 < r--)
		p = p->succ;
	copyNodes(p, n);
}


template <typename T>
List<T>::~List()
{
	clear();
	delete header;
	delete trailer;
}


template <typename T>
T& List<T>::operator[] (Rank r) const  //重载下标操作符，以通过秩直接访问列表节点（复杂度O(n)）
{
	ListNodePosi(T) p = first();  //从首节点出发
	while (0 < r--)
		p = p->succ;  //顺数第r个节点即是
	return p->data;  //目标节点，返回其中所存元素
}


template <typename T>
ListNodePosi(T) List<T>::insertAsFirst(T const& e)  //e当作首节点插入
{
	_size++;
	return header->insertAsSucc(e);
}


template <typename T>
ListNodePosi(T) List<T>::insertAsLast(T const& e)  //e当作末节点插入
{
	_size++;
	return trailer->insertAsPred(e);
}


template <typename T>
ListNodePosi(T) List<T>::insertA(ListNodePosi(T) p, T const& e)  //e当作p的后继插入（After）
{
	_size++;
	return p->insertAsSucc(e);
}


template <typename T>
ListNodePosi(T) List<T>::insertB(ListNodePosi(T) p, T const& e)  //e当作p的前驱插入（Before）
{
	_size++;
	return p->insertAsPred(e);
}


template <typename T>
T List<T>::remove(ListNodePosi(T) p)  //删除合法节点p，返回其数值，复杂度 O(1)
{
	T e = p->data;  //备份待删除节点的数值（假定T类型可直接赋值）
	p->pred->succ = p->succ;  //后继
	p->succ->pred = p->pred;  //前驱
	delete p;  //释放节点
	_size--;  //更新规模
	return e;  //返回备份的数值
}


template<typename T>
void List<T>::sort(ListNodePosi(T) p, int n)
{
	mergeSort(p, n);
}


template <typename T>  //清空列表
int List<T>::clear()
{
	int oldSize = _size;
	while (0 < _size)
		remove(header->succ); //反复删除首节点，直至列表变空
	return oldSize;
}



template <typename T>
int List<T>::deduplicate()  //剔除无序列表中的重复节点
{
	if (_size < 2)
		return 0;  //平凡列表自然无重复
	int oldSize = _size;  //记录原规模
	ListNodePosi(T) p = header;  //p从首节点开始
	Rank r = 0;
	while ((p = p->succ) != trailer)  //依次直到末节点
	{
		ListNodePosi(T) q = find(p->data, r, p);  //在p的r个（真）前驱中查找雷同者
		q ? remove(q) : r++;  //若存在，则删除之；否则秩加一
	}
	//assert: 循环过程中的任意时刻，p的所有前驱互不相同
	return oldSize - _size;  //列表规模变化量，即被删除元素总数
}


template <typename T>
int List<T>::uniquify()              //剔除有序列表中的重复节点
{
	if (_size < 2)
		return 0;                    //平凡列表自然无重复
	int oldSize = _size;             //记录原规模
	ListNodePosi(T) p = first();
	ListNodePosi(T) q;               //p为各区段起点，q为其后继
	while (trailer != (q = p->succ)) //反复考查紧邻的节点对(p, q)
	{
		if (p->data != q->data)
			p = q;                   //若互异，则转向下一区段
		else
			remove(q);               //否则删除后者
	}
	return oldSize - _size;          //列表规模变化量，即被删除元素总数
}


template <typename T>
void List<T>::reverse()
{ //前后倒置
	if (_size <= 1) return; //平凡情况
	for (ListNodePosi(T) p = header; p; p = p->pred) //自前向后，依次
		swap(p->pred, p->succ); //交换各节点癿前驱、后继指针
	swap(header, trailer); //头、尾节点互换
}


template <typename T>
int List<T>::disordered() const
{
	int n = 0;
	ListNodePosi(T) p = header;
	while (p != trailer)
		if (p > p->succ)
		{
			n++;
			p = p->succ;
		}
	return n;
}


//在无序列表内节点p（可能是trailer）的n个（真）前驱中，找到等于e的最后者
template<typename T>
ListNodePosi(T) List<T>::find(T const& e, int n, ListNodePosi(T) p) const
{
	while (0 < n--)  //（0 <= n <= rank(p) < _size）对于p的最近的n个前驱，从右向左逐个比对，直至命中或范围越界
	{
		p = p->pred;
		if (e == p->data) return p;
	}
	return nullptr;  //p越出左边界意味着区间内不含e，查找失败，返回空指针
}


template <typename T> //在有序列表内节点p（可能是trailer）的n个（真）前驱中，找到不大于e的最后者
ListNodePosi(T) List<T>::search(T const& e, int n, ListNodePosi(T) p) const
{
	// assert: 0 <= n <= rank(p) < _size
	do
	{
		p = p->pred; n--;  //从右向左

	} while ((-1 < n) && (e < p->data)); //逐个比较，直至命中或越界
	return p; //返回查找终止的位置
} //失败时，返回区间左边界的前驱（可能是header）


template<typename T>
ListNodePosi(T) List<T>::selectMax(ListNodePosi(T) p, int n)
{
	ListNodePosi(T) max = p;
	while (--n)  //执行n-1次
	{
		p = p->succ;
		if (p->data > max->data)
			max->p;
	}
	return max;
}


template <typename T>
void List<T>::selectionSort(ListNodePosi(T) p, int n)   //列表的选择排序算法：对起始于位置p的n个元素排序
{
	//valid(p) && rank(p) + n <= size
	ListNodePosi(T) head = p->pred;
	ListNodePosi(T) tail = p;
	for (int i = 0; i < n; i++)
		tail = tail->succ;                               //待排序区间为(head, tail)
	while (1 < n)                                        //在至少还剩两个节点之前，在待排序区间内
	{
		ListNodePosi(T) max = selectMax(head->succ, n);  //找出[head->succ, head->succ + n)中最大者（歧义时后者优先）
		insertB(tail, remove(max));                      //将其移至无序区间末尾（作为有序区间新的首元素）
		tail = tail->pred;
		n--;
	}
}


template <typename T> //列表的插入排序算法：对起始于位置p的n个元素排序
void List<T>::insertionSort(ListNodePosi(T) p, int n)
{ //valid(p) && rank(p) + n <= size
	for (int r = 0; r < n; r++)
	{ //逐一为各节点
		insertA(search(p->data, r, p), p->data); //查找适当的位置并插入
		p = p->succ;
		remove(p->pred); //转向下一节点
	}
}


template <typename T> void List<T>::traverse(void(*visit) (T&)) //借助函数指针机制遍历
{
	for (ListNodePosi(T) p = header->succ; p != trailer; p = p->succ) visit(p->data);
}

template <typename T> template <typename VST> //元素类型、操作器
void List<T>::traverse(VST& visit) //借助函数对象机制遍历
{
	for (ListNodePosi(T) p = header->succ; p != trailer; p = p->succ) visit(p->data);
}
#endif  
