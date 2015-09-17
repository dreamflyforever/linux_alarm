#include <stdio.h>
#include <list.h>
#include <alarm.h>
#include <stdlib.h>
#include<time.h>

struct alarm tick_queue[7];

int tick_queue_init(void)
{
	U8 i;
	for (i = 0; i < 7; i++)
        	list_init(&tick_queue[i].list);

	return 0;

}

int tick_queue_insert(struct alarm *tick)
{
	list_insert_behind(&tick_queue[tick->week].list, &tick->list);

	return 0;
}

int tick_queue_delete(struct alarm *tick)
{
	list_delete(&tick->list);

	return 0;
}

/*XXX: no test*/
U32 rmtimer(U8 id)
{
	struct alarm *tmp = search_alarm(id, 0);
	if (tmp == NULL)
		return 0;

	tick_queue_delete(tmp);

	return 0;
}

struct alarm *readtimer(U8 id)
{
	return search_alarm(id, 100);
}

/*XXX: no test*/
U32 set_repeat(U8 id, U8 week, U8 repeat)
{
	struct alarm *tmp = search_alarm(id, 100);
	if (tmp == NULL)
		return FAIL;
	tmp->rflag |= (1 << week);
	tmp->run |= (1 << week);

	return 0;
}

/*traverse the alarm list to search alarm id*/
struct alarm *search_alarm(char id, char week)
{
	int i;
	struct alarm *tick_tmp;
	for (i = 0; i < 7; i++) {
		LIST *tmp = &tick_queue[i].list;
		while (!is_list_last(tmp)) {
			tick_tmp = list_entry(tmp->next, struct alarm, list);
			tmp = tmp->next;

			if (id == tick_tmp->id) {
				tick_tmp->week = week;
				if (!(tick_tmp->rflag & (1 << tick_tmp->week)))
					tick_tmp->run &=  (~(1 << tick_tmp->week));
				return tick_tmp;
			}
		}
	}

	return NULL;
}

int main()
{
	tick_queue_init();
	addtimer(0, 0, 3, 6, 1, 1);
	addtimer(1, 0, 3, 2, 0, 1);
	addtimer(2, 2, 3, 9, 10, 1);
	addtimer(3, 3, 1, 1, 20, 1);
	addtimer(4, 4, 20, 1, 23, 1);
	addtimer(5, 5, 20, 1, 23, 1);
	addtimer(6, 6, 20, 1, 23, 1);
	addtimer(7, 3, 20, 1, 23, 1);

	struct alarm *tick_tmp = get_new_alarm();
	if (tick_tmp == NULL) {
		printf("no alarm get\n");
		return 0;
	}

	printf("search result ====> id: %d, week: %d, hour: %d, minute: %d, second: %d\n",
		tick_tmp->id, tick_tmp->week, tick_tmp->hour, tick_tmp->minute,
		tick_tmp->second);

	return 0;
}

U32 addtimer(U8 id,
		U8 week,
		U8 hour,
		U8 minute,
		U8 second,
		bool repeat)
{
	if (!((id >=0) && (week >= 0) && (week < 7) &&
		(hour >= 0) && (hour < 25) && (minute >= 0) &&
		(minute < 61) && (second >= 0) && (second < 61)))
	{
		for (;;)
			printf("error argument \n");
	}

	/*traverse the alarm list to search alarm id*/
	int i;
	for (i = 0; i < 7; i++) {
		struct alarm *tick_tmp;
		LIST *tmp = &tick_queue[i].list;

		while (!is_list_last(tmp)) {
			tick_tmp = list_entry(tmp->next, struct alarm, list);
			tmp = tmp->next;

			if (tick_tmp->id == id) {
				printf("id: %d, week: %d, hour: %d, minute: %d, second: %d, repeat: %d\n",
						tick_tmp->id, tick_tmp->wflag, tick_tmp->hour, tick_tmp->minute,
						tick_tmp->second, tick_tmp->rflag);
				printf("============>alarm exist<==============\n");
				return EXIST;
			}
		}
	}

	/*if alarm id not exist in the queue, then add it*/
	struct alarm *node = (struct alarm *)malloc(sizeof(struct alarm));
	node->id = id;
	/*bitmap for which day*/
	node->wflag |= (1 << week);
	node->week = week;
	node->hour = hour;
	node->minute = minute;
	node->second = second;

	node->run |= (1<<week);
	if (repeat) {
		node->rflag |= (1 << week);
	}
	printf("id: %d, week: %d, hour: %d, minute: %d, second: %d\n",
		node->id, node->week, node->hour, node->minute,
		node->second);

	tick_queue_insert(node);
	return SUCCESS;
}

struct alarm *get_new_alarm(void)
{
	struct alarm *now = system_timer_get();
	struct alarm *tmp = _get_new_alarm(now->week, 7, now);
	if (tmp == NULL) {
		tmp = _get_new_alarm(0, now->week, now);
	}
	return tmp;
}

int compare(char c, struct fit in[5], OUT char fout[5], OUT char *num)
{
	int i;
	int j = 0;
	char copy[5] = {100, 100, 100, 100, 100};
	*num = 0;

	for (i = 0; i < 5; i++) {
		if ((in[i].num - c) >= 0) {
			copy[i] = in[i].num - c;
			j++;
		} else {
			/*no alarm in this queue*/
		}
	}
	if (!j)
		return FAIL;

	char mout[5] = {-1, -1, -1, -1, -1};
	i = min(copy, mout, num);
	if (i == MANY_CASES) {
		for (i = 0; i < *num; i++) {
			fout[i] = in[(int)mout[i]].id;
		}
		return MANY_CASES;
	}

	return in[i].id;
}

int min(IN char in[5], OUT char out[5], char *num)
{
	char m = in[0];
	char x;
	int i;
	int j = 0;
	out[0] = in[0];
	for (i = 1; i < 5; i++) {
		if (in[i] <= m) {
				out[i] = in[i];
				m = in[i];
				x = i;
		}
	}

	for (i = 0; i < 5; i++) {
		if ((m == out[i]) && (m != 100)) {
			out[j] = i;
			j++;
		}
	}

	if (j > 1) {
		*num = j;
		return MANY_CASES;
	}

	return x;
}

struct alarm *system_timer_get(void)
{
	time_t now;
	struct tm *timenow;
	time(&now);
	timenow = localtime(&now);
	printf("Local   time   is   %s", asctime(timenow));
	localtime_r(&now, timenow);
#if 0
	printf("year: %d, mon: %d, mday: %d, week: %d, hour: %d, min: %d, sec: %d\n",
		timenow->tm_year, timenow->tm_mon, timenow->tm_mday, timenow->tm_wday,
		timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
#endif 
	struct alarm *tmp = malloc(sizeof(struct alarm));
	tmp->week = timenow->tm_wday;
	tmp->hour = timenow->tm_hour;
	tmp->minute = timenow->tm_min;
	tmp->second = timenow->tm_sec;

	return tmp;
}

struct alarm *_get_new_alarm(int start, int end, struct alarm *now)
{
	struct fit figure[5] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
	int i;
	struct alarm *tick_tmp;
	for (i = start; i < end; i++) {
		int j = 0;
		LIST *tmp = &tick_queue[i].list;
		while (!is_list_last(tmp)) {
			tick_tmp = list_entry(tmp->next, struct alarm, list);
			tmp = tmp->next;

			if (tick_tmp->run & (1 << i)) {
				figure[j].id = tick_tmp->id;
				figure[j].num = tick_tmp->hour + 24;
				j++;
			}
		}
		char num;
		char out[5] = {-1, -1, -1, -1, -1};
		int id = compare(now->hour, figure, out, &num);
		if (id == MANY_CASES) {
			struct alarm *tmp[num];
			int k;
			for (k = 0; k < num; k++) {
				tmp[k] = search_alarm(out[k], i);
				figure[k].id = out[k];
				figure[k].num = tmp[k]->minute + 60;
			}
			id = compare(now->minute, figure, out, &num);
			return  search_alarm(id, i);

		} else if (id == FAIL) {

		} else {
			printf("===============>id: %d <================\n", id);
			return search_alarm(id, i);
		}
	}

	return NULL;
}
