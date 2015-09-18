#define OUT
#define IN

enum {
	SUCCESS = 0,
	FAIL = -1,
	MANY_CASES = 100,
	EXIST
};

/*for search fit number*/
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
struct alarm *alarm_read(U8 id);
U32 alarm_add(U8 id, U8 week, U8 hour, U8 minute, U8 second, bool repeat);
U32 alarm_delete(U8 id);
U32 alarm_set_repeat(U8 id, U8 week, U8 repeat);
U32 alarm_reset(U8 id);
U32 alarm_disable(U8 id);
U32 alarm_enable(U8);

/*return latest alarm to user*/
struct alarm *get_new_alarm(void);

/*system lib*/
struct alarm *system_time_get(void);
struct alarm *alarm_search(char id, char week);
int compare(char c, struct fit in[7], OUT char fout[7], OUT char *num);
int min(IN char in[7], OUT char out[7], char *num);
struct alarm *_get_new_alarm(int start, int end, struct alarm *now);
