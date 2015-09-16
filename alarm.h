enum {
	SUCCESS = 0,
	FAIL,
	EXIST
};

struct fit {
	char id;
	char num;
};

/*alarm context*/
struct alarm {
	struct LIST list;
	/*bitmap according week in one alarm*/
	U8 rflag;
	U8 id;
	/*bitmap for set which day*/
	U8 wflag;
	/*for insert which queue*/
	U8 week;
	U8 hour;
	U8 minute;
	U8 second;
	U8 run;
};

/*user api*/
U32 addtimer(U8 id, U8 week, U8 hour, U8 minute, U8 second, bool repeat);
U32 rmtimer(U8 id);
U32 set_repeat(U8 id, U8 week, U8 repeat);
struct alarm readtimer(U8 id);
struct alarm *system_timer_get(void);

/*return latest alarm to user*/
struct alarm *get_new_alarm(void);

/*system lib*/
int compare(char c, struct fit n[5]);
int min(char a[5]);
struct alarm *search_alarm(char id);
int week_bitmap[] = {0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6};
char _week[1<<6];
