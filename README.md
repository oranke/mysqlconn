# Simple mysql connection pool

복잡한것이 싫을 때 쓰기 좋은 mysql 연결 풀. 



테스트 빌드. (우분투 16.04-x86_64) 
```bash
$ g++ -Wall -std=c++11 -I/usr/include/mysql -I. test.cpp mysqlconn.cpp -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lz
```

`test.cpp` 내의 `HOST`, `USER`, `PASSWD`, `DATABASE`, `PORT` 를 자신의 환경에 맞출 것. 


기능
* 지정 갯수 내의 연결 유지 및 락 관리. 
* 여러줄의 쿼리 전송 및 여러개의 리턴값 처리. 
* 끝.

