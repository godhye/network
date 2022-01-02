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


	}

	cv.notify_one();
	if (a == 2)
		done = true;
}

void consume() {

	std::unique_lock<std::mutex> lock(mu);
	cv.wait(lock, [] {return done; });

	if (done)
		return;

	printf("work =  %d\n", nmessage);
	lock.unlock();


}




int main() {


	vectorthread.reserve(3);
	for (int i = 0; i < 3; i++)
		vectorthread.emplace_back(consume);


	for (int i = 0; i < 3; i++)
	{
		work(i);

	}


	for (int i = 0; i < 3; i++)
	{
		vectorthread[i].join();
	}

	return 0;
}