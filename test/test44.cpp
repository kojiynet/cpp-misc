
/*
	test44.cpp

	test43.cppを受けて、OpenMP用の乱数生成器などを整備。

// 参考
// https://qiita.com/syngetodo/items/f6641e6fe57b8c806754

// 乱数発生器をスレッド間で共有してしまうと、相関が生じてしまうようなので、
// 共有しないようにした。
おそらく、Engineを共有していると、複数のスレッドが同時にEngineにアクセスし、
1つのスレッドが状態の一部を変えた直後に別のスレッドが値を得てしまったりして、
乱数としての独立性が担保されなくなるから、だろう。


kstatにboostを使った相関係数などを入れたい。

ktuilにchronoを使った時間計測を入れたい。

moveと&&の関係が難しい。



	XとYを1つの乱数発生器から乱数を得て、交互にXとYに割り当てる。
	・シングルスレッドのとき、XとYは無相関か？
	・OpenMPを使うとき、異なるスレッド間で同じ乱数発生器を共有すると、
	　XとYに相関が生じてしまう、、のか？
	→少なくとも、後者で相関係数が大きくなっている。

	他にアプリケーションを立ち上げているときに、OpenMPの相関が大きい、
	かもしれない。
	（今の出力ファイルは、ExcelとFireFoxとエクスプローラーとVSCodeを
	開けている状態で試行した結果である）
	
	GCCでコンパイルした場合の方が、OpenMPの相関が小さい。
	
	コンパイル時のコマンド
	> cl test43.cpp -EHsc -openmp -Ox
	> g++ test43.cpp -fopenmp -O3
	
*/

/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
#include <cstdint>
//#include <k09/kutil01.cpp>
#include <k09/kstat02.cpp>
#include <k09/krand00.cpp>

// あらかじめ最大スレッド数・スレッド番号を得る関数と
// 使用スレッド数を設定する関数を宣言（定義）
// Declare/define functions
// to get the maximum number of threads,
// to set the number of threads used,
// and to get ID for the current thread
#ifdef _OPENMP
#include <omp.h>
#else
int omp_get_max_threads(){ return 1; }
void omp_set_num_threads( int num_threads) { return; }
int omp_get_thread_num(){ return 0; }
#endif


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

class RandomNumberEngineMP;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);

int getNMaxThreads( void);
void setNThreads( int);

double corr( const std::vector <double> &, const std::vector <double> &);
double sumofproducts( const std::vector <double> &, const std::vector <double> &);
double sumofsquares( const std::vector <double> &);


/* ********** Class Definitions ********** */

class RandomNumberEngineMP {

private:

	// this instance is used to set seeds of other random number generators
	std::mt19937 rng;
	
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

	int npoints = 50000000;

	static int nthreads = getNMaxThreads();

	std::cout << "N Threads: " << nthreads << std::endl << std::endl;

	// マルチスレッド
	// 乱数列を1本の長いvectorで返すバージョン
	{
		
		std::cout << "Trial 1" << std::endl;

		time_point startt = std::chrono::system_clock::now();
	
		RandomNumberEngineMP xrnemp( 123);
		RandomNumberEngineMP yrnemp( 456);
		// ↓この実装を書く（今のはスケルトン）
		// below instances will be initialized by moved objects
		std::vector <double> xvec = xrnemp.getRealUniformSeq( npoints, -1.0, 1.0);
		std::vector <double> yvec = yrnemp.getRealUniformSeq( npoints, -1.0, 1.0); 

		time_point endt = std::chrono::system_clock::now();
		auto millisec = std::chrono::duration_cast <chrono::milliseconds> ( endt - startt);
		std::cout << millisec.count() << " milliseconds passed." << std::endl;

	}


	// シングルスレッドでの比較用
	// 乱数列を1本の長いvectorで返すバージョン
	{
		
		std::cout << "Trial 2" << std::endl;

		time_point startt = std::chrono::system_clock::now();
	
		RandomNumberEngine xrne( 123);
		RandomNumberEngine yrne( 456);
		// below instances will be initialized by moved objects
		vector <double> xvec = xrne.getRealUniformSeq( npoints, -1.0, 1.0);
		vector <double> yvec = yrne.getRealUniformSeq( npoints, -1.0, 1.0); 

		time_point endt = std::chrono::system_clock::now();
		auto millisec = std::chrono::duration_cast <chrono::milliseconds> ( endt - startt);
		std::cout << millisec.count() << " milliseconds passed." << std::endl;

	}








/*

1本の長いベクトルで返すバージョンと、スレッド数だけのベクトルにして返すバージョンをつくる。
omp_set_num_threadsで、スレッド数を指定できるようにする。


	static int num_threads = omp_get_max_threads();
	
	vector <RandomNumberEngine> rngvec( num_threads);

	#pragma omp parallel reduction(+:ninside) 
	{
		int tid = omp_get_thread_num();
		
		// staticな乱数発生器でseedを決める。
		rngvec[ tid].setSeed( ( int)( randomUniform() * 10000.0));
		
		for ( long long int i = 0; i < nblock; i++){
			double x = rngvec[ tid].getRealUniform( -1.0, 1.0);
			double y = rngvec[ tid].getRealUniform( -1.0, 1.0);
			if ( x * x + y * y <= 1.0){
				ninside++;
			}
		}
		
	}



*/

/*
	int npoints = 10000000;
	int nreps = 10;

	{

		std::cout << "Test1: Single Thread" << std::endl;
		std::cerr << "Test1: Single Thread" << std::endl;

		std::mt19937 engine( 12345);
		std::uniform_real_distribution <double> dist( 0.0, 1.0);

		std::vector <double> result( nreps); 
		for ( int j = 0; j < nreps; j++){

			std::vector <double> xvec( npoints);
			std::vector <double> yvec( npoints);

			for ( int i = 0; i < npoints; i++){
				xvec[ i] = dist( engine);
				yvec[ i] = dist( engine);
			}

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

	{

		std::cout << "Test2: Multi Thread; Naive" << std::endl;
		std::cerr << "Test2: Multi Thread; Naive" << std::endl;

		std::mt19937 engine( 12345);
		std::uniform_real_distribution <double> dist( 0.0, 1.0);


		std::vector <double> result( nreps); 

		for ( int j = 0; j < nreps; j++){

			std::vector <double> xvec( npoints);
			std::vector <double> yvec( npoints);

			#pragma omp parallel for
			for ( int i = 0; i < npoints; i++){
				xvec[ i] = dist( engine);
				yvec[ i] = dist( engine);
			}
			
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
*/

	return 0;
	
}


// Returns the maximum number of threads that
// OpenMP can hold in the environment 
int getNMaxThreads( void)
{
	return omp_get_max_threads();
}


// Sets the number of threads that
// OpenMP will use  
void setNThreads( int n0)
{
	omp_set_num_threads( n0);
}


double corr( const std::vector <double> &xvec, const std::vector <double> &yvec)
{

	int n = xvec.size();
	if ( xvec.size() != yvec.size()){
		alert( "corr()", "The two vectors are not of the same size");
		n = std::min( xvec.size(), yvec.size());
	}

	double sumprod = sumofproducts( xvec, yvec); // not defined

	double xmean = mean( xvec);
	double ymean = mean( yvec);

	double xsumsq = sumofsquares( xvec); // not defined
	double ysumsq = sumofsquares( yvec); // not defined

	double ret = ( sumprod / n - xmean * ymean) / sqrt( ( xsumsq / n - xmean * xmean) * ( ysumsq / n - ymean * ymean));

	return ret;	 

}


double sumofproducts( const std::vector <double> &xvec, const std::vector <double> &yvec)
{

	int n = xvec.size();
	if ( xvec.size() != yvec.size()){
		alert( "corr()", "The two vectors are not of the same size");
		n = std::min( xvec.size(), yvec.size());
	}

	double sumprod = 0.0;
	for ( int i = 0; i < n; i++){
		sumprod += xvec[ i] * yvec[ i];
	}

	return sumprod;

}

double sumofsquares( const std::vector <double> &vec)
{

	double sumsq = 0.0;
	for ( const auto &v : vec){
		sumsq += v * v;
	}

	return sumsq;

}

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
	
	// Because seed of mt19937 is actually set by the type "unit_fast32_t",
	// the boundary will be checked;
	// In the most environments "unsigned int" is a 32-bit type.
	// UINT_FAST32_MAX is defined in <cstdint>
	
	if ( s0 > UINT_FAST32_MAX){
		
		alert( "RandomNumberEngineMP :: setSeed()");
		
	} else {
		
		rng.seed( s0);
		
	}
	
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
	
	std::vector <double> ret( npoints);
	ret[ 0] = lower;
	ret[ 1] = upper;
	return std::move( ret);

}


