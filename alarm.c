#include <stdio.h>
#include <list.h>
#include <alarm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct alarm tick_queue[7];

int weekdays_active_output(struct alarm *a)
{
	int i;
	for (i = 0; i < 7; i++) {
		if (a->rflag & (1 << i)) {
			a->weekdays_active[i] = 1;
		}
	}

	return SUCCESS;
}

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
U32 alarm_delete(U8 id)
{
	struct alarm *tmp = alarm_search(id, ALL);
	if (tmp == NULL)
		return SUCCESS;

	tick_queue_delete(tmp);

	return FAIL;
}

struct alarm *alarm_read(U8 id)
{
	return alarm_search(id, ALL);
}

/*XXX: no test*/
U32 alarm_set_repeat(U8 id, U8 week, U8 repeat)
{
	struct alarm *tmp = alarm_search(id, ALL);
	if (tmp == NULL)
		return FAIL;

	if (repeat) {
		tmp->rflag |= (1 << week);
		tmp->enable |= (1 << week);
		tmp->week = week;
		tick_queue_insert(tmp);
	} else {
		tmp->rflag &= (~(1 << week));
		/*search the list, if alarm is no in the list, do nothing,else
		  delete the alarm
		  */
	}

	return 0;
}

U32 alarm_disable(U8 id)
{
	struct alarm *tmp = alarm_search(id, ALL);
	if (tmp == NULL)
		return FAIL;
	tmp->enable = 0;
	return SUCCESS;
}

U32 alarm_reset(U8 id)
{

	struct alarm *tmp = alarm_search(id, ALL);
	if (tmp == NULL)
		return FAIL;
	tmp->rflag = 0;
	tmp->wflag = 0;
	tmp->hour = 0;
	tmp->minute = 0;
	tmp->second = 0;
	tmp->enable = 0;
	tmp->week = 0;
	
	return SUCCESS;
}

/*traverse the alarm list to search alarm id*/
struct alarm *alarm_search(char id, char week)
{
	int i;
	struct alarm *tick_tmp;
	for (i = 0; i < 7; i++) {
		LIST *tmp = &tick_queue[i].list;
		while (!is_list_last(tmp)) {
			tick_tmp = list_entry(tmp->next, struct alarm, list);
			tmp = tmp->next;

			if (id == tick_tmp->id) {
				if (week == ALL)
					return tick_tmp;

				if (!(tick_tmp->rflag & (1 << week))) {
					tick_tmp->week = week;
					tick_tmp->enable &= (~(1 << tick_tmp->week));
				} else {
					return tick_tmp;
				}
			}
		}
	}

	return NULL;
}

U32 alarm_add(U8 id, U8 week, U8 hour, U8 minute, U8 second, bool repeat)
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
				printf("repeat id: %d", tick_tmp->id);
				printf("============>alarm exist<==============\n");
				return EXIST;
			}
		}
	}

	/*if alarm id not exist in the queue, then add it*/
	struct alarm *node = (struct alarm *)malloc(sizeof(struct alarm));
	memset(node, 0, sizeof(struct alarm));
	node->id = id;
	/*bitmap for which day*/
	node->wflag |= (1 << week);
	node->week = week;
	node->hour = hour;
	node->minute = minute;
	node->second = second;

	node->enable |= (1<<week);
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
	struct tm *now = system_time_get();
#if 0
	printf("week: %d, hour: %d, minute: %d, second: %d\n",
		now->tm_mday, now->tm_hour, now->tm_min,
		now->tm_sec);
#endif
	struct alarm *tmp = _get_new_alarm(now->tm_wday, 7, now);

	if (tmp == NULL) {

		tmp = _get_new_alarm(0, now->tm_wday, now);
	}

	if (tmp != NULL) {
		now->tm_sec = tmp->second;
		now->tm_min = tmp->minute;
		now->tm_hour = tmp->hour;

		if (now->tm_wday > tmp->week) {
			now->tm_mday += tmp->week + 6 - now->tm_wday;
		} else {
			now->tm_mday += tmp->week - now->tm_wday;
		}
		
		now->tm_wday = tmp->week;
#if 0
		printf("==>sec: %d, min: %d, hour: %d, mday: %d, mon: %d, year: %d, wday: %d, yday: %d, isdst: %d\n",
			now->tm_sec,
			now->tm_min,
			now->tm_hour,
			now->tm_mday,
			now->tm_mon,
			now->tm_year,
			now->tm_wday,
			now->tm_yday,
			now->tm_isdst);
#endif
		time_t result;
		result = mktime(now);
		if (result != -1) {
			tmp->timestamp = result;
		}

	}

	return tmp;
}

int compare(char c, struct fit in[7], OUT char fout[7], OUT char *num)
{
	int i;
	int j = 0;
	char copy[7] = {100, 100, 100, 100, 100, 100, 100};
	*num = 0;

	for (i = 0; i < 7; i++) {
		if ((in[i].num - c) >= 0) {
			copy[i] = in[i].num - c;
			j++;
		} else {
			/*no alarm in this queue*/
		}
	}
	if (!j)
		return FAIL;

	char mout[7] = {-1, -1, -1, -1, -1, -1, -1};
	i = min(copy, mout, num);
	if (i == MANY_CASES) {
		for (i = 0; i < *num; i++) {
			fout[i] = in[(int)mout[i]].id;
		}
		return MANY_CASES;
	}

	return in[i].id;
}

int min(IN char in[7], OUT char out[7], char *num)
{
	char m = in[0];
	char x;
	int i;
	int j = 0;
	out[0] = in[0];
	for (i = 1; i < 7; i++) {
		if (in[i] <= m) {
				out[i] = in[i];
				m = in[i];
				x = i;
		}
	}

	for (i = 0; i < 7; i++) {
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

struct tm *system_time_get()
{
	time_t now;
	time(&now);
	struct tm *timenow = localtime(&now);
	printf("Local   time   is   %s", asctime(timenow));
	localtime_r(&now, timenow);
#if 0
	printf("year: %d, mon: %d, mday: %d, week: %d, hour: %d, min: %d, sec: %d\n",
		timenow->tm_year, timenow->tm_mon, timenow->tm_mday, timenow->tm_wday,
		timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
#endif 
	printf("second: %ld\n", now);

	return timenow;
}

struct alarm *_get_new_alarm(int start, int end, struct tm *now)
{
	struct fit figure[7] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
	int i;
	struct alarm *tick_tmp;
	for (i = start; i < end; i++) {
		int j = 0;
		LIST *tmp = &tick_queue[i].list;
		while (!is_list_last(tmp)) {
			tick_tmp = list_entry(tmp->next, struct alarm, list);
			tmp = tmp->next;

			if (tick_tmp->enable & (1 << i)) {
				figure[j].id = tick_tmp->id;
				if (i == now->tm_wday) {
					if ((now->tm_hour == tick_tmp->hour) && (now->tm_min >= tick_tmp->minute))
						continue;

					figure[j].num = tick_tmp->hour;
				} else {
					figure[j].num = tick_tmp->hour + 24;
				}
				j++;
			}
		}
		char num;
		char out[7] = {-1, -1, -1, -1, -1, -1, -1};
		int id = compare(now->tm_hour, figure, out, &num);
		if (id == MANY_CASES) {
			struct fit _figure[7] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
			struct alarm *tmp[num];
			int k;
			for (k = 0; k < num; k++) {
				tmp[k] = alarm_search(out[k], i);
				_figure[k].id = out[k];
				if ((i != now->tm_wday) || (now->tm_hour != tmp[k]->hour))
					_figure[k].num = tmp[k]->minute + 60;
				else 
					_figure[k].num = tmp[k]->minute;
			}
			id = compare(now->tm_min, _figure, out, &num);
			return alarm_search(id, i);

		} else if (id == FAIL) {

		} else {
			printf("===============>id: %d <================\n", id);
			return alarm_search(id, i);
		}
	}

	return NULL;
}

void print(U8 week_queue)
{
	struct alarm *tick_tmp;
	LIST *tmp = &tick_queue[week_queue].list;
	printf("week %d queue node id: ", week_queue);
	while (!is_list_last(tmp)) {
		tick_tmp = list_entry(tmp->next, struct alarm, list);
		tmp = tmp->next;
		printf("%d...\t", tick_tmp->id);
	}
	printf("\n");
}

int main()
{
	int ret;
	tick_queue_init();
	ret = alarm_add(0, 5, 3, 6, 1, 1);
	if (ret == EXIST)
		printf("alarm EXIST\n");

	alarm_add(1, 2, 10, 9, 0, 1);
	alarm_add(2, 1, 12, 8, 10, 1);
	alarm_add(3, 1, 11, 1, 20, 1);
	alarm_add(4, 1, 11, 2, 23, 1);
	alarm_add(5, 2, 0, 0, 0, 1);
	alarm_add(6, 0, 20, 3, 23, 1);

	struct alarm *tick_tmp = get_new_alarm();
	if (tick_tmp == NULL) {
		printf("no alarm get\n");
		return 0;
	}

	printf("search result ====> id: %d, week: %d, hour: %d, minute: %d, second: %d, timestamp: %ld\n",
		tick_tmp->id, tick_tmp->week, tick_tmp->hour, tick_tmp->minute,
		tick_tmp->second, tick_tmp->timestamp);

	return 0;
}
