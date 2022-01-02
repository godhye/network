#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

using std::string;

bool done = false;
std::condition_variable cv;
std::mutex mu;
int ncount = 0;
int nmessage;
std::vector<std::thread> vectorthread;
std::vector<std::thread> consumethread;

void work(int a)
{
	{
		std::lock_guard<std::mutex> lock(mu);
		nmessage = a;
		ncount++;
		printf("work =  %d ncount = %d \n", nmessage, ncount);

	}
	if (ncount == 3)
		done = true;

	cv.notify_one();

}

void consume() {

	while (1)
	{
		std::unique_lock<std::mutex> lock(mu);
		// done이 false이면  wait
		cv.wait(lock, [&] {return done; });

		if (ncount == 3)
			printf("consume =  %d , nCount =  %d\n", nmessage, ncount);
		lock.unlock();
		return;

		printf("consume =  %d , nCount =  %d\n", nmessage, ncount);
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::microseconds(50));
	}




}




int main() {


	vectorthread.reserve(3);
	for (int i = 0; i < 3; i++)
	{

		vectorthread.emplace_back([&]() {work(i); });
		consumethread.emplace_back([]() {consume(); });
	}

	for (int i = 0; i < 3; i++)
	{
		vectorthread[i].join();
	}
	cv.notify_all();
	printf("end\n");

	for (int i = 0; i < 3; i++)
	{

		consumethread[i].join();
	}

	return 0;
}