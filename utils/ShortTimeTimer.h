#ifndef SHORT_TIME_TIMER_H_
#define SHORT_TIME_TIMER_H_

// TODO: import arduino for millis() ?

/*
	Count how much time has passed from last call to `start()`.
	Notice: millis() resets the counter after ~50 days. This
	class handles it by canceling `start()` after some time.
*/

class ShortTimeTimer {
	// TODO: implement!
	void start();
	void reset();
	int millisPassed();
	int secondsPassed();
	bool isPassedSeconds(int nrSeconds);
	bool timeTook(); // returns whether `start()` has been called and timer has not been stopped since than.
	void loop(); // so it could know to automatically reset after too much time has been passed.
};

#endif /* SHORT_TIME_TIMER_H_ */
