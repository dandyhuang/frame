#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <string>
#include <iostream>
#include "vivolog.h"

#define LOG_FILENAME "./logs/biz/vivo_biz_rerank.log"

static int g_running = 1;
static int g_num = 100;
static int g_thread = 2;
static const int MAX_THREAD = 128;
static long g_ticks[MAX_THREAD] = {0};

bool SignalAction(int signo, sighandler_t func)
{
    struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(signo, &act, &oact) < 0)
    {
		LOG_CONSOLE("sigaction error: %s",strerror(errno));
		return false;
	}
	return true;
}

void SignalIgnore(int signo)
{
    LOG_INFO("ignore signal %d", signo);
}

void SignalQuit(int signo)
{
    LOG_INFO("quit signal %d", signo);
	g_running = 0;
}

static void * test_libvivolog_multi_thread(void *param)
{
	static int g_count;
	int index = g_count++;

	//sleep(1);

	LOG_CONSOLE("start index %d",index);

	while (g_running) {
		if (g_ticks[index] < g_num) {
			LOG_DEBUG("thread_%d %ld",index,g_ticks[index]);
			LOG_INFO("thread_%d %ld",index,g_ticks[index]);
			LOG_WARN("thread_%d %ld",index,g_ticks[index]);
			LOG_ERROR("thread_%d %ld",index,g_ticks[index]);

			std::string str{"test str %c%d%s%s%%"};
			LOG_CONSOLE("%s",str.c_str());
			LOG_DEBUG("%s",str.c_str());
			LOG_INFO("%s",str.c_str());
			LOG_WARN("%s",str.c_str());
			LOG_ERROR("%s",str.c_str());
			g_ticks[index]++;
		} else {
			usleep(1000 * 10);
		}
	}

	LOG_CONSOLE("exit index %d",index);

	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		g_num = atoi(argv[1]);
		if (argc > 2) {
			g_thread = atoi(argv[2]);
			g_thread = g_thread > MAX_THREAD ? MAX_THREAD : g_thread;
		}
	}

	vivolog_init(LOG_FILENAME);

	g_running = 1;

	pthread_t tids[MAX_THREAD] = { 0 };

	for (int i = 0; i < g_thread; i++) {
		int ret = pthread_create(&tids[i], NULL, test_libvivolog_multi_thread, NULL);
		if (ret) {
			LOG_CONSOLE("%s", "pthread_create fail");
			exit(0);
			return 0;
		}
	}

	SignalAction(SIGUSR2, SignalIgnore);
	SignalAction(SIGUSR1, SignalIgnore);
	SignalAction(SIGPIPE, SignalIgnore);
	SignalAction(SIGTERM, SignalQuit);
	SignalAction(SIGQUIT, SignalQuit);
	SignalAction(SIGINT, SignalQuit);

	//sleep(1);

	while (g_running) {
		bool done = true;
		for (int index = 0; index < g_thread; index++) {
			if (g_ticks[index] < g_num) {
				done = false;
				break;
			}
		}

		if (done) {
			break;
		}

		usleep(1000 * 10);
	}

	g_running = 0;

	for (int i = 0; i < g_thread; i++) {
		//pthread_cancel(tids[i]);
		pthread_join(tids[i], NULL);
	}

	sleep(1);

	vivolog_stop();

	LOG_CONSOLE("main exit");

	return 0;
}

