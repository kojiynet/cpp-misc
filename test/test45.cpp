
/*
	test45.cpp

	rvalue reference と move の使い方のテスト
*/

/* ********** Preprocessor Directives ********** */

#include <iostream>
#include <algorithm>
#include <vector>


/* ********** Namespace Declarations/Directives ********** */

//using namespace std;


/* ********** Class Declarations ********** */

class DataType;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */

int main( int, char *[]);

double workerA( std::vector <DataType>);
double workerB( std::vector <DataType> &&);
std::vector <DataType> workerC( void);
std::vector <DataType> &&workerD( void);


/* ********** Class Definitions ********** */

class DataType {

public:

	double d;
	int i;

	DataType( void) : d( 0.0), i( 0)
	{
		std::cout << "DataType Default-Constructor Called." << std::endl;
	}

	DataType( const DataType &d0) : d( d0.d), i( d0.i)
	{
		std::cout << "DataType Copy-Constructor Called." << std::endl;
	}

	~DataType( void)
	{}
	
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

int main( int argc, char *argv[])
{

	{

		std::vector <DataType> data( 2);
		data[ 0].d = 1.0;
		data[ 1].i = 2.0;

		std::cout << std::endl;

		// argument is just copied
		std::cout << "Calling workerA( data)" << std::endl;
		double ret1 = workerA( data);
		std::cout << " Return Value: " << ret1 << std::endl;
		std::cout << std::endl;
		
		// this can work
		std::cout << "Calling workerA( std::move( data))" << std::endl;
		double ret2 = workerA( std::move( data));
		std::cout << " Return Value: " << ret2 << std::endl;
		std::cout << std::endl;

/*
		// should not do this; the object was already "moved" and its lifetime has ended 
		std::cout << "Look inside data[ 0].d: " << data[ 0].d << std::endl;
		std::cout << std::endl;
*/
	}

	{
		
		std::vector <DataType> data( 2);
		data[ 0].d = 1.0;
		data[ 1].i = 2.0;

		std::cout << std::endl;
/*
		std::cout << "Calling workerB( data)" << std::endl;
		double ret1 = workerB( data); // compilation error
		std::cout << " Return Value: " << ret1 << std::endl;
		std::cout << std::endl;
*/		
		// correct way 
		std::cout << "Calling workerB( std::move( data))" << std::endl;
		double ret2 = workerB( std::move( data));
		std::cout << " Return Value: " << ret2 << std::endl;
		std::cout << std::endl;

		// this happened to be done; but it does not seem to be correct
		std::cout << "Look inside data[ 0].d: " << data[ 0].d << std::endl;
		std::cout << std::endl;

	}

	{
		
		std::cout << std::endl;

		std::cout << "Calling ret1 = workerC()" << std::endl;
		std::vector <DataType> ret1 = workerC();
		std::cout << " Return Value: " << ret1[ 0].d << std::endl;
		std::cout << std::endl;
		
		std::cout << "Calling &&ret2 = workerC()" << std::endl;
		std::vector <DataType> &&ret2 = workerC();
		std::cout << " Return Value: " << ret2[ 0].d << std::endl;
		std::cout << std::endl;

	}

	{
		
		std::cout << std::endl;

/*
		std::cout << "Calling ret1 = workerD()" << std::endl;
		// should not do this; returned object was destroyed 
		std::vector <DataType> ret1 = workerD();
		std::cout << " Return Value: " << ret1[ 0].d << std::endl;
		std::cout << std::endl;
*/
		
/*
		std::cout << "Calling &&ret2 = workerD()" << std::endl;
		// should not do this; returned object was destroyed 
		std::vector <DataType> &&ret2 = workerD();
		std::cout << " Return Value: " << ret2[ 0].d << std::endl;
		std::cout << std::endl;
*/

	}

	return 0;
	
}


double workerA( std::vector <DataType> dvec)
{
	double ret = 0.0;
	for ( const auto &v : dvec){
		ret += v.d;
		ret += v.i;
	}
	return ret;
}


double workerB( std::vector <DataType> &&dvec)
{
	double ret = 0.0;
	for ( const auto &v : dvec){
		ret += v.d;
		ret += v.i;
	}
	return ret;
}


std::vector <DataType> workerC( void)
{
	std::vector <DataType> ret( 2);
	ret[ 0].d = 0.1;
	ret[ 0].i = 2;
	return std::move( ret);
}


std::vector <DataType> &&workerD( void)
{
	std::vector <DataType> ret( 2);
	ret[ 0].d = 0.3;
	ret[ 0].i = 4;
	return std::move( ret);
}


