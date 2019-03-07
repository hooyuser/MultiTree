//vector class interface

#ifndef VECTOR_H_
#define VECTOR_H_

//#include <stdlib.h>      
#include <time.h> 
#include <algorithm>

using namespace std;

typedef int Rank; //秩
#define DEFAULT_CAPACITY  3 //默认的初始容量（实际应用中可设置为更大）

template <typename T>
class Vector //向量模板类
{
protected:
	Rank _size; //规模
	int _capacity;  //容量
	T* _elem; //数据区
	void copyFrom(T const* A, Rank lo, Rank hi); //复制数组区间A[lo, hi)
	void expand(); //空间不足时扩容
	void shrink(); //装填因子过小时压缩
	Rank bubble(Rank lo, Rank hi); //扫描交换
	Rank max(Rank lo, Rank hi); //选取最大元素
	Rank partition(Rank lo, Rank hi); //轴点构造算法
	void merge(Rank lo, Rank mi, Rank hi); //归并算法


public:
	// 构造函数
	Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) //容量为c、规模为s、所有元素初始为v
	{
		_elem = new T[_capacity = c];
		for (_size = 0; _size < s; _elem[_size++] = v);
	} //s<=c

	Vector(T const* A, Rank n) //数组整体复制
	{
		copyFrom(A, 0, n);
	}

	Vector(T const* A, Rank lo, Rank hi) //数组区间复制
	{
		copyFrom(A, lo, hi);
	}

	Vector(Vector<T> const& V) //向量整体复制
	{
		copyFrom(V._elem, 0, V._size);
	}

	Vector(Vector<T> const& V, Rank lo, Rank hi) //向量区间复制
	{
		copyFrom(V._elem, lo, hi);
	}


	// 析构函数
	~Vector()
	{
		delete[] _elem;  //释放内部空间
	}


	// 只读访问接口
	Rank size() const //规模
	{
		return _size;
	}

	bool empty() const //判空
	{
		return !_size;
	}

	int disordered() const; //判断向量是否已排序

	Rank find(T const& e) const //无序向量整体查找
	{
		return find(e, 0, _size);
	}

	Rank find(T const& e, Rank lo, Rank hi) const; //无序向量区间查找

	Rank search(T const& e) const //有序向量整体查找
	{
		return (0 >= _size) ? -1 : search(e, 0, _size);
	}

	Rank search(T const& e, Rank lo, Rank hi) const; //有序向量区间查找


// 可写访问接口
	T& operator[] (Rank r) const; //重载下标操作符，可以类似于数组形式引用各元素

	Vector<T> & operator= (Vector<T> const&); //重载赋值操作符，以便直接克隆向量

	T remove(Rank r); //删除秩为r的元素

	int remove(Rank lo, Rank hi); //删除秩在区间[lo, hi)之内的元素

	Rank insert(Rank r, T const& e); //插入元素

	Rank insert(T const& e) //默认作为末元素插入
	{
		return insert(_size, e);
	}

	int deduplicate(); //无序去重

	int uniquify(); //有序去重

	void sort(Rank lo, Rank hi); //对[lo, hi)排序

	void sort() //整体排序
	{
		sort(0, _size);
	}

	void unsort(Rank lo, Rank hi); //对[lo, hi)置乱

	void unsort() //整体置乱
	{
		unsort(0, _size);
	}

	void insertionSort(Rank lo, Rank hi);  //插入排序算法

	void insertionSort()
	{
		insertionSort(0, _size);
	}


	void selectionSort(Rank lo, Rank hi); //选择排序算法

	void selectionSort()
	{
		selectionSort(0, _size);
	}

	void mergeSort(Rank lo, Rank hi); //归并排序算法

	void mergeSort()
	{
		mergeSort(0, _size);
	}

	void bubbleSort(Rank lo, Rank hi); //起泡排序算法

	void bubbleSort()
	{
		bubbleSort(0, _size);
	}

	void quickSort(Rank lo, Rank hi);  //快速排序算法

	void quickSort()
	{
		quickSort(0, _size);
	}

	void heapSort(Rank lo, Rank hi); //堆排序（稍后结合完全堆讲解）


// 遍历
	void traverse(void(*) (T&)); //遍历（使用函数指针，只读或局部性修改）

	template <typename VST> void traverse(VST&); //遍历（使用函数对象，可全局性修改）
}; //Vector


class Fib
{ //Fibonacci数列类
private:
	Rank f, g; //f = fib(k - 1), g = fib(k)。均为int型，徆快就会数值溢出

public:
	Fib(Rank n) //初始化为不小于n的最小Fibonacci项，O(log_phi(n))时间
	{
		f = 1; //f初始化为fib(-1)
		g = 0; //g初始化为fib(0)
		while (g < n) next();
	}

	Rank get() //获取当前Fibonacci项，O(1)时间
	{
		return g;
	}

	Rank next() //转至下一Fibonacci项，O(1)时间
	{
		g += f;
		f = g - f;
		return g;
	}

	Rank prev() //转至上一Fibonacci项，O(1)时间
	{
		f = g - f;
		g -= f;
		return g;
	}
};






//向量模板类实现

const double MIN_OCCUPANCY = 0.2;

template <typename T>
Rank Vector<T>::max(Rank lo, Rank hi)
{
	return (_elem[lo] > _elem[hi] ? _elem[lo] : _elem[hi]);
}

template<typename T>
Rank Vector<T>::partition(Rank lo, Rank hi)
{
	int first = lo;
	int last = hi - 1;
	int key = _elem[first];/*用字表的第一个记录作为枢轴*/

	while (first < last)
	{
		while (first < last &&  _elem[last] >= key)
		{
			--last;
		}

		_elem[first] = _elem[last];/*将比第一个小的移到低端*/

		while (first < last &&  _elem[first] <= key)
		{
			++first;
		}

		_elem[last] = _elem[first];
		/*将比第一个大的移到高端*/
	}
	_elem[first] = key;/*枢轴记录到位*/
	return first;
}


template <typename T> //元素类型为T
void Vector<T>::copyFrom(T const* A, Rank lo, Rank hi)  //以数组区间A[lo, hi)为蓝本复刢向量
{
	_elem = new T[_capacity = 2 * (hi - lo)]; //分配空间
	_size = 0;  //规模清零
	while (lo < hi)
		_elem[_size++] = A[lo++]; //A[lo, hi)内的元素逐一复制至_elem[0, hi - lo)
}


template <typename T>
T& Vector<T>::operator[] (Rank r) const //重载下标运算符[]
{
	if (r < _size)
		return _elem[r];
	else
		return _elem[_size - 1];
}


template <typename T>
Vector<T>& Vector<T>::operator= (Vector<T> const& V) //重载赋值运算符=
{
	if (_elem)
		delete[] _elem; //释放原有内容
	copyFrom(V._elem, 0, V.size()); //整体复制
	return *this; //返回当前对象的引用，以便链式赋值
}


template <typename T>
void Vector<T>::expand()  //向量空间不足时扩容
{
	if (_size < _capacity)
		return; //尚未满员时，不必扩容
	if (_capacity < DEFAULT_CAPACITY)
		_capacity = DEFAULT_CAPACITY; //不低于最小容量
	T* oldElem = _elem;  //保存原向量地址
	_elem = new T[_capacity <<= 1]; // 容量加倍
	for (int i = 0; i < _size; i++)
		_elem[i] = oldElem[i]; //复制原向量内容
	delete[] oldElem; //释放原空间
}


template <typename T>
void Vector<T>::shrink() //装填因子过小时压缩向量所占空间 
{
	if (_capacity < DEFAULT_CAPACITY << 1)
		return; //不致收缩到 DEFAULT_CAPACITY 以下
	if (_size > _capacity* MIN_OCCUPANCY)
		return; //以MIN_OCCUPANCY为下界
	T* oldElem = _elem;
	_elem = new T[_capacity >>= 1]; //容量减半
	for (int i = 0; i < _size; i++)
		_elem[i] = oldElem[i]; //复制原向量内容
	delete[] oldElem; //释放原空间
}


template <typename T>
int Vector<T>::disordered() const  //返回向量中逆序相邻元素对的总数
{
	int n = 0; //计数器
	for (int i = 1; i < _size; i++) //逐一检查_size - 1对相邻元素
		if (_elem[i - 1] > _elem[i])
			++n; //逆序则计数
	return n; //向量有序当且仅当n = 0
}


template <typename T>
void Vector<T>::unsort(Rank lo, Rank hi)  //等概率随机置乱区间[lo, hi)
{
	T* V = _elem + lo; //将子向量_elem[lo, hi)视作另一向量V[0, hi - lo)
	for (Rank i = hi - lo; i > 0; i--) //自后向前
	{
		srand((unsigned)time(0));
		swap(V[i - 1], V[rand() % i]); //将V[i - 1]与V[0, i)中某一元素随机交换
	}

}


template <typename T>
Rank Vector<T>::find(T const& e, Rank lo, Rank hi) const //无序向量的顺序查找：返回最后一个元素e的位置；失败时，返回lo - 1
{
	while ((lo < hi--) && (e != _elem[hi])); //从后向前，顺序查找，直到数组下标hi越界或查找成功
	return hi; //若hi < lo，则意味着失败；否则hi即命中元素的秩
}


template <typename T> //将e作为秩为r元素揑入
Rank Vector<T>::insert(Rank r, T const& e)
{ //assert: 0 <= r <= size
	expand(); //若有必要，扩容
	for (int i = _size; i > r; i--)
		_elem[i] = _elem[i - 1]; //自后向前，后继元素顺次后移一个单元
	_elem[r] = e;
	_size++; //置入新元素幵更新容量
	return r; //迒回秩
}


template <typename T>
int Vector<T>::remove(Rank lo, Rank hi) //初除匙间[lo, hi)
{
	if (lo == hi) return 0; //出于效率考虑，单独处理退化情况，比如remove(0, 0)
	while (hi < _size)
		_elem[lo++] = _elem[hi++]; //[hi, _size)顺次前移hi - lo个单元
	_size = lo; //更新规模，直接丢弃尾部[lo, _size = hi)区间
	shrink(); //若有必要，则缩容
	return hi - lo; //返回被初除元素癿数目
}


template <typename T>
T Vector<T>::remove(Rank r) //删除向量中秩为r的元素，0 <= r < size
{
	T e = _elem[r]; //备份被删除元素
	remove(r, r + 1); //调用区间删除算法，等效于对区间[r, r + 1)的删除
	return e; //返回被删除元素
}


template <typename T>
int Vector<T>::deduplicate() //删除无序向量中重复元素
{
	int oldSize = _size; //记录原规模
	Rank i = 1; //从_elem[1]开始
	while (i < _size) //自前向后逐一考查各元素_elem[i]
		(find(_elem[i], 0, i) < 0) ? //在其前缀中寻找不相雷同者（至少一个）
		i++ : remove(i); //若无雷同则继续考查其后继，否则初除雷同者
	return oldSize - _size; //向量觃模发化量，即被初除元素总数
}


template <typename T>
int Vector<T>::uniquify() //有序向量重复元素剔除算法
{
	Rank i = 0, j = 0; //各对互异“相邻”元素的秩
	while (++j < _size) //逐一扫描，直至末元素
		if (_elem[i] != _elem[j]) //跳过雷同者
			_elem[++i] = _elem[j]; //发现不同元素时，向前移至紧邻于前者右侧
	_size = ++i;
	shrink(); //直接截除尾部多余元素
	return j - i; //向量规模变化量，即被删除元素总数
}


template <typename T>
void Vector<T>::sort(Rank lo, Rank hi) //向量区间[lo, hi)排序
{
	switch (rand() % 5) //随机选取排序算法。可根据具体问题的特点灵活选取或扩充
	{
	case 1:  bubbleSort(lo, hi); break; //起泡排序
	case 2:  selectionSort(lo, hi); break; //选择排序（习题）
	case 3:  mergeSort(lo, hi); break; //归并排序
	case 4:  heapSort(lo, hi); break; //堆排序（稍后介绍）
	default: quickSort(lo, hi); break; //快速排序（稍后介绍）
	}
}


template <typename T> //向量的起泡排序
void Vector<T>::bubbleSort(Rank lo, Rank hi) //assert: 0 <= lo < hi <= size
{
	while (lo < (hi = bubble(lo, hi)));
} //逐趟做扫描交换，直至全序


template<typename T>
void Vector<T>::quickSort(Rank lo, Rank hi)
{
	if (hi - lo <= 1)
		return;
	else
	{
		Rank pivot = partition(lo, hi);
		quickSort(lo, pivot);
		quickSort(pivot + 1, hi);
	}
}


template <typename T>
Rank Vector<T>::bubble(Rank lo, Rank hi) //一趟扫描交换
{
	Rank last = lo; //最右侧的逆序对初始化为[lo - 1, lo]
	while (++lo < hi)
	{//自左向右，逐一检查各对相邻元素
		if (_elem[lo - 1] > _elem[lo])
		{ //若逆序，则
			last = lo; //更新最右侧逆序对位置记录，并通过交换使局部有序
			swap(_elem[lo - 1], _elem[lo]);
		}
	}
	return last; //返回最右侧的逆序对位置
}


template<typename T>
void Vector<T>::selectionSort(Rank lo, Rank hi)  //选择排序
{
	int max;
	for (int i = hi - 1; i > lo; i--)
	{
		max = lo;
		for (int j = lo + 1; j <= i; j++)
			if (_elem[j] > _elem[max])
				max = j;
		swap(_elem[i], _elem[max]);
	}
}


template<typename T>
void Vector<T>::insertionSort(Rank lo, Rank hi) //插入排序
{
	for (int i = lo + 1; i < hi; i++) //进行hi-lo-1轮排序，第i轮将_elem[i]插入到[lo,i)中的恰当位置
	{
		T temp = _elem[i];
		Rank index = search(_elem[i], lo, i) + 1;  //应该插入_elem[i]的位置
		for (int j = i; j > index; j--)
			_elem[j] = _elem[j - 1];
		_elem[index] = temp;
	}
}


template <typename T> //向量归并排序
void Vector<T>::mergeSort(Rank lo, Rank hi)
{ //0 <= lo < hi <= size
	if (hi - lo < 2) return; //单元素区间自然有序，否则...
	int mi = (lo + hi) / 2; //以中点为界
	mergeSort(lo, mi);
	mergeSort(mi, hi); //分别排序
	merge(lo, mi, hi); //归并
}


template <typename T> //有序向量的归并
void Vector<T>::merge(Rank lo, Rank mi, Rank hi)//各自有序的子向量[lo, mi)和[mi, hi)
{
	T* A = _elem + lo; //合并后的向量A[0, hi - lo) = _elem[lo, hi)

	int lb = mi - lo;
	T* B = new T[lb]; //前子向量B[0, lb) = _elem[lo, mi)
	for (Rank i = 0; i < lb; i++)
		B[i] = A[i]; //复制前子向量

	int lc = hi - mi;
	T* C = _elem + mi; //后子向量C[0, lc) = _elem[mi, hi)

	for (Rank i = 0, j = 0, k = 0; (j < lb) || (k < lc); )
	{ //B[j]和C[k]中的小者续至A末尾
		if ((j < lb) && (!(k < lc) || (B[j] <= C[k]))) A[i++] = B[j++];
		if ((k < lc) && (!(j < lb) || (C[k] < B[j]))) A[i++] = C[k++];
	}
	delete[] B; //释放临时空间B
} //归并后得到完整的有序向量[lo, hi)


template <typename T> //在有序向量的区间[lo, hi)内，确定小于等于e的最后一个节点的秩
Rank Vector<T>::search(T const& e, Rank lo, Rank hi) const
{ //assert: 0 <= lo < hi <= _size
	return (rand() % 2) ? //按各50%的概率随机使用二分查找或Fibonacci查找
		binSearch(_elem, e, lo, hi) : fibSearch(_elem, e, lo, hi);
}


// 二分查找算法（版本C）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo <= hi <= _size
template <typename T>
static Rank binSearch(T* A, T const& e, Rank lo, Rank hi)
{
	while (lo < hi)
	{ //每步迭代仅需做一次比较判断，有两个分支
		Rank mi = (lo + hi) >> 1; //以中点为轴点（区间宽度的折半，等效于宽度之数值表示的右移）
		(e < A[mi]) ? hi = mi : lo = mi + 1; //经比较后确定深入[lo, mi)或(mi, hi)
	} //成功查找不能提前终止
	return --lo; //循环结束时，lo为大于e的元素的最小秩，故lo - 1即不大于e的元素的最大秩
} //有多个命中元素时，总能保证返回秩最大者；查找失败时，能够返回失败的位置


 // Fibonacci查找算法（版本A）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo <= hi <= _size
template <typename T>
static Rank fibSearch(T* A, T const& e, Rank lo, Rank hi)
{
	Fib fib(hi - lo); //用O(log_phi(n = hi - lo)时间创建Fib数列
	while (lo < hi) //每步迭代可能要做两次比较判断，有三个分支
	{
		while (hi - lo < fib.get())
			fib.prev(); //通过向前顺序查找（分摊O(1)）——至多迭代几次？
		Rank mi = lo + fib.get() - 1; //确定形如Fib(k) - 1的轴点
		(e < A[mi]) ? hi = mi : lo = mi + 1;
	} //成功查找可以提前终止
	return --lo; //查找失败
} //有多个命中元素时，不能保证返回秩最大者；失败时，简单地返回-1，而不能指示失败的位置


template <typename T>
static Rank simpleFibSearch(T* A, T const& e, Rank lo, Rank hi)
{
	Fib fib(hi - lo); //用O(log_phi(n = hi - lo)时间创建Fib数列
	while (lo < hi) //每步迭代可能要做两次比较判断，有三个分支
	{
		while (hi - lo < fib.get())
			fib.prev(); //通过向前顺序查找（分摊O(1)）——至多迭代几次？
		Rank mi = lo + fib.get() - 1; //确定形如Fib(k) - 1的轴点
		if (e < A[mi])
			hi = mi; //深入前半段[lo, mi)继续查找
		else if (A[mi] < e)
			lo = mi + 1; //深入后半段(mi, hi)继续查找
		else
			return mi; //在mi处命中	
	} //成功查找可以提前终止
	return -1; //查找失败
} //有多个命中元素时，不能保证返回秩最大者；失败时，简单地返回-1，而不能指示失败的位置


template<typename T>
void Vector<T>::traverse(void(*)(T &))
{
	for (int i = 0; i < _size; i++) visit(_elem[i]); //遍历向量
}


template <typename T> //元素类型
template <typename VST> //操作器
void Vector<T>::traverse(VST& visit) //倚劣函数对象机刢
{
	for (int i = 0; i < _size; i++) visit(_elem[i]); //遍历向量
}


#endif 