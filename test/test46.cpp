
/*
	test46.cpp

	Stroustrup _A Tour of C++ (2nd ed)_
	Ch.3のテスト

	・moduleはC++20の機能
	　VCでは実験的サポートでGCCには実装されていないので、
	　ここではテストしない。

	・assert()とstatic_assert()
	　前提条件を明記するのに用いられるらしい。
	　関連して、is_integralなどの型特性の判定が、
	　テンプレートを使うときに有効そう。

	・structured binding （構造化束縛）
	　多値が返ったのを受け取るのに使える。
	　C++17だとコンパイルできる。
	　tupleでも、ユーザ定義の型でもできる。
	
*/


/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <tuple>
#include <cassert>


/* ********** Namespace Declarations/Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */



/* ********** Definitions of Static Member Variables ********** */



/* ********** Function Definitions ********** */

tuple <int, string, double> func1( void)
{
	return tuple <int, string, double> ( 123, "func1test", 45.6);
} 

typedef tuple<int, string, double> rettype;

rettype func2( void)
{
	return rettype( 789, "func2test", -98.7);
} 

struct structtype {
	
	int x; string s; double y; 
	
	structtype( int x0, const string &s0, double y0)
	: x( x0), s( s0), y( y0)
	{}

};

structtype func3( void)
{
	return structtype( 135, "func3test", 7.9);
} 


int main( int argc, char *argv[])
{


	cout << endl;

	// pp.40-41
	// assert() and static_assert()
	// static_assert() looks particularly useful 
	// when we use template and want to check T satisfies some traits.
	// In additon:
	// is_integral<> etc. seem useful in checking for generic programming
	{

		cout << "Part 1" << endl;

		cout << "Input: ";
		string str;
		cin >> str;
		assert( str.size() >= 2);
		
		// successful assertion
		static_assert( is_integral<int>::value == true);

		// produces failure at compilation time
//		static_assert( is_integral<double>::value == true);

	}

	cout << endl;

	// p.45 
	// "structured binding" can be used when we return multiple values
	// based on tuple/map or user-defined struct type
	// !! Only effective in C++17 or later !!
	{

		cout << "Part 2" << endl;

		const auto &[ k1, str1, v1] = func1();
		cout << "k1: " << k1 << " str1: " << str1 << " v1: " << v1 << endl;

		const auto &[ k2, str2, v2] = func2();
		cout << "k2: " << k2 << " str2: " << str2 << " v2: " << v2 << endl;

		const auto &[ k3, str3, v3] = func3();
		cout << "k3: " << k3 << " str3: " << str3 << " v3: " << v3 << endl;
		// k3++; // Error. This is a const variable.

	}
	
	return 0;
	
}


/* ********** Definitions of Member Functions ********** */

