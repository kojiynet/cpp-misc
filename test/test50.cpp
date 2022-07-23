
/*
	test50.cpp

	valarrayを使うと、簡潔に書けるが、やはり遅い。
	…ということを言おうとしている。

*/


/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <valarray>
#include <k09/kutil02.cpp>
#include <k09/krand02.cpp>
#include <k09/kalgo03.cpp>
#include <k09/ksvggraph00.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);


/* ********** Class Definitions ********** */


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

/*
void floor_overwrite( std::vector <double> &vec0)
{
	for ( auto &v : vec0){
		v = std::floor( v);
	}
}


void floor_overwrite_foreach( std::vector <double> &vec0)
{

	std::for_each(
		vec0.begin(), vec0.end(),
		[]( double &v){ v = std::floor( v); }
	);

}


std::vector <double> floor_copy( const std::vector <double> &vec0)
{

	std::vector <double> ret = vec0;
	std::for_each(
		ret.begin(), ret.end(),
		[]( double &v){ v = std::floor( v); }
	);
	return ret;

}


std::vector <double> floor_move( const std::vector <double> &vec0)
{

	std::vector <double> ret = vec0;
	std::for_each(
		ret.begin(), ret.end(),
		[]( double &v){ v = std::floor( v); }
	);
	return std::move( ret);

}

*/

// vectorの各要素をostreamに出力する。区切りはスペース。
template <typename T>
std::ostream & operator<<( std::ostream &ost, std::vector <T> &vec)
{
	for ( const auto &v : vec){
		ost << v << " ";
	}
	return ost;
}

// valarrayの各要素をostreamに出力する。区切りはスペース。
template <typename T>
std::ostream & operator<<( std::ostream &ost, std::valarray <T> &vec)
{
	for ( const auto &v : vec){
		ost << v << " ";
	}
	return ost;
}

// vectorの各要素を、sep区切りで、stringに変換する。
template <typename T>
std::string vectorStr( const std::vector <T> &vec, const std::string &sep)
{

	int n = vec.size();
	if ( n < 1){
		return std::string();
	}

	std::stringstream ss;
	ss << vec[ 0];
	for ( int i = 1; i < n; ++i){
		ss << sep;
		ss << vec[ i];
	}
	return ss.str();

}

// valarrayの各要素を、sep区切りで、stringに変換する。
template <typename T>
std::string vectorStr( const std::valarray <T> &vec, const std::string &sep)
{

	int n = vec.size();
	if ( n < 1){
		return std::string();
	}

	std::stringstream ss;
	ss << vec[ 0];
	for ( int i = 1; i < n; ++i){
		ss << sep;
		ss << vec[ i];
	}
	return ss.str();

}

int main( int argc, char *argv[])
{

	using namespace std;

	/*
	int nobs = 50'000'000;
	RandomNumberEngine rne( 123);
	cerr << "Generating random numbers...";
	vector <double> vec1 = rne.getRealUniformSeq( nobs, 0.0, 100.0);
	vector <double> vec2 = rne.getRealUniformSeq( nobs, 0.0, 100.0);
	cerr << " end." << endl;
	*/

	int nobs = 100;

	vector <double> vec1( nobs);
	iota( vec1.begin(), vec1.end(), 0.0);
	cout << "vec1: " << vec1 << endl;
	vector <double> vec2( nobs);
	iota( vec2.begin(), vec2.end(), 0.0);
	cout << "vec2: " << vec2 << endl;
	cout << "vec2: " << vectorStr( vec2, ", ") << endl;
	

	int idx;
	Timer tm;
	long int interval;

	// vectorを用いた演算

	// vector要素同士の加算の場合。

	// 加算して他オブジェクトに代入する。
	// for文で書く。
	{
		vector <double> ans( nobs);
		for ( int i = 0; i < nobs; ++i){
			ans[ i] = vec1[ i] + vec2[ i];
		}
		cout << "ans: " << vectorStr( ans, ", ") << endl;
	}

	// 加算して他オブジェクトに代入
	// transform()を使う。
	{
		vector <double> ans( nobs);
		std::transform(
			vec1.begin(), vec1.end(), vec2.begin(), ans.begin(),
			[]( double a, double b){ return a + b;}
		);
		cout << "ans: " << vectorStr( ans, ", ") << endl;
	}

	// valarrayをつかう。
	// transformを使うバージョン。
	{

		valarray <double> ans( nobs);

		// まずvalarrayにコピーする。
		valarray <double> va1( vec1.size());
		copy( vec1.begin(), vec1.end(), std::begin( va1));
		valarray <double> va2( vec2.size());
		copy( vec2.begin(), vec2.end(), std::begin( va2));

		// 加算する。
		std::transform(
			std::begin( va1), std::end( va1), std::begin( va2), std::begin( ans),
			[]( double a, double b){ return a + b;}
		);

		cout << "ans: " << vectorStr( ans, ", ") << endl;

	}
	// ↑コピーするときの速度も測れるようにしておく。

	// valarrayをつかう。
	// 演算子を使うバージョン。
	{

		valarray <double> ans( nobs);

		// まずvalarrayにコピーする。
		valarray <double> va1( vec1.size());
		copy( vec1.begin(), vec1.end(), std::begin( va1));
		valarray <double> va2( vec2.size());
		copy( vec2.begin(), vec2.end(), std::begin( va2));

		// 加算する。
		ans = va1 + va2;

		cout << "ans: " << vectorStr( ans, ", ") << endl;

	}
	// ↑コピーするときの速度も測れるようにしておく。




	// 自己オブジェクトを定数倍　→valarrayならapplyを使える。
	double mul = 0.1;


/*


	valarray <double> va_touse( vec.size());
	copy( vec.begin(), vec.end(), std::begin( va_touse));
	va_touse = va_touse.apply( []( double x){ return std::floor( x);});

	idx = rne.getDiscreteUniform( 0, nobs);
	cout << " dummyresult:" << va_touse[ idx];
*/




	return 0;
	

	/*

	// 関数を呼ぶのではなくその場でfor文で処理。
	auto test0proc = [&]()->long int {
		
		cout << "test0";

		vector <double> vec_touse = vec;

		tm.restart();
		for ( auto &v : vec_touse){
			v = floor( v);
		}
		tm.markEnd();

		idx = rne.getDiscreteUniform( 0, nobs);
		cout << " dummyresult:" << vec_touse[ idx];

		interval = tm.getInterval();
		cout << " Duration: " << interval << " milliseconds" << endl;

		return interval;

	};

	// 関数を呼んでその中でfor文で処理。
	auto test1proc = [&]()->long int {
		
		cout << "test1";

		vector <double> vec_touse = vec;

		tm.restart();
		floor_overwrite( vec_touse);
		tm.markEnd();

		idx = rne.getDiscreteUniform( 0, nobs);
		cout << " dummyresult:" << vec_touse[ idx];

		interval = tm.getInterval();
		cout << " Duration: " << interval << " milliseconds" << endl;

		return interval;

	};

	// 関数を呼んでその中でstd::for_each文で処理。
	auto test2proc = [&]()->long int {
		
		cout << "test2";

		vector <double> vec_touse = vec;

		tm.restart();
		floor_overwrite_foreach( vec_touse);
		tm.markEnd();

		idx = rne.getDiscreteUniform( 0, nobs);
		cout << " dummyresult:" << vec_touse[ idx];

		interval = tm.getInterval();
		cout << " Duration: " << interval << " milliseconds" << endl;

		return interval;

	};

	// 関数を呼んでその中で処理させて、コピーを返してもらう。
	auto test3proc = [&]()->long int {

		cout << "test3";

		vector <double> vec_touse = vec;

		tm.restart();
		vector <double> ans = floor_copy( vec_touse);
		tm.markEnd();

		idx = rne.getDiscreteUniform( 0, nobs);
		cout << " dummyresult:" << ans[ idx];

		interval = tm.getInterval();
		cout << " Duration: " << interval << " milliseconds" << endl;

		return interval;

	};

	// 関数を呼んでその中で処理させて、ムーブで返してもらう。
	auto test4proc = [&]()->long int {

		cout << "test4";

		vector <double> vec_touse = vec;

		tm.restart();
		vector <double> ans = floor_move( vec_touse);
		tm.markEnd();

		idx = rne.getDiscreteUniform( 0, nobs);
		cout << " dummyresult:" << ans[ idx];

		interval = tm.getInterval();
		cout << " Duration: " << interval << " milliseconds" << endl;

		return interval;

	};



	auto test_n = [&]( int n)->long int {
		if ( n == 0){
			return test0proc();
		} else if ( n == 1){
			return test1proc();
		} else if ( n == 2){
			return test2proc();
		} else if ( n == 3){
			return test3proc();
		} else if ( n == 4){
			return test4proc();
		} else if ( n == 5){
			return test5proc();
		}
		return numeric_limits<double>::quiet_NaN();
	};

	int niter = 10;

	struct VecDouble {
		vector <double> vec;
	};
	vector <VecDouble> results( 6);
	for ( auto &dvec : results){
		dvec.vec.reserve( niter);
	}

	for ( int i = 0; i < niter; i++){
		vector <int> seq( 6);
		iota( seq.begin(), seq.end(), 0);
		randomizeOrder( seq, rne);
		cout << "iter: " << i << endl;
		for ( auto k : seq){
			double ret = static_cast<double>( test_n( k));
			results[ k].vec.push_back( ret);
		}
		cout << endl;
	}
	
	for ( auto &dvec : results){
		for ( auto &d : dvec.vec){
			cout << d << " ";
		}
		cout << endl;
	}


	// 以下、グラフ化を試す。

	vector <double> xvec( niter);
	iota( xvec.begin(), xvec.end(), 1.0);

	{
		string graph_title = "test0 Results";
		string xaxis_title = "# Iteration";
		string yaxis_title = "Duration (millisecond)";

		SvgScatterMaker maker( xvec, results[ 0].vec);

		maker.setGraphTitle( graph_title);
		maker.setXAxisTitle( xaxis_title); 	
		maker.setYAxisTitle( yaxis_title); 

		SvgGraph svgg = maker.createGraph(); 
		svgg.writeFile( "test50_out_0.svg");
	}

	{
		string graph_title = "test1 Results";
		string xaxis_title = "# Iteration";
		string yaxis_title = "Duration (millisecond)";

		SvgScatterMaker maker( xvec, results[ 1].vec);

		maker.setGraphTitle( graph_title);
		maker.setXAxisTitle( xaxis_title); 	
		maker.setYAxisTitle( yaxis_title); 

		SvgGraph svgg = maker.createGraph(); 
		svgg.writeFile( "test50_out_1.svg");
	}

	{
		string graph_title = "test2 Results";
		string xaxis_title = "# Iteration";
		string yaxis_title = "Duration (millisecond)";

		SvgScatterMaker maker( xvec, results[ 2].vec);

		maker.setGraphTitle( graph_title);
		maker.setXAxisTitle( xaxis_title); 	
		maker.setYAxisTitle( yaxis_title); 

		SvgGraph svgg = maker.createGraph(); 
		svgg.writeFile( "test50_out_2.svg");
	}

	{
		string graph_title = "test3 Results";
		string xaxis_title = "# Iteration";
		string yaxis_title = "Duration (millisecond)";

		SvgScatterMaker maker( xvec, results[ 3].vec);

		maker.setGraphTitle( graph_title);
		maker.setXAxisTitle( xaxis_title); 	
		maker.setYAxisTitle( yaxis_title); 

		SvgGraph svgg = maker.createGraph(); 
		svgg.writeFile( "test50_out_3.svg");
	}

	{
		string graph_title = "test4 Results";
		string xaxis_title = "# Iteration";
		string yaxis_title = "Duration (millisecond)";

		SvgScatterMaker maker( xvec, results[ 4].vec);

		maker.setGraphTitle( graph_title);
		maker.setXAxisTitle( xaxis_title); 	
		maker.setYAxisTitle( yaxis_title); 

		SvgGraph svgg = maker.createGraph(); 
		svgg.writeFile( "test50_out_4.svg");
	}

	{
		string graph_title = "test5 Results";
		string xaxis_title = "# Iteration";
		string yaxis_title = "Duration (millisecond)";

		SvgScatterMaker maker( xvec, results[ 5].vec);

		maker.setGraphTitle( graph_title);
		maker.setXAxisTitle( xaxis_title); 	
		maker.setYAxisTitle( yaxis_title); 

		SvgGraph svgg = maker.createGraph(); 
		svgg.writeFile( "test50_out_5.svg");
	}

	{

		string graph_title = "test50.cpp Results";
		string xaxis_title = "# Iteration";
		string yaxis_title = "Duration (millisecond)";
		
		SvgGraph svgg( 500, 500, 0, 0, 500, 500);

		auto xminmax = minmax_element( xvec.begin(), xvec.end());
		double xmin = *( xminmax.first);
		double xmax = *( xminmax.second);

		vector <double> ymins;
		vector <double> ymaxs;
		for ( auto &dvec : results){
			auto yminmax = minmax_element( dvec.vec.begin(), dvec.vec.end());
			ymins.push_back( *( yminmax.first));
			ymaxs.push_back( *( yminmax.second));
		}
		auto ymin_minmax = minmax_element( ymins.begin(), ymins.end());
		double ymin = *( ymin_minmax.first);
		auto ymax_minmax = minmax_element( ymaxs.begin(), ymaxs.end());
		double ymax = *( ymax_minmax.second);

		svgg.setDefaultCoordAndGridpoints( xmin, xmax, ymin, ymax);

		svgg.setGraphTitle( graph_title);
		svgg.setXAxisTitle( xaxis_title);
		svgg.setYAxisTitle( yaxis_title);

		svgg.prepareGraph();


		// 描画領域への描画開始

		svgg.startDrawingGraphPane();

		svgg.drawPoints( xvec, results[ 0].vec, "black");
		svgg.drawPoints( xvec, results[ 1].vec, "blue");
		svgg.drawPoints( xvec, results[ 2].vec, "green");
		svgg.drawPoints( xvec, results[ 3].vec, "red");
		svgg.drawPoints( xvec, results[ 4].vec, "purple");
		svgg.drawPoints( xvec, results[ 5].vec, "yellow");

		svgg.endDrawingGraphPane();

		// 描画領域への描画終了

		svgg.writeFile( "test50_out_all.svg");

	}

	*/


}


/* ********** Definitions of Member Functions ********** */

