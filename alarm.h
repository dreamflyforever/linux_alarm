enum {
	SUCCESS = 0,
	FAIL = -1,
	MANY_CASES = 100,
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
#define OUT
#define IN
struct alarm *search_alarm(char id, char week);
int compare(char c, struct fit in[5], OUT char fout[5], OUT char *num);
int min(IN char in[5], OUT char out[5], char *num);
