/**
* 
 * 
 * $ g++ -Wall -std=c++11 -I/usr/include/mysql -I. test.cpp mysqlconn.cpp -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lz
 * 
 * 
 */

#include <iostream>

#include "mysqlconn.h"

#define HOST        "localhost"
#define USER        "user"
#define PASSWD      "password"
#define DATABASE    "mysql"
#define PORT        0

using namespace std; 

void procConn(MysqlConnection& conn) {
    conn.print(); 
    cout << "" << endl; 

    conn.print(); 
    cout << "" << endl; 

    if (!conn.is_connected()) return; 

    string query = "SELECT * FROM `db` LIMIT 3;";
    cout << "query   \t" << query << endl << endl; 

    auto data = conn.open( query );
    if (!data->is_valid()) return; 
    
    cout << "Columns: " << data->get_columnCount() << endl; 
    for (size_t i = 0; i < data->get_columnCount(); i++ ) {
        cout << "  " << data->get_columnName(i) << endl; 
    }

    auto row = data->next(); 
    unsigned int row_idx = 0; 
    while (row) {
        row_idx++; 
        cout << "==========" << endl; 
        cout << "Data " << row_idx << endl; 

        for (size_t i = 0; i < data->get_columnCount(); i++ ) {
            cout << "  " << row->get_value(i) << endl; 
        }

        row = data->next(); 
    }

    // /*
    // escapestr 테스트. 
    string test = conn.escapeString(R"('%\test')");
    cout << test << ", " << test.size() << endl; 
    // */
}

void testConnection() {
    MysqlConnection conn;

    conn.connect(HOST, USER, PASSWD, DATABASE);
    procConn(conn); 
}

void testConnection2() {
    MysqlConnection conn;
    conn.connect(HOST, USER, PASSWD, "");

    { // DB 생성
        string query = R"(
            CREATE DATABASE IF NOT EXISTS `test` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
            USE `test`;
        )";

        my_ulonglong affectRows = 0; 

        if (!conn.execute(query, true)) {
            cout << "Step1 " << conn.lastError() << endl; 
            return;
        } 
        
        cout << "Step1 affect: " << affectRows << endl; 
    }

    {   // 테이블 생성
        string query = R"(
            CREATE TABLE IF NOT EXISTS t (
                id INT AUTO_INCREMENT NOT NULL PRIMARY KEY,
                name VARCHAR(10) NOT NULL
            );
        )";

        my_ulonglong affectRows = 0; 

        if (!conn.execute(query, true)) {
            cout << "Step2 " << conn.lastError() << endl; 
            return;
        } 

        cout << "Step2 affect: " << affectRows << endl; 
    }

    {   // 정보 추가. affectRows값은 처음에는 3, 두번째 이후부터 2가 된다. 
        string query = R"(
            INSERT INTO t VALUES (NULL, 'Bob');
            INSERT IGNORE INTO t VALUES (5, 'Tomas');
            INSERT INTO t VALUES (NULL, 'Cherry');
        )";

        my_ulonglong affectRows = 0; 

        if (!conn.execute(query, true, &affectRows)) {
            cout << "Step3 " << conn.lastError() << endl; 
            cout << query << endl; 
            return;
        } 
        
        cout << "Step3 affect: " << affectRows << endl; 
    }


    {   // 한 줄 정보 추가
        // affectrows 값은 멀티라인에서는 제대로 동작하지 않는다! 해당 내용 테스트용. 
        // https://dev.mysql.com/doc/refman/8.0/en/mysql-affected-rows.html
        // 한 줄 쿼리의 경우 execute 의 인자값을 false 로 하면 affectrows를 얻을 수 있다. 
        string query = R"(
            INSERT INTO t VALUES (NULL, 'Test');
        )";

        if (!conn.execute(query, false)) {
            cout << "Step4 " << conn.lastError() << endl; 
            cout << query << endl; 
            return;
        }
        cout << "Step4 affect: " << conn.affectrows() << endl; 
    }

    // my_ulonglong 은 unsigned 이므로 오류에 의한 음수값 리턴시 (my_ulonglong)-1 과 비교 필요. 
    //cout << (my_ulonglong)-1 << endl;

    cout << "Last insert ID: " << conn.lastInsertID() << endl; 

}

void testConnPool() {
    MysqlConnPool pool(5, HOST, USER, PASSWD, DATABASE);

    //cout << pool.checkConnection() << endl; 

    auto conn = pool.lockConnection();
    if (!conn) return; 

    procConn(*conn); 

    conn->unlock(); 

}

int main() {
    cout << "Simple MysqlConnection test" << endl << endl; 

    // SELECT 테스트
    testConnection(); 

    // INSERT 테스트
    //testConnection2(); 

    // POOL 테스트
    //testConnPool();

}

