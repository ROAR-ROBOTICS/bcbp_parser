#include "ros/ros.h"
#include <stdlib.h>
#include <iostream>
#include "DB.h"


using namespace std;




bool DB::instanceFlag = false;
DB* DB::singleton = nullptr;

DB* DB::getInstance() {
    if(! instanceFlag) {
        singleton = new DB();
        instanceFlag = true;
        //cout << "Created DB Singleton!\n\n";
    }

    //cout << "DB Singleton exists already!\n\n";
    return singleton;
}



DB::DB(){

	try{
		driver = get_driver_instance();
                //cout << "Got a mysql cppconn driver instance\n";
		con = driver->connect(DB_HOST, DB_USER, DB_PASS);
		//cout << "Got a connection to MySQL\n";
		con->setSchema(DB_NAME);
		ROS_INFO("Connected to db: %s\n", DB_NAME.c_str());
	}
	catch (sql::SQLException &e) {
		cerr << "# ERR: SQLException in " << __FILE__;
		cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cerr << "# ERR: " << e.what();
		cerr << " (MySQL error code: " << e.getErrorCode();
		cerr << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
        
        res = nullptr;
        stmt = nullptr;      
}


DB::~DB(){
    instanceFlag = false;
    delete res;
	delete stmt;
	delete con;
}



/* Query gate based on flightCarrier and flightNumber */
string DB::queryGate(const string flightCarrier, const string flightNumber) {

	try {				
		
		stmt = con->createStatement();
                
        const string query = STD_QUERY_STR 
                        + FLIGHT_CARRIER_FIELD_NAME + " = '" + flightCarrier + "'" 
                        + " AND "
                        + FLIGHT_NUMBER_FIELD_NAME  + " = " + flightNumber + "";
	
        ROS_INFO("Running MySQL query:\n%s\n", query.c_str());            
		res = stmt->executeQuery( query );
		
		while (res->next()) {
		  //cout << "\t... MySQL replies: ";
		  /* Access column data by alias or column name */
		  const string str = res->getString(FLIGHT_GATE_FIELD_NAME);
		  //cout << str << '\n';
		  return str;
		  /* Access column fata by numeric offset, 1 is the first column */
		  //cout << res->getString(1) << endl;
		}

	} catch (sql::SQLException &e) {
		cerr << "# ERR: SQLException in " << __FILE__;
		cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cerr << "# ERR: " << e.what();
		cerr << " (MySQL error code: " << e.getErrorCode();
		cerr << ", SQLState: " << e.getSQLState() << " )" << endl;
	}


	return "";
}


/* Query gate based on flightCode (flightCarrier + flightNumber) */ 
string DB::queryGate(const string flightCode) {

	try {				
		
		stmt = con->createStatement();
		
		const string query = STD_QUERY_STR 
                                            + FLIGHT_CODE_FIELD_NAME 
                                            + " = '" + flightCode + "'";
		
		res = stmt->executeQuery( query );
		
		while (res->next()) {
		  cout << "\t... MySQL replies: ";
		  /* Access column data by alias or column name */
		  const string str = res->getString(FLIGHT_GATE_FIELD_NAME);
		  cout << str << '\n';
		  return str;
		  /* Access column fata by numeric offset, 1 is the first column */
		  //cout << res->getString(1) << endl;
		}

	} catch (sql::SQLException &e) {
		cerr << "# ERR: SQLException in " << __FILE__;
		cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cerr << "# ERR: " << e.what();
		cerr << " (MySQL error code: " << e.getErrorCode();
		cerr << ", SQLState: " << e.getSQLState() << " )" << endl;
	}


	return "";
}
