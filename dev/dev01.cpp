
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

	・簡易Datasetをつくりたい。
	　SimpleDataset？
	　Resultを入れる→OK
	　次に、乱数でできたデータセットそのものを格納したい。
	　	
	　以下を目指すのだがまずdoubleのみで。途中。

	　変数名と対応させつつVectorを持っている、というような。そしてVectorとしてintかdoubleかstringのどれもあり、というような。
	　方針：
	　　SimpleDataColumnクラスをつくる。
	　　中にdoubleとintとstringのvectorを持っている。（C++17ならvariantで）
	　　中身をdoubleで得たり（stringコラムだとNaN）、stringで得たり（stringstream）できるように。
	　　　doubleの場合の桁数を指定するのはあとで。。
	　　それからの、SimpleDatasetで整形しながらostreamに出す、か、koutputfileに渡してファイルに出力。
	　　
	

*/


/* ********** Preprocessor Directives ********** */

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <tuple>
#include <boost/math/distributions/beta.hpp>
#include <k09/krand01.cpp>
#include <k09/kdataset03.cpp>
#include <k09/kstat02.cpp>
#include <k09/kstatboost01.cpp>
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


/* ********** Class Definitions ********** */

class MyCase {
public:

	int id;
	int gender;
	std::string name;
	double score;
	int qaclass; 

	void print( void){
		std::cout << id << " " << gender << " " << qaclass << " " << score << " " << name << std::endl;	
	}

};

// currently testing ******************************************
// using only doubles as far 
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
//	SimpleDataset(); // using map

	~SimpleDataset( void){};

	void setVarNames( const std::vector <std::string> &vec0)
	{
		dcvec.clear();
		dcvec.resize( vec0.size());
		for ( int i = 0; i < vec0.size(); i++){
			dcvec[ i].vname = vec0[ i];
		}
	}

//	void setVarNames( const std::string &); // to be tokenized 

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

//	void writeToStream( std::ostream &);

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

};
// currently testing ******************************************


/* ********** Global Variables ********** */



/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int argc, char *argv[])
{

	int ret = 0;
	
	if ( ret == 0){
		ret = aichi26();
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
	for ( int i = 0; i < 998; i++){
		nobsvec[ i] = i + 3;
	}

	// Generating Beta Distribution 

	RandomNumberEngine rne( 123); 

	double mu;
	double sigma;

	std::cout << "Generating random numbers..." << std::endl;

	vector <double> score_qayes_m;
	vector <double> score_qayes_f;
	mu = 70.0 / 100.0;
	sigma = 10.0 / 100.0;
	// ↓この中で時間がかかっている間、別スレッドで"."を表示していきたい気がする。
	// できれば、現状で何ケースまで抽出できたかの数を共有して、何%進捗しているかを。。
	getBetaRandomVec( score_qayes_m, rne, 250'000, mu, sigma * sigma);
	getBetaRandomVec( score_qayes_f, rne, 250'000, mu, sigma * sigma);

	vector <double> score_qano_m;
	vector <double> score_qano_f;
	mu = 60.0 / 100.0;
	sigma = 10.0 / 100.0;
	getBetaRandomVec( score_qano_m, rne, 250'000, mu, sigma * sigma);
	getBetaRandomVec( score_qano_f, rne, 250'000, mu, sigma * sigma);
	
	// データをケースのvectorにつめる
	Timer tm;
	vector <MyCase> popvec; 
	int i = 0;
	for ( auto v : score_qayes_m){
		MyCase c;
		c.id = i;
		i++;
		c.gender = 1; 
		c.qaclass = 1;
		c.score = floor( v * 100.0);
		c.name = "";
		popvec.push_back( c);
	}
	for ( auto v : score_qayes_f){
		MyCase c;
		c.id = i;
		i++;
		c.gender = 2; 
		c.qaclass = 1;
		c.score = floor( v * 100.0);
		c.name = "";
		popvec.push_back( c);
	}
	for ( auto v : score_qano_m){
		MyCase c;
		c.id = i;
		i++;
		c.gender = 1; 
		c.qaclass = 0;
		c.score = floor( v * 100.0);
		c.name = ""; 
		popvec.push_back( c);
	}
	for ( auto v : score_qano_f){
		MyCase c;
		c.id = i;
		i++;
		c.gender = 2; 
		c.qaclass = 0;
		c.score = floor( v * 100.0);
		c.name = ""; 
		popvec.push_back( c);
	}
	tm.markEnd();
	cout << "Duration for Storing 1: " << tm.getInterval() << " millisecond" << endl;

	// currently testing ******************************************

	// ↓かなり時間がかかる。
	// 　→vectorをそのまま格納する書き方をしてみて時間を比べたい。

	tm.restart();

	SimpleDataset popds; 
	// 現状ではnameは使えない。
	std::vector <std::string> popvnvec { 
		"id", "gender", "qaclass", "score"
		/* , "name" */ 
	};
	popds.setVarNames( popvnvec);

	i = 0;
	for ( auto v : score_qayes_m){
		popds.addCase( "id", i); 
		popds.addCase( "gender", 1);
		popds.addCase( "qaclass", 1);
		popds.addCase( "score", floor( v * 100.0));
		popds.assertRecutangular();
		i++;
	}
	for ( auto v : score_qayes_f){
		popds.addCase( "id", i); 
		popds.addCase( "gender", 2);
		popds.addCase( "qaclass", 1);
		popds.addCase( "score", floor( v * 100.0));
		popds.assertRecutangular();
		i++;
	}
	for ( auto v : score_qano_m){
		popds.addCase( "id", i); 
		popds.addCase( "gender", 1);
		popds.addCase( "qaclass", 0);
		popds.addCase( "score", floor( v * 100.0));
		popds.assertRecutangular();
		i++;
	}
	for ( auto v : score_qano_f){
		popds.addCase( "id", i); 
		popds.addCase( "gender", 2);
		popds.addCase( "qaclass", 0);
		popds.addCase( "score", floor( v * 100.0));
		popds.assertRecutangular();
		i++;
	}
	tm.markEnd();
	cout << "Duration for Storing 2: " << tm.getInterval() << " millisecond" << endl;

	// currently testing ******************************************




	
	// 母集団統計量
	vector <double> all_vec;
	vector <double> all_sq_vec;
	vector <double> male_vec;
	vector <double> female_vec;
	vector <double> qayes_vec;
	vector <double> qano_vec;
	for ( auto c : popvec){
		all_vec.push_back( c.score);
		all_sq_vec.push_back( c.score * c.score);
		if ( c.gender == 1){
			male_vec.push_back( c.score);
		} else {
			female_vec.push_back( c.score);
		}
		if ( c.qaclass == 1){
			qayes_vec.push_back( c.score);
		} else {
			qano_vec.push_back( c.score);
		}
	}
	
	{
		koutputfile kof( output_fn1);
		bool b = kof.open( false, false, true);
		if ( b == true){
			stringstream ss;
			ss << "mean" "\t" << mean( all_vec) << endl
			   << "mean male" "\t" << mean( male_vec) << endl
			   << "mean female" "\t" << mean( female_vec) << endl
			   << "mean qayes" "\t" << mean( qayes_vec) << endl
			   << "mean qano" "\t" << mean( qano_vec) << endl
			   << "var(all)" "\t" << unbiasedVarBoost( all_vec) << endl
			   << "var(male)" "\t" << unbiasedVarBoost( male_vec) << endl
			   << "var(female)" "\t" << unbiasedVarBoost( female_vec) << endl
			   << "var(qayes)" "\t" << unbiasedVarBoost( qayes_vec) << endl
			   << "var(qano)" "\t" << unbiasedVarBoost( qano_vec) << endl
			   << "N" "\t" << all_vec.size() << endl
			   << "N male" "\t" << male_vec.size() << endl
			   << "N female" "\t" << female_vec.size() << endl
			   << "N qayes" "\t" << qayes_vec.size() << endl
			   << "N qano" "\t" << qano_vec.size() << endl;
			kof.writeLine( ss.str());
		}
	}
	
	// currently testing ******************************************
	SimpleDataset resultds; 
	std::vector <std::string> vnvec { 
		"nobs", "iter", "n_male", "n_female", "n_qayes", "n_qano",
		"mean", "mean_male", "mean_female", "mean_qayes", "mean_qano",
		"gender_diff", "qa_diff"
	};
	resultds.setVarNames( vnvec);
	// currently testing ******************************************

	// サンプリングする
	// そのたびに統計量を算出して記録

	std::cout << "Sampling..." << std::endl;

	for ( int nobs : nobsvec){

		for ( int j = 0; j < nsim; j++){

			vector <MyCase> samvec = getSample( popvec, rne, nobs);

			vector <double> svec; 
			vector <double> svec_male;
			vector <double> svec_female;
			vector <double> svec_qayes;
			vector <double> svec_qano;

			for ( const auto &c : samvec){

				svec.push_back( c.score);

				if ( c.gender == 1){
					svec_male.push_back( c.score);
				} else {
					svec_female.push_back( c.score);
				}

				if ( c.qaclass == 1){
					svec_qayes.push_back( c.score);
				} else {
					svec_qano.push_back( c.score);
				}


			}

			// currently testing ******************************************
			double mean_male = mean( svec_male);
			double mean_female = mean( svec_female);
			double mean_qayes = mean( svec_qayes);
			double mean_qano = mean( svec_qano);
			resultds.addCase( "nobs", nobs);
			resultds.addCase( "iter", j);
			resultds.addCase( "n_male", svec_male.size());
			resultds.addCase( "n_female", svec_female.size());
			resultds.addCase( "n_qayes", svec_qayes.size());
			resultds.addCase( "n_qano", svec_qano.size());
			resultds.addCase( "mean", mean( svec));
			resultds.addCase( "mean_male", mean( svec_male));
			resultds.addCase( "mean_female", mean( svec_female));
			resultds.addCase( "mean_qayes", mean( svec_qayes));
			resultds.addCase( "mean_qano", mean( svec_qano));
			resultds.addCase( "gender_diff", mean_male - mean_female);
			resultds.addCase( "qa_diff", mean_qayes - mean_qano);
			resultds.assertRecutangular();
			// currently testing ******************************************
		}

	}

	// 結果を書き込む

	koutputfile kof( output_fn2);
	bool s = kof.open( false, false, true);
	if ( s == true){
		
		// currently testing ******************************************
		resultds.writeToFile( kof);
		// currently testing ******************************************

		kof.close();

	}

	return 0;

}


/* ********** Definitions of Member Functions ********** */ 

