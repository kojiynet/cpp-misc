
/*
	test47.cpp

	Stroustrup _A Tour of C++ (2nd ed)_
	Ch.4のテスト

	・多値返却の練習
	　C++17以降。
	　構造化束縛 (structured binding) と、
	　波カッコによるreturn文での初期化を使える。

	・initializer-list
	　{ a, b, c}形式で初期化子リストを書くと、これは
	　std::initializer_list<T>のオブジェクトとして扱われる。
	　ユーザ定義クラスのコンストラクタで使える。

	・overrideキーワード
	　オーバーライドしているメソッド以外にoverrideが
	　修飾されていたらコンパイルエラーになる。

	・実行時型情報
	　実行時にtypeid()で型を比較できる。
	　（本にはキャストして比較する方法が記されていたが）
	
*/


/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <vector>
#include <tuple>
#include <initializer_list>
#include <typeinfo>


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */

class Myvector {

private:

	vector <double> vec;

public:

	Myvector( initializer_list <double> d0)
	 : vec( d0.begin(), d0.end())
	{}

	void print( void)
	{
		for ( auto v : vec){
			cout << " " << v;
		}
		cout << endl;
	}

};

class Base {
public:
	virtual void print( void) = 0;
};

class Derived : public Base {
public:
	void print( void) override { cout << " print() of Derived class" << endl; }

	// Compilation error: this is not a method that we "override"
//	void print2( void) override { cout << " print2() of Derived class" << endl; }

};

class Derived2 : public Base {
public:
	void print( void) override { cout << " print() of Derived2 class" << endl; }
};


/* ********** Global Variables ********** */



/* ********** Definitions of Static Member Variables ********** */



/* ********** Function Definitions ********** */

tuple <int, string, double> func1( void)
{

//	return ( 123, "func1test", 45.6); // compilation error!

	// "initializer-list" works well with return statement.
	return { 123, "func1test", 45.6};

} 

int main( int argc, char *argv[])
{

	cout << endl;

	// p.50 
	// returning multiple values 
	// note that in func1(), we wrote:
	//	 return { 123, "func1test", 45.6};
	// !! Only effective in C++17 or later !!
	{

		cout << "Part 1" << endl;

		auto [ k1, str1, v1] = func1();
		cout << " k1: " << k1 << endl <<  " str1: " << str1 << endl << " v1: " << v1 << endl;
		cout << endl;

	}

	// pp.52-53
	// "initializer-list" can be used by user-defined classes
	{

		cout << "Part 2" << endl;

		Myvector myv = { 12.3, 45.6, 78.9};
		myv.print();
		cout << endl;

	}
	
	// p.55
	// keyword "override" can be used to explicitly specify a method to override
	{

		cout << "Part 3" << endl;

		Derived obj;
		obj.print();
		cout << endl;

	}

	// p.61
	// Comparing types at runtime 
	{

		cout << "Part 4" << endl;

		Derived der;
		Derived2 der2;

		cout << " typeid( der) == typeid( Base): " << ( typeid( der) == typeid( Base)) << endl;
		cout << " typeid( der) == typeid( Derived): " << ( typeid( der) == typeid( Derived)) << endl;
		cout << " typeid( der) == typeid( der2): " << ( typeid( der) == typeid( der2)) << endl;
		cout << endl;

	}

	cout << endl;

	return 0;
	
}


/* ********** Definitions of Member Functions ********** */

