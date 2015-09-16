enum {
	SUCCESS = 0,
	FAIL,
	EXIST
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
};

/*user api*/
U32 addtimer(U8 id, U8 week, U8 hour, U8 minute, U8 second, bool repeat);
U32 rmtimer(U8 id);
U32 set_repeat(U8 id, U8 week, U8 repeat);
struct alarm readtimer(U8 id);
struct alarm system_timer_get(void);

/*return latest alarm to user*/
struct alarm get_new_alarm(void);
