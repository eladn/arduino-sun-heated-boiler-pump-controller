#ifndef SHORT_TIME_TIMER_H_
#define SHORT_TIME_TIMER_H_

// TODO: import arduino for millis() ?

/*
	Count how much time has passed from last call to `start()`.
	Notice: millis() resets the counter after ~50 days. This
	class handles it by canceling `start()` after some time.
*/

class ShortTimeTimer {
private:
	bool _isSet;
	bool _isOverflow;
	unsigned long _millisOnStart;

public:

	ShortTimeTimer()
		: _isSet(false), _isOverflow(false), _millisOnStart(0)
	{}

	// TODO: implement!
	
	void start() {
		// TODO: implement!
	}
	
	void reset() {
		// TODO: implement!
	}
	
	int millisPassed() {
		// TODO: implement!
		return 0;
	}
	
	int secondsPassed() {
		// TODO: implement!
		return 0;
	}
	
	bool isPassedSeconds(int nrSeconds) {
		// TODO: implement!
		return false;
	}
	
	bool isPassedMillis(unsigned long nrMillis) {
		// TODO: implement!
		return false;
	}
	
	// returns whether `start()` has been called and timer has not been stopped since than.
	inline bool isSet() {
		return this->_isSet;
	}
	
	inline bool isOverflow() {
		return this->_isOverflow;
	}
	
	// so it could know to automatically reset after too much time has been passed.
	void loop() {
		// TODO: implement!
	}
};

#endif /* SHORT_TIME_TIMER_H_ */
