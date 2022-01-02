#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

using std::string;

bool done = false;
std::condition_variable cv;
std::mutex mu;
int nmessage;
std::vector<std::thread> vectorthread;

void work(int a)
{
	{
		std::lock_guard<std::mutex> lock(mu);
		nmessage = a;

		if (a == 2)
			done = true;
	}

	cv.notify_one();

}


int main() {


	//int값을 반환할 예정인 promise(생산자역할)
	std::promise<int> p;
	//int값 반환(소비자역할)
	std::future<int> f;

	//미래에 int값 데이터 돌려주겟다.
	f = p.get_future();

	vectorthread.reserve(3);
	for (int i = 0; i < 3; i++)
		vectorthread.emplace_back(std::bind(work, i));

	while (1)
	{
		std::unique_lock<std::mutex> lock(mu);
		cv.wait(lock, [] {return done; });

		if (done)
			return;

		printf("work =  %d\n", nmessage);
		lock.unlock();
		for (int i = 0; i < 3; i++)
			vectorthread[i].join();
	}



	return 0;
}