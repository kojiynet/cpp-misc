
/*
	test44.cpp

	test43.cppを受けて、OpenMP用の乱数生成器などを整備。

	参考
	https://qiita.com/syngetodo/items/f6641e6fe57b8c806754

	乱数発生器をスレッド間で共有してしまうと、相関が生じてしまうようなので、
	共有しないようにした。
	おそらく、Engineを共有していると、複数のスレッドが同時にEngineにアクセスし、
	1つのスレッドが状態の一部を変えた直後に別のスレッドが値を得てしまったりして、
	乱数としての独立性が担保されなくなるから、だろう。

	コンパイル時のコマンド
	> cl test44.cpp -EHsc -openmp -Ox
	> g++ test44.cpp -std=c++17 -fopenmp -O3

*/

/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <boost/math/tools/bivariate_statistics.hpp>
#include <k09/kstat02.cpp>
#include <k09/krand00.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

class RandomNumberEngineMP;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);

int getNMaxThreads( void);
int getNUsedThreads( void);
void setNThreads( int);

double corr( const std::vector <double> &, const std::vector <double> &);
/*
double mycorr( const std::vector <double> &, const std::vector <double> &);
double sumofproducts( const std::vector <double> &, const std::vector <double> &);
double sumofsquares( const std::vector <double> &);
*/

// 最大スレッド数・使用スレッド数・スレッド番号を得る関数と
// 使用スレッド数を設定する関数を宣言
// Declare functions
// to get the maximum number of threads,
// to set the number of threads used,
// and to get ID for the current thread
#ifdef _OPENMP
#include <omp.h>
#else
int omp_get_max_threads( void);
void omp_set_num_threads( int num_threads);
int omp_get_num_threads( void);
int omp_get_thread_num( void);
#endif


/* ********** Class Definitions ********** */

class RandomNumberEngineMP {

private:

	// this instance is used to set seeds of 
	// other random number generators
	std::mt19937 rng;

	// this function is to get seed using "rng" above
	uint_fast32_t getRandomSeed( void);
	
public:
	
	RandomNumberEngineMP( void);
	RandomNumberEngineMP( unsigned int);
	~RandomNumberEngineMP( void);
	
	void setSeed( unsigned int);

	std::vector <double> getRealUniformSeq( int, double = 0.0, double = 1.0);

};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int argc, char *argv[])
{

	using time_point = std::chrono::system_clock::time_point;

	int nmaxthreads = getNMaxThreads();
	std::cout << "N Max Threads: " << nmaxthreads << std::endl << std::endl;

	int nusedthreads = getNUsedThreads();
	std::cout << "N Used Threads: " << nusedthreads << std::endl << std::endl;

	int npoints = 10000000;
	int nreps = 10;

	// Single Thread
	{

		std::cout << "Test1: Single Thread" << std::endl;
		std::cerr << "Test1: Single Thread" << std::endl;

		RandomNumberEngine rne( 12345);

		std::vector <double> result( nreps); 

		for ( int j = 0; j < nreps; j++){

			std::vector <double> xvec = rne.getRealUniformSeq( npoints, -1.0, 1.0);
			std::vector <double> yvec = rne.getRealUniformSeq( npoints, -1.0, 1.0);

			double corrv = corr( xvec, yvec);
			result[ j] = corrv;

			std::cerr << "Trial No. " << ( j + 1) << "  Corr = " << corrv << std::endl;

		}
		std::cerr << std::endl;

		std::cout << "Correlation" << std::endl;
		for ( auto v : result){
			std::cout << v << std::endl;
		}
		std::cout << std::endl;
		
	}

	// Multi Thread
	{

		std::cout << "Test2: Multi Thread" << std::endl;
		std::cerr << "Test2: Multi Thread" << std::endl;

		std::vector <double> result( nreps); 

		RandomNumberEngineMP rnemp( 12345);

		for ( int j = 0; j < nreps; j++){

			std::vector <double> xvec = rnemp.getRealUniformSeq( npoints, -1.0, 1.0);
			std::vector <double> yvec = rnemp.getRealUniformSeq( npoints, -1.0, 1.0);

			double corrv = corr( xvec, yvec);
			result[ j] = corrv;

			std::cerr << "Trial No. " << ( j + 1) << "  Corr = " << corrv << std::endl;

		}
		std::cerr << std::endl;

		std::cout << "Correlation" << std::endl;
		for ( auto v : result){
			std::cout << v << std::endl;
		}
		std::cout << std::endl;
		
	}

	return 0;
	
}


// Returns the maximum number of threads that
// OpenMP can hold in the environment 
int getNMaxThreads( void)
{
	return omp_get_max_threads();
}


// Returns the number of threads to be used
int getNUsedThreads( void)
{
	int ret;
	#pragma omp parallel
	{
		ret = omp_get_num_threads();
	}
	return ret;
}


// Sets the number of threads that
// OpenMP will use  
void setNThreads( int n0)
{
	omp_set_num_threads( n0);
}



double corr( const std::vector <double> &xvec, const std::vector <double> &yvec)
{

	double ret = boost::math::tools::correlation_coefficient( xvec, yvec);
	return ret;

}


// OpenMPを使ってコンパイルされない場合の関数の定義
// Define functions to use when compiled without OpenMP 
#ifndef _OPENMP

int omp_get_max_threads(void)
{
	return 1;
}

void omp_set_num_threads( int num_threads)
{
	return;
}

int omp_get_num_threads( void)
{
	return 1;
}

int omp_get_thread_num( void)
{
	return 0;
}

#endif // _OPENMP


/* ********** Definitions of Member Functions ********** */

/* ***** class RandomNumberEngineMP ***** */

// Default constructor
// set seed equal to 1 
RandomNumberEngineMP ::
RandomNumberEngineMP( void)
: rng( 1)
{}

// Constructor
// set seed equal to s0 
RandomNumberEngineMP ::
RandomNumberEngineMP( unsigned int s0)
{
	
	// Because seed of mt19937 is actually set by the type "unit_fast32_t",
	// the boundary will be checked;
	// In the most environments "unsigned int" is a 32-bit type.
	// UINT_FAST32_MAX is defined in <cstdint>
	
	if ( s0 > UINT_FAST32_MAX){
		
		alert( "RandomNumberEngineMP :: RandomNumberEngineMP()");
		rng.seed( 1);
		
	} else {
		
		rng.seed( s0);
		
	}
		
}

RandomNumberEngineMP ::
~RandomNumberEngineMP( void)
{}

// Setting seed as s0
void 
RandomNumberEngineMP :: 
setSeed( unsigned int s0)
{
	
	// Because seed of mt19937 is actually set by the type "uint_fast32_t",
	// the boundary will be checked;
	// In the most environments "unsigned int" is a 32-bit type.
	// UINT_FAST32_MAX is defined in <cstdint>
	
	if ( s0 > UINT_FAST32_MAX){
		
		alert( "RandomNumberEngineMP :: setSeed()");
		
	} else {
		
		rng.seed( s0);
		
	}
	
}

uint_fast32_t
RandomNumberEngineMP :: 
getRandomSeed( void)
{

	std::uniform_int_distribution <uint_fast32_t> dist( 0, UINT_FAST32_MAX);

	return dist( rng);

}


// ↓戻り値の型を&&にしてはいけない。
// それをすると関数内のみがスコープのオブジェクトの参照が返ってしまうらしい。
// vectorにはムーブコンストラクタがあるので、move()を使って返せば中身を抜き取ってくれるらしい。
std::vector <double>  
RandomNumberEngineMP :: 
getRealUniformSeq(
	int npoints,
	double lower, // = 0.0
	double upper  // = 1.0
)
{

	int nthreads = getNUsedThreads(); 

	std::vector <std::mt19937> enginevec( nthreads);
	for ( int i = 0; i < enginevec.size(); i++){
		enginevec[ i].seed( getRandomSeed());
	}

	std::vector <double> ret( npoints);

	std::uniform_real_distribution <double> dist( lower, upper);

	#pragma omp parallel for
	for ( int i = 0; i < npoints; i++){
		int tid = omp_get_thread_num();
		ret[ i] = dist( enginevec[ tid]);
	}

	return std::move( ret);

}
