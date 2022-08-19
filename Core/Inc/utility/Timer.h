/**
 * \file       utility/Timer.h
 * \class      Timer
 * \brief      Microsecond timer.
 */

#ifndef TIMER_H
#define TIMER_H

#define microseconds(x) (x*1)
#define milliseconds(x) (x*1000)
#define seconds(x) (x*1000000)

#define MAX_DURATION (seconds(10))   // HACK!

/**
 * A general purpose alarm/timer class.
 */
class Timer
{
public:
    /**
     * Starts the timer.
     * @param duration The interval after which the alarm should elapse.
     */
    void start(uint32_t duration)
    {
    	this->duration = duration;
        started = now();
        alarm = started + duration;
    }


    /**
     * Sets the timer duration without starting the timer.
     * @param duration The interval after which the alarm should elapse.
     */
    void set(uint32_t  duration)
    {
    	this->duration = duration;
    }


    /**
     * Restarts the timer.
     */
    void restart(void)
    {
        start(duration);
    }


    /**
     * Resets the timer to a clean state. Does not restart the timer.
     */
    void reset(void)
    {
        started = 0;
        alarm = 0;
    }


    /**
     * Returns true if the alarm timer has elapsed
     * @return True if the alarm time has been reached.
     */
    bool is_elapsed(void)
    {
        // HACK: We check now() < started because the DWT timer overflows about once a minute.
        return alarm != 0 && (now() >= alarm || now() < started);
    }


    /**
     * Returns true if the alarm has been started.
     * @return true if the alarm is running.
     */
    bool is_started(void)
    {
        return started != 0;
    }


    /**
     * Gets the time elapsed since the timer was started.
     * @return The time elapsed.
     */
    uint32_t elapsed(void)
    {
        return now() - started;
    }


    /**
     * Gets the current timestamp.
     * @return
     */
    static uint32_t now(void)
    {
    	return DWT->CYCCNT / (HAL_RCC_GetHCLKFreq() / 1000000);
    }

private:
    uint32_t started;  /// The time when the timer was started.
    uint32_t alarm;  /// The time when the alarm should be invoked.
    uint32_t duration;  /// The duration of the timer.
    uint32_t last;
};

#endif
