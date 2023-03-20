
/*
	test50.cpp

	cl -EHsc -utf-8 -std:c++17 test50.cpp

	std::variantのテスト
	異種混合のvectorに対して同一操作をしていくのに使える。
	すべての型に共通の操作が可能でないと、
	コンパイル時にエラーになってくれる。
*/


/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <vector>
#include <variant>


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */

class T1 {
public:
	void print( void)
	{
		cout << "T1" << endl;
	}
};

class T2 {
public:
	void print( void)
	{
		cout << "T2" << endl;
	}
};

/* ********** Global Variables ********** */



/* ********** Definitions of Static Member Variables ********** */



/* ********** Function Definitions ********** */

ostream & operator<<( ostream &os0, T1 &obj)
{
	os0 << "T1";
	return os0;
}

ostream & operator<<( ostream &os0, T2 &obj)
{
	os0 << "T2";
	return os0;
}

int main( int argc, char *argv[])
{

	vector < variant<T1, T2> > vec1;
	vec1.push_back( T1());
	vec1.push_back( T2());
	vec1.push_back( T1());
	for ( auto &obj : vec1){
		visit(
			[](auto &x) { x.print(); },
			obj
		);
	}

	vector < variant<T1, T2, double> > vec2;
	vec2.push_back( T1());
	vec2.push_back( T2());
	vec2.push_back( 0.123);
	vec2.push_back( T1());
	for ( auto &obj : vec2){
		visit(
			[](auto &x) { cout << x << " "; },
			obj
		);
	}
	cout << endl;

// 以下がコンパイルエラーになってくれる。
// double型だとx.print()がないから。
/*
	for ( auto &obj : vec2){
		visit(
			[](auto &x) { x.print(); },
			obj
		);
	}
*/

	return 0;
	
}


/* ********** Definitions of Member Functions ********** */

