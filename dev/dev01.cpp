
/*

	dev01.cpp

	compile options:
	-EHsc -utf-8 -std:c++17

	開発していく用のフォルダ dev
	このファイルでは、aichiの
	　21a-qa-14.cpp　～　21a-qa-26.cpp
	について、もう少し簡単に書けるように、
	使いまわせそうな一般的なモジュールをつくってみる。
	そしてkoliに加える。

	新しい方のファイルから改善していく。
	まず21a-qa-26.cppから。

	・SimpleDatasetをつくる。
	　Resultを入れる→OK
	　乱数でできたデータセットそのものを格納したい→OK
	　母集団統計量をデータセットから出す→OK
	　サンプリングをデータセットからする→OK
	　もとのコードをうしろからコメントアウトしていって確認→OK
	　getVectorIf()の別バージョン→書いた
	
	　koutputfileのインスタンスをostreamとして扱えるようにしたい。

*/


/* ********** Preprocessor Directives ********** */

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <tuple>
#include <algorithm>
#include <cmath>
#include <execution>
#include <boost/math/distributions/beta.hpp>
#include <k09/krand02.cpp>
#include <k09/kdataset03.cpp>
#include <k09/kstat02.cpp>
#include <k09/kstatboost02.cpp>
#include <k09/koutputfile00.cpp>
#include <k09/kalgo02.cpp>
#include <k09/kexcept00.cpp>
#include <k09/kutil02.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

class MyCase;
class SimpleDataset;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);
int aichi26( void);

// overwrite each element with the floored value
void floor( std::vector <double> &vec0)
{
	std::for_each(
		vec0.begin(), vec0.end(),
		[]( double& v){ v = std::floor( v); }
	);
}

// overwrite each element with the value multiplied by d0 
void mulBy( std::vector <double> &vec0, double d0)
{
	std::for_each(
		vec0.begin(), vec0.end(),
		[=]( double& v){ v *= d0; }
	);
}


/* ********** Class Definitions ********** */

class SimpleDataset {

public:

	/* ***** Internal Classes ***** */

	struct SimpleDataColumn {
		
		std::string vname;
		std::vector <double> vals;
		
		SimpleDataColumn( void) : vname(), vals(){}
		~SimpleDataColumn( void){}

		std::string getValueStringFormat( int r0)
		{
			
			if ( r0 >= vals.size()){
				throwMsgExcept( "", "index out of bound");
			}

			if ( isnan( vals[ r0])){
				return ".";
			}

			std::stringstream ss;
			ss << vals[ r0];
			return ss.str();

		}

	};
	
	/* ***** Data Fields ***** */

	std::vector <SimpleDataColumn> dcvec;

	/* ***** Methods ***** */

	SimpleDataset( void) : dcvec(){};
	SimpleDataset( const SimpleDataset &obj0) : dcvec()
	{
		copyFrom( obj0);
	}

	~SimpleDataset( void){};

	SimpleDataset & operator=( const SimpleDataset &obj0)
	{
		copyFrom( obj0);
		return *this; 
	}

	void copyFrom( const SimpleDataset &obj0)
	{
		dcvec = obj0.dcvec; 
	}

	void setVarNames( const std::vector <std::string> &vec0)
	{
		dcvec.clear();
		dcvec.resize( vec0.size());
		for ( int i = 0; i < vec0.size(); i++){
			dcvec[ i].vname = vec0[ i];
		}
	}

	// add only a value for one-case one-variable 
	// throws exception if the variable does not exist 
	// this may cause the dataset non-rectangular 
	// using only doubles as far 
	void addCase( const std::string &vn0, double v0)
	{
		
		auto [ b, idx] = getColumnIndex( vn0);
		if ( b == true){
			dcvec[ idx].vals.push_back( v0);
		} else {
			throwMsgExcept( "", "variable not found");
		}

	}

	// add a vector for one-variable 
	// throws exception if the variable does not exist 
	// this will usually cause the dataset non-rectangular 
	// using only doubles as far 
	void addVector( const std::string &vn0, const std::vector <double> &vec0)
	{
		
		auto [ b, idx] = getColumnIndex( vn0);
		if ( b == true){
			auto &alias = dcvec[ idx].vals;
			alias.insert( alias.end(), vec0.begin(), vec0.end());
		} else {
			throwMsgExcept( "", "variable not found");
		}

	}

	// add a set of sequntial numbers
	// starting "s0", of size "len0"
	// throws exception if the variable does not exist 
	// this will usually cause the dataset non-rectangular 
	// using only doubles as far 
	void addSequentialNumber( const std::string &vn0, double s0, int len0)
	{
		
		auto [ b, idx] = getColumnIndex( vn0);

		if ( b == true){

			std::vector <double> vec0( len0);
			std::iota( vec0.begin(), vec0.end(), s0);
			auto &alias = dcvec[ idx].vals;
			alias.insert( alias.end(), vec0.begin(), vec0.end());

		} else {

			throwMsgExcept( "", "variable not found");

		}

	}

	// add a vector of a constatn "c0" of the specified size "len0" 
	// throws exception if the variable does not exist 
	// this will usually cause the dataset non-rectangular 
	// using only doubles as far 
	void addConstant( const std::string &vn0, double c0, int len0)
	{
		
		auto [ b, idx] = getColumnIndex( vn0);

		if ( b == true){

			std::vector <double> vec0( len0, c0);
			auto &alias = dcvec[ idx].vals;
			alias.insert( alias.end(), vec0.begin(), vec0.end());

		} else {

			throwMsgExcept( "", "variable not found");

		}

	}

	// returns the vector of the variable named "vn0"
	// throws exception if the variable does not exist 
	std::vector <double> getVector( const std::string &vn0)
	{

		auto [ b, idx] = getColumnIndex( vn0);

		if ( b == false){
			throwMsgExcept( "", "variable not found");
		}

		return dcvec[ idx].vals;

	}

	// returns the vector of the variable named "vn0", only for the cases where 
	// the value of "ifvn0" satisfies the condition given by the function "func0"
	// throws exception if the variable does not exist 
	std::vector <double> getVectorIf( const std::string &vn0, const std::string &ifvn0, std::function <bool(double)> func0)
	{

		auto [ b1, idx1] = getColumnIndex( vn0);
		if ( b1 == false){
			throwMsgExcept( "", "variable not found: " + vn0);
		}

		auto [ b2, idx2] = getColumnIndex( ifvn0);
		if ( b2 == false){
			throwMsgExcept( "", "variable not found: " + ifvn0);
		}

		const auto &vals_to_return = dcvec[ idx1].vals;
		const auto &vals_for_cond = dcvec[ idx2].vals;

		int len = vals_to_return.size();
		if ( len != vals_for_cond.size()){
			throwMsgExcept( "", "sizes of two variables not equal");
		}

		std::vector <double> ret;
		for ( int i = 0; i < len; i++){
			if ( func0( vals_for_cond[ i]) == true){
				ret.push_back( vals_to_return[ i]);
			}
		}

		return ret;
		
	}

	// returns the vector of the variable named "vn0", only for the cases where 
	// the certain condition given by "func0" is satifcied
	// "func0" will return bool taking as input the map
	// which includes varnames and values
	// throws exception if the variable to use does not exist 
	// 
	// inside "func0" we should use map::at() to throw exception for erroneous varname
	// very slow because for each case one map object will be created
	std::vector <double>
	getVectorIf( 
		const std::string &vn0,
		std::function < bool( std::map <std::string, double> ) > func0
	)
	{

		auto [ b, idx] = getColumnIndex( vn0);
		if ( b == false){
			throwMsgExcept( "", "variable not found: " + vn0);
		}

		const auto &vals_to_return = dcvec[ idx].vals; // vector <double> & 

		int len = vals_to_return.size();

		std::vector <double> ret;
		ret.reserve( len);

		std::map <std::string, double> casemap;
		for ( const auto &vec : dcvec){
			casemap.insert( std::make_pair( vec.vname, 0.0));
		}

		try {

			for ( int i = 0; i < len; i++){

				for ( const auto &vec : dcvec){
					casemap.at( vec.vname) = vec.vals[ i];
				}

				if ( func0( casemap) == true){
					ret.push_back( vals_to_return[ i]);
				}

			}

		} catch ( const std::out_of_range &e) {

			throwMsgExcept( "", "variable used in condition func was not found");

		}

		return ret;
		
	}	

	// returns the vector of the variable named "vn0", only for the cases 
	// where the certain condition given by "func0" is satifcied
	// "func0" will return bool taking as input varname->double func object
	// throws exception if the variable to use does not exist 
	std::vector <double>
	getVectorIf( 
		const std::string &vn0,
		std::function < bool( std::function <double(std::string)> ) > func0
	)
	{

		auto [ b, idx] = getColumnIndex( vn0);
		if ( b == false){
			throwMsgExcept( "", "variable not found: " + vn0);
		}

		const auto &vals_to_return = dcvec[ idx].vals; // vector <double> & 

		int len = vals_to_return.size();

		std::vector <double> ret;
		ret.reserve( len);

		for ( int i = 0; i < len; i++){

			std::function < double( std::string) > casevaluefunc =
				[&]( std::string vn0)->double
				{
					return value( vn0, i);
				};
			
			if ( func0( casevaluefunc) == true){
				ret.push_back( vals_to_return[ i]);
			}

		}

		return ret;

	}

	// accessor
	double & value( const std::string &vn0, int caseid0)
	{

		auto [b, idx] = getColumnIndex( vn0);
		if ( b == false){
			throwMsgExcept( "", "variable not found");
		}
		auto &alias = dcvec[ idx].vals;
		if ( caseid0 < 0 || caseid0 >= alias.size()){
			throwMsgExcept( "", "case id out of bound");
		}
		
		return alias[ caseid0];

	} 

	// returns:
	//   bool: true if the variable exists
	//   int: index of the column for the variable if it exists; 0 if not
	std::tuple <bool, int> getColumnIndex( const std::string &vn0)
	{

		for ( int i = 0; i < dcvec.size(); i++){
			if ( dcvec[ i].vname == vn0){
				return { true, i};
			}
		}

		return { false, 0};

	}

	// check if dataset is recutanglar
	// returns true if reculangular (or there is no column)
	// typically useful after calling addCase() 
	bool isRectanglar( void)
	{

		if ( dcvec.size() < 1){
			return true;
		}

		int n = dcvec[ 0].vals.size();
		for ( int i = 1; i < dcvec.size(); i++){
			if ( dcvec[ i].vals.size() != n){
				return false;
			}
		}

		return true;

	}
	
	// throws exception if non-rectangular 
	void assertRecutangular( void)
	{
		if ( isRectanglar() == false){
			throwMsgExcept( "", "dataset is not rectangular");
		}
	}

	void writeToFile( koutputfile &kof0, const std::string &sep0 = "\t")
	{

		if ( dcvec.size() < 1){
			return;
		}

		// write variable names 
		{
			std::string s;
			s = dcvec[ 0].vname;
			for ( int i = 1; i < dcvec.size(); i++){
				s += sep0;
				s += dcvec[ i].vname;
			}
			kof0.writeLine( s);
		}

		// write values
		{
			for ( int i_row = 0; i_row < dcvec[ 0].vals.size(); i_row++){

				int i_col = 0;

				std::stringstream ss;
				ss << dcvec[ i_col].getValueStringFormat( i_row);

				for ( i_col = 1; i_col < dcvec.size(); i_col++){
					ss << sep0;
					ss << dcvec[ i_col].getValueStringFormat( i_row);
				}

				kof0.writeLine( ss.str());

			}
		}

	}

	// returns the length of the longetst column
	// this works even if no variable exists 
	// or the dataset is non-rectangular
	int size( void) const 
	{
		
		if ( dcvec.size() < 1){
			return 0;
		}

		int ret = dcvec[ 0].vals.size();
		for ( int i = 1; i < dcvec.size(); i++){
			int n = dcvec[ i].vals.size();
			if ( n > ret){
				ret = n;
			}
		}

		return ret;

	}

	// *thisを母集団として、標本を返す。非復元抽出による。
	// *thisの要素から、ランダムに（rneを用いて）、ssizeだけの要素を選び、同じ型にして返す。
	// kalgo.cppにあるgetSample()と同様のアルゴリズムで実装されている。
	SimpleDataset getSample( RandomNumberEngine &rne, int ssize)
	{

		int npop = size();

		if ( ssize > npop){
			throwMsgExcept( "sample size should not exceed population size");
		}

		std::map <int, bool> idmap;

		int ndrawn = 0;
		while ( ndrawn < ssize){
			int randomid = rne.getDiscreteUniform( 0, npop - 1);
			auto it = idmap.find( randomid);
			if ( it == idmap.end()){
				idmap[ randomid] = true;
				ndrawn++;
			}
		}

		SimpleDataset ret{};

		for ( const auto &dc : dcvec){

			SimpleDataColumn newdc{};
			newdc.vname = dc.vname;
			newdc.vals.resize( ssize);

			auto it = idmap.begin();
			for ( int i = 0; i < ssize; i++){
				newdc.vals[ i] = dc.vals[ it -> first];
				it++;
			}

			ret.dcvec.push_back( newdc);

		}

		return ret;

	}

};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int argc, char *argv[])
{

	int ret = 0;

	try {

		if ( ret == 0){
			ret = aichi26();
		}

	} catch ( const std::exception &e){

		cout << e.what() << endl;

	}

	return ret; 

}


int aichi26( void)
{

	using namespace std;

	// 出力ファイル名の指定
	std::string output_fn1 = "21a-qa-26out1.txt";
	std::string output_fn2 = "21a-qa-26out2.txt";

	// サンプリング回数の指定（サンプルサイズごとに）
	int nsim = 1; 

	// サンプリング時のサンプルサイズの指定
	std::vector <int> nobsvec;
	nobsvec.resize( 998);
	std::iota( nobsvec.begin(), nobsvec.end(), 3);


	// 母集団の生成。Generating Beta Distribution 

	RandomNumberEngine rne( 123); 

	double mu;
	double sigma;

	std::cout << "Generating random numbers..." << std::endl;

	vector <double> score_qayes_m;
	vector <double> score_qayes_f;
	mu = 70.0 / 100.0;
	sigma = 10.0 / 100.0;
	// ↑ここで100で割るのは、本来Betaは[0,1]だから。
	// 　なので下では戻す計算をする。
	getBetaRandomVec( score_qayes_m, rne, 250'000, mu, sigma * sigma, true);
	mulBy( score_qayes_m, 100.0);
	floor( score_qayes_m);
	getBetaRandomVec( score_qayes_f, rne, 250'000, mu, sigma * sigma, true);
	mulBy( score_qayes_f, 100.0);
	floor( score_qayes_f);

	vector <double> score_qano_m;
	vector <double> score_qano_f;
	mu = 60.0 / 100.0;
	sigma = 10.0 / 100.0;
	getBetaRandomVec( score_qano_m, rne, 250'000, mu, sigma * sigma, true);
	mulBy( score_qano_m, 100.0);
	floor( score_qano_m);
	getBetaRandomVec( score_qano_f, rne, 250'000, mu, sigma * sigma, true);
	mulBy( score_qano_f, 100.0);
	floor( score_qano_f);

	
	// 母集団データをSimpleDatasetにつめる

	Timer tm;
	tm.restart();

	SimpleDataset popds; 
	std::vector <std::string> popvnvec { 
		"id", "gender", "qaclass", "score"
	};
	popds.setVarNames( popvnvec);

	{
		
		int currentsize = popds.size(); // 最長コラムの長さ

		vector <double> scorevec = score_qayes_m;
		int len = scorevec.size();

		popds.addSequentialNumber( "id", ( double)currentsize, len);
		popds.addConstant( "gender", 1.0, len);
		popds.addConstant( "qaclass", 1.0, len);
		popds.addVector( "score", scorevec);
		popds.assertRecutangular();

	}

	{
		
		int currentsize = popds.size(); // 最長コラムの長さ

		vector <double> scorevec = score_qayes_f;
		int len = scorevec.size();

		popds.addSequentialNumber( "id", ( double)currentsize, len);
		popds.addConstant( "gender", 2.0, len);
		popds.addConstant( "qaclass", 1.0, len);
		popds.addVector( "score", scorevec);
		popds.assertRecutangular();

	}

	{
		
		int currentsize = popds.size(); // 最長コラムの長さ

		vector <double> scorevec = score_qano_m;
		int len = scorevec.size();

		popds.addSequentialNumber( "id", ( double)currentsize, len);
		popds.addConstant( "gender", 1.0, len);
		popds.addConstant( "qaclass", 0.0, len);
		popds.addVector( "score", scorevec);
		popds.assertRecutangular();

	}

	{
		
		int currentsize = popds.size(); // 最長コラムの長さ

		vector <double> scorevec = score_qano_f;
		int len = scorevec.size();

		popds.addSequentialNumber( "id", ( double)currentsize, len);
		popds.addConstant( "gender", 2.0, len);
		popds.addConstant( "qaclass", 0.0, len);
		popds.addVector( "score", scorevec);
		popds.assertRecutangular();

	}

	tm.markEnd();
	cout << "Duration for Storing: " << tm.getInterval() << " millisecond" << endl;

	// 母集団統計量

	tm.restart();

	vector <double> all_vec = popds.getVector( "score");
	
	tm.restart();
	vector <double> male_vec = 
		popds.getVectorIf(
			"score",
			[]( std::map <std::string, double> v){ return ( std::round( v.at( "gender")) == 1.0); }
		);
	tm.markEnd();
	cout << "Duration 1: " << tm.getInterval() << " millisecond" << endl;
	
	tm.restart();
	vector <double> female_vec =
		popds.getVectorIf(
			"score", 
			[]( std::function <double(std::string)> v){ return ( std::round( v( "gender")) == 2.0); }
		);
	tm.markEnd();
	cout << "Duration 2: " << tm.getInterval() << " millisecond" << endl;
	
	tm.restart();
		vector <double> qayes_vec =
		popds.getVectorIf(
			"score", "qaclass",
			[]( double qaclass){ return ( std::round( qaclass) == 1.0); }
		);
	tm.markEnd();
	cout << "Duration 3: " << tm.getInterval() << " millisecond" << endl;
	
	tm.restart();
	vector <double> qano_vec =
		popds.getVectorIf(
			"score", "qaclass",
			[]( double qaclass){ return ( std::round( qaclass) == 0.0); }
		);
	tm.markEnd();
	cout << "Duration 4: " << tm.getInterval() << " millisecond" << endl;

	tm.restart();

	{
		koutputfile kof( output_fn1);
		bool b = kof.open( false, false, true);
		if ( b == true){
			stringstream ss;
			ss << "mean"		"\t" << mean( all_vec) << endl
			   << "mean male"	"\t" << mean( male_vec) << endl
			   << "mean female"	"\t" << mean( female_vec) << endl
			   << "mean qayes"	"\t" << mean( qayes_vec) << endl
			   << "mean qano"	"\t" << mean( qano_vec) << endl
			   << "var(all)"	"\t" << unbiasedVarBoost( all_vec) << endl
			   << "var(male)"	"\t" << unbiasedVarBoost( male_vec) << endl
			   << "var(female)"	"\t" << unbiasedVarBoost( female_vec) << endl
			   << "var(qayes)"	"\t" << unbiasedVarBoost( qayes_vec) << endl
			   << "var(qano)"	"\t" << unbiasedVarBoost( qano_vec) << endl
			   << "N"			"\t" << all_vec.size() << endl
			   << "N male"		"\t" << male_vec.size() << endl
			   << "N female"	"\t" << female_vec.size() << endl
			   << "N qayes"		"\t" << qayes_vec.size() << endl
			   << "N qano"		"\t" << qano_vec.size() << endl;
			kof.writeLine( ss.str());
		}
	}

	tm.markEnd();
	cout << "Duration for calculating pop stats: " << tm.getInterval() << " millisecond" << endl;
	

	// サンプリングする
	// そのたびに統計量を算出して記録

	SimpleDataset resultds; 
	std::vector <std::string> vnvec { 
		"nobs", "iter", "n_male", "n_female", "n_qayes", "n_qano",
		"mean", "mean_male", "mean_female", "mean_qayes", "mean_qano",
		"gender_diff", "qa_diff"
	};	
	resultds.setVarNames( vnvec);

	std::cout << "Sampling..." << std::endl;

	RandomNumberEngine rne_to_sample( 456);

	for ( int nobs : nobsvec){

		for ( int j = 0; j < nsim; j++){

			SimpleDataset sampleds = popds.getSample( rne_to_sample, nobs);

			vector <double> svec = sampleds.getVector( "score");

			vector <double> svec_male = 
				sampleds.getVectorIf(
					"score", "gender",
					[]( double v){ return ( std::round( v) == 1.0); }
				);
			
			vector <double> svec_female = 
				sampleds.getVectorIf(
					"score", "gender",
					[]( double v){ return ( std::round( v) == 2.0); }
				);
			
			vector <double> svec_qayes = 
				sampleds.getVectorIf(
					"score", "qaclass",
					[]( double v){ return ( std::round( v) == 1.0); }
				);

			vector <double> svec_qano = 
				sampleds.getVectorIf(
					"score", "qaclass",
					[]( double v){ return ( std::round( v) == 0.0); }
				);

			double mean_male   = mean( svec_male);
			double mean_female = mean( svec_female);
			double mean_qayes  = mean( svec_qayes);
			double mean_qano   = mean( svec_qano);
			resultds.addCase( "nobs", nobs);
			resultds.addCase( "iter", j);
			resultds.addCase( "n_male",      svec_male.size());
			resultds.addCase( "n_female",    svec_female.size());
			resultds.addCase( "n_qayes",     svec_qayes.size());
			resultds.addCase( "n_qano",      svec_qano.size());
			resultds.addCase( "mean",        mean( svec));
			resultds.addCase( "mean_male",   mean_male);
			resultds.addCase( "mean_female", mean_female);
			resultds.addCase( "mean_qayes",  mean_qayes);
			resultds.addCase( "mean_qano",   mean_qano);
			resultds.addCase( "gender_diff", mean_male - mean_female);
			resultds.addCase( "qa_diff",     mean_qayes - mean_qano);
			resultds.assertRecutangular();
		}

	}

	// 結果を書き込む
	{
		koutputfile kof( output_fn2);
		bool s = kof.open( false, false, true);
		if ( s == true){
			
			resultds.writeToFile( kof);
			kof.close();

		}
	}

	return 0;

}


/* ********** Definitions of Member Functions ********** */ 

