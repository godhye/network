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

	std::thread t(std::bind(plus, 3, 4, std::move(p)));


	//f.wait() 할 필요없는 이유는 값이 아직 future에 없더라도 promise가 future에 전달할 때 까지 기다린 다음 리턴함으로
	//get을 호출하면 future내에 있는 데이터가 이동됨 재호출 금지
	printf("plus %d\n", f.get());
	t.join();

	return 0;
}