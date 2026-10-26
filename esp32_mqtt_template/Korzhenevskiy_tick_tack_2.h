#ifndef KORZHENEVSKIY_TICK_TACK_2_H
#define KORZHENEVSKIY_TICK_TACK_2_H

#include <Arduino.h>

// Non-blocking timer. Every timer registers itself, so mass_start() / mass_tick() / mass_stop()
// handle all of them at once. Declare timers at global scope.
class Korzhenevskiy_tick_tack_2
{
public:

	using Callback = void (*)();
	static constexpr int MAX_TIMERS = 10;  // registry costs 4 bytes per slot on ESP32

	Korzhenevskiy_tick_tack_2(Callback callback, uint32_t interval_ms);
	~Korzhenevskiy_tick_tack_2();

	// Timers register by address, so copies are not allowed
	Korzhenevskiy_tick_tack_2(const Korzhenevskiy_tick_tack_2&) = delete;
	Korzhenevskiy_tick_tack_2& operator=(const Korzhenevskiy_tick_tack_2&) = delete;

	void start();
	void start(uint32_t interval_ms);
	void stop();
	void tick();
	void setinterval(uint32_t interval_ms);
	void restart();
	void zero_count();
	bool enabled() const;
	uint32_t counts() const;  // how many times the callback has fired

	static void mass_start();
	static void mass_tick();
	static void mass_stop();
	static bool registry_overflow();  // true if more than MAX_TIMERS timers were created

private:

	static Korzhenevskiy_tick_tack_2* registry[MAX_TIMERS];
	static bool _overflow;

	Callback _callback;
	uint32_t _interval;
	uint32_t _lastTime;
	uint32_t _counts;
	bool _enabled;
	bool _registered;
};

#endif // KORZHENEVSKIY_TICK_TACK_2_H
