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
		printf("nmessage =  %d\n", nmessage);

	}

	cv.notify_one();
	if (a == 2)
		done = true;
}

void consume() {


	std::unique_lock<std::mutex> lock(mu);
	cv.wait(lock, [] {return done; });


	printf("consume =  %d\n", nmessage);
	lock.unlock();





}




int main() {


	vectorthread.reserve(3);
	for (int i = 0; i < 3; i++)
		vectorthread.emplace_back([]() {consume(); });


	for (int i = 0; i < 3; i++)
	{
		work(i);
		_sleep(3009);
	}


	for (int i = 0; i < 3; i++)
	{
		vectorthread[i].join();
	}
	printf("end\n");
	return 0;
}