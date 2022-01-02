#include <future>
#include <iostream>
#include <string>
#include <thread>

using std::string;

int plus(int a, int b, std::promise<int> &p)
{
	p.set_value(a + b);
	return a + b;
}

int main() {

	//int값을 반환할 예정인 promise(생산자역할)
	std::promise<int> p;
	//int값 반환(소비자역할)
	std::future<int> f;

	//미래에 int값 데이터 돌려주겟다.
	f = p.get_future();

	std::thread t(plus, 3, 4, std::move(p));


	printf("plus %d\n", f.get());
	t.join();

	return 0;
}