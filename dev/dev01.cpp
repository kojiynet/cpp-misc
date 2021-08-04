
/*

	dev01.cpp

	開発していく用のフォルダ dev
	このファイルでは、aichiの
	　21a-qa-14.cpp　～　21a-qa-26.cpp
	について、もう少し簡単に書けるように、
	使いまわせそうな一般的なモジュールをつくってみる。
	そしてkoliに加える。

	新しい方のファイルから改善していく。
	まず21a-qa-26.cppから。
	・getBetaRandomVector()をkoliに。
	　→getBetaRandomVector2()の中につくっていっている。
	　　RandomNumberEngine :: getDistRandomVec() をつくった。
	　　→確率分布はBoostからきているので、krandboostに一応入れる。
	　　　krandboostの意味を変える。乱数発生器としてBoostのものを使うわけではない、という。

	・簡易Datasetをつくりたい。
	　SimpleDataset？
	　まず、Resultを入れるような用途。
	　変数名と対応させつつVectorを持っている、というような。そしてVectorとしてintかdoubleかstringのどれもあり、というような。
	・簡易Datasetをつくりたい、その2。
	　乱数でできたデータセットそのものの格納を。
	

*/


/* ********** Preprocessor Directives ********** */

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <boost/math/distributions/beta.hpp>
#include <k09/krand01.cpp>
#include <k09/kdataset03.cpp>
#include <k09/kstat02.cpp>
#include <k09/kstatboost01.cpp>
#include <k09/koutputfile00.cpp>
#include <k09/kalgo02.cpp>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

class MyCase;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);
int aichi26( void);

void getBetaRandomVector( std::vector <double> &, RandomNumberEngine &, int, double, double);
void getBetaRandomVector2( std::vector <double> &, RandomNumberEngine &, int, double, double);
void getDistRandomVec( std::vector <double> &, RandomNumberEngine &, int, std::function <double( double)>);



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

struct ResultCase {
	int nobs;
	int iter;
	int n_male, n_female, n_qayes, n_qano;
	double mean, mean_male, mean_female, mean_qayes, mean_qano;
	double gender_diff, qa_diff;
};


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
	std::vector <int> nobsvec = { 3}; // define later

	nobsvec.resize( 998);
	for ( int i = 0; i < 998; i++){
		nobsvec[ i] = i + 3;
	}

	// Generating Beta Distribution 

	RandomNumberEngine rne( 123); 

	double mu;
	double sigma;

	vector <double> score_qayes_m;
	vector <double> score_qayes_f;
	mu = 70.0 / 100.0;
	sigma = 10.0 / 100.0;
	getBetaRandomVector( score_qayes_m, rne, 250'000, mu, sigma * sigma);
	getBetaRandomVector( score_qayes_f, rne, 250'000, mu, sigma * sigma);

	vector <double> score_qano_m;
	vector <double> score_qano_f;
	mu = 60.0 / 100.0;
	sigma = 10.0 / 100.0;
	getBetaRandomVector( score_qano_m, rne, 250'000, mu, sigma * sigma);
	getBetaRandomVector( score_qano_f, rne, 250'000, mu, sigma * sigma);
	
	// データをケースのvectorにつめる
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
	
	vector <ResultCase> resultvec;

	// サンプリングする
	// そのたびに統計量を算出して記録

	for ( int nobs : nobsvec){

		cout << "N Obs: " << nobs << endl;

		cout << "Iteration:";

		for ( int j = 0; j < nsim; j++){

			if ( j % 100 == 1){
				cout << " " << j;
			}

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
			
			ResultCase rc;

			rc.nobs = nobs;
			rc.iter = j;
			rc.n_male = svec_male.size();
			rc.n_female = svec_female.size();
			rc.n_qayes = svec_qayes.size();
			rc.n_qano = svec_qano.size();
			rc.mean = mean( svec);
			rc.mean_male = mean( svec_male);
			rc.mean_female = mean( svec_female);
			rc.mean_qayes = mean( svec_qayes);
			rc.mean_qano = mean( svec_qano);
			rc.gender_diff = rc.mean_male - rc.mean_female;
			rc.qa_diff = rc.mean_qayes - rc.mean_qano;

			resultvec.push_back( rc);

		}

		cout << endl;

	}

	// 結果を書き込む

	koutputfile kof( output_fn2);
	bool s = kof.open( false, false, true);
	if ( s == true){

		string t;
		t = "nobs" "\t" "iter" "\t" "n_male" "\t" "n_female" "\t" "n_qayes" "\t" "n_qano" "\t" "mean" "\t" "mean_male" "\t" "mean_female" "\t" "mean_qayes" "\t" 
		    "mean_qano" "\t" "gender_diff" "\t" "qa_diff";
		kof.writeLine( t);
		
		for ( auto c : resultvec){
			stringstream ss;
			ss << c.nobs << "\t" << c.iter << "\t";
			ss << c.n_male << "\t";
			ss << c.n_female << "\t";
			ss << c.n_qayes << "\t";
			ss << c.n_qano << "\t";
			if ( isnan( c.mean)){
				ss << "." << "\t";
			} else {
				ss << c.mean << "\t";
			}
			if ( isnan( c.mean_male)){
				ss << "." << "\t";
			} else {
				ss << c.mean_male << "\t";
			}
			if ( isnan( c.mean_female)){
				ss << "." << "\t";
			} else {
				ss << c.mean_female << "\t";
			}
			if ( isnan( c.mean_qayes)){
				ss << "." << "\t";
			} else {
				ss << c.mean_qayes << "\t";
			}
			if ( isnan( c.mean_qano)){
				ss << "." << "\t";
			} else {
				ss << c.mean_qano << "\t";
			}
			if ( isnan( c.gender_diff)){
				ss << "." << "\t";
			} else {
				ss << c.gender_diff << "\t";
			}
			if ( isnan( c.qa_diff)){
				ss << ".";
			} else {
				ss << c.qa_diff;
			}
			kof.writeLine( ss.str());
		}

		kof.close();

	}

	return 0;

}


void
getBetaRandomVector(
	std::vector <double> &ret,
	RandomNumberEngine &rne,
	int len,
	double mean,
	double sigmasq
)
{

	using boost::math::beta_distribution;
	
	ret.clear();
	
	double alpha = beta_distribution<>::find_alpha( mean, sigmasq);
	double beta = beta_distribution<>::find_beta( mean, sigmasq);
	beta_distribution<> mybeta( alpha, beta);
	
	for ( int i = 0; i < len; i++){
		double rnd = rne.getRealUniform();
		ret.push_back( quantile( mybeta, rnd));
	}

}

void
getBetaRandomVector2(
	std::vector <double> &ret,
	RandomNumberEngine &rne,
	int len,
	double mean,
	double sigmasq
)
{

	using boost::math::beta_distribution;
	
	ret.clear();
	
	double alpha = beta_distribution<>::find_alpha( mean, sigmasq);
	double beta = beta_distribution<>::find_beta( mean, sigmasq);
	beta_distribution<> mybeta( alpha, beta);
	
	ret = rne.getDistRandomVec( len, [=]( const double& p){ return quantile( mybeta, p); } );

}


/* ********** Definitions of Member Functions ********** */ 
