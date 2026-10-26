#include "Korzhenevskiy_tick_tack_2.h"


Korzhenevskiy_tick_tack_2* Korzhenevskiy_tick_tack_2::registry[Korzhenevskiy_tick_tack_2::MAX_TIMERS]{};
bool Korzhenevskiy_tick_tack_2::_overflow = false;

Korzhenevskiy_tick_tack_2::Korzhenevskiy_tick_tack_2(Callback callback, uint32_t interval_ms)
	: _callback(callback), _interval(interval_ms), _lastTime(0), _counts(0), _enabled(false), _registered(false)
{
	for (int i = 0; i < MAX_TIMERS; i++)
	{
		if (registry[i] == nullptr)
		{
			registry[i] = this;
			_registered = true;
			break;
		}
	}
	// Global constructors run before Serial.begin(), so the overflow is only flagged here
	// and reported by mass_start()
	if (!_registered) _overflow = true;
}

Korzhenevskiy_tick_tack_2::~Korzhenevskiy_tick_tack_2() {
	for (int i = 0; i < MAX_TIMERS; i++)
	{
		if (registry[i] == this) registry[i] = nullptr;
	}
}

void Korzhenevskiy_tick_tack_2::start() {
	_lastTime = millis();
	_enabled = true;
}

void Korzhenevskiy_tick_tack_2::start(uint32_t interval_ms) {
	_interval = interval_ms;
	start();
}


void Korzhenevskiy_tick_tack_2::stop() {
	_enabled = false;
}


void Korzhenevskiy_tick_tack_2::tick() {
	if (_enabled && (millis() - _lastTime) >= _interval)
	{
		_lastTime = millis();
		_counts++;
		if (_callback) _callback();
	}
}


void Korzhenevskiy_tick_tack_2::mass_start()
{
	if (_overflow)
	{
		Serial.print("WARNING: more than ");
		Serial.print(MAX_TIMERS);
		Serial.println(" timers created, the extra ones are not in mass_start/mass_tick");
	}
	for (int i = 0; i < MAX_TIMERS; i++)
	{
		if (registry[i] != nullptr) registry[i]->start();
	}
}


void Korzhenevskiy_tick_tack_2::mass_tick()
{
	for (int i = 0; i < MAX_TIMERS; i++)
	{
		if (registry[i] != nullptr) registry[i]->tick();
	}
}


void Korzhenevskiy_tick_tack_2::mass_stop()
{
	for (int i = 0; i < MAX_TIMERS; i++)
	{
		if (registry[i] != nullptr) registry[i]->stop();
	}
}


void Korzhenevskiy_tick_tack_2::setinterval(uint32_t interval_ms) {
	_interval = interval_ms;
}


void Korzhenevskiy_tick_tack_2::restart() {
	_lastTime = millis();
}


void Korzhenevskiy_tick_tack_2::zero_count() {
	_counts = 0;
}

bool Korzhenevskiy_tick_tack_2::enabled() const {
	return _enabled;
}

uint32_t Korzhenevskiy_tick_tack_2::counts() const {
	return _counts;
}

bool Korzhenevskiy_tick_tack_2::registry_overflow() {
	return _overflow;
}
