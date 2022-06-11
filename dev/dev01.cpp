
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
	21a-qa-26.cpp　→できた。
	21a-qa-25.cpp　→26でやった内容でカバーできている。
	21a-qa-24.cpp　→26でやった内容でカバーできている。

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
#include <k09/koutputfile01.cpp>
#include <k09/kalgo02.cpp>
#include <k09/kexcept00.cpp>
#include <k09/kutil02.cpp>
#include <k09/ksimpleds00.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

class MyCase;


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
	
	// 属性ごとのvectorをつくる。この際の選別の方法を異にしてみて、それぞれにかかる時間を計っている。

	tm.restart();
	vector <double> male_vec = 
		popds.getVectorIf( // mapを使うバージョン。
			"score",
			[]( std::map <std::string, double> v){ return ( std::round( v.at( "gender")) == 1.0); }
		);
	tm.markEnd();
	cout << "Duration 1: " << tm.getInterval() << " millisecond" << endl;
	
	tm.restart();
	vector <double> female_vec =
		popds.getVectorIf( // 選別に1変数を用いる場合で、変数名を指定するバージョン。
			"score", 
			[]( std::function <double(std::string)> v){ return ( std::round( v( "gender")) == 2.0); }
		);
	tm.markEnd();
	cout << "Duration 2: " << tm.getInterval() << " millisecond" << endl;
	
	tm.restart();
	vector <double> qayes_vec =
		popds.getVectorIf( // あらかじめ、選別に使用する変数を指定しておくバージョン。
			"score", "qaclass",
			[]( double qaclass){ return ( std::round( qaclass) == 1.0); }
		);
	tm.markEnd();
	cout << "Duration 3: " << tm.getInterval() << " millisecond" << endl;
	
	tm.restart();
	vector <double> qano_vec =
		popds.getVectorIf( // あらかじめ、選別に使用する変数を指定しておくバージョン。
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
			kof.stream()
			   << "mean"		"\t" << mean( all_vec) << endl
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
			resultds.addCase( "nobs",        nobs);
			resultds.addCase( "iter",        j);
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

