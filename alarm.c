#include <stdio.h>
#include <list.h>
#include <alarm.h>
#include <stdlib.h>

struct alarm tick_queue;

void tick_queue_init(void)
{
	list_init(&tick_queue.list);
}

void tick_queue_insert(struct alarm *tick)
{
    list_insert_behind(&tick_queue.list, &tick->list);
}

void tick_queue_delete(struct alarm *tick)
{
    list_delete(&tick->list);
}


int main()
{
	tick_queue_init();
	addtimer(0, 1, 1, 1, 1, 1);
	addtimer(1, 2, 2, 1, 1, 1);
	addtimer(1, 2, 2, 1, 1, 1);
	addtimer(3, 2, 2, 1, 1, 1);
	addtimer(4, 2, 2, 1, 1, 1);

	/*traverse the alarm list to search alarm id*/
	struct alarm *tick_tmp;
	LIST *tmp = &tick_queue.list;
	while (!is_list_last(tmp)) {
		tick_tmp = list_entry(tmp->next, struct alarm, list);
		tmp = tmp->next;

		printf("id: %d, week: %d, hour: %d, minute: %d, second: %d, repeat: %d\n",
		tick_tmp->id, tick_tmp->wflag, tick_tmp->hour, tick_tmp->minute,
		tick_tmp->second, tick_tmp->rflag);
	}

	return 0;
}

U32 addtimer(U8 id,
		U8 week,
		U8 hour,
		U8 minute,
		U8 second,
		bool repeat)
{
	if (!((id >=0) && (week > 0) && (week < 7) &&
		(hour > 0) && (hour < 25) && (minute > 0) &&
		(minute < 61) && (second > 0) && (second < 61)))
	{
		for (;;)
			printf("error argument \n");
	}

	/*traverse the alarm list to search alarm id*/
	struct alarm *tick_tmp;
	LIST *tmp = &tick_queue.list;
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
	/*if alarm id not exist in the queue, then add it*/
	struct alarm *node = (struct alarm *)malloc(sizeof(struct alarm));
	node->id = id;
	/*bitmap for which day*/
	node->wflag |= (1 << week);
	node->minute = minute;
	node->second = second;

	if (repeat) {
		node->rflag |= (1 << week);
	}

	tick_queue_insert(node);
	return SUCCESS;
}

#if 0
struct alarm get_new_alarm(void)
{
	struct alarm now = system_timer_get();
	
	struct alarm *tick_tmp;
	LIST *tmp = &tick_queue.list;
	while (!is_list_last(tmp)) {
		tick_tmp = list_entry(tmp->next, struct alarm, list);
		tmp = tmp->next;

		if (tick_tmp->run) {
			int i = 0;
			while (!(tick_tmp->run && (1<<i))) {
				i++;
			}
		}
	}
}
#endif
