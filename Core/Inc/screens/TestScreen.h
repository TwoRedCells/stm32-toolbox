/**
 * \file       screens/TestScreen.h
 * \class      TestScreen
 * \brief      Shows progess of a functional test, on an LCD screen.
 *
 * \author     Yvan Rodrigues <yrodrigues@clearpath.ai>
 * \copyright  Copyright (c) 2021, Clearpath Inc., All rights reserved.
 */

#ifndef TESTSCREEN_H
#define TESTSCREEN_H

#include "diagnostics/fault.h"
#include "screens/Screen.h"
#include "constants.h"

/**
 * A screen with general information such as the version and uptime.
 */
class TestScreen : public Screen
{
public:
    /**
     * Shows this screen on the LCD.
     */
    void show(void)
    {
        lcd->clear();
        Screen::show();
        lcd->set_cursor(true, true);
        graph(progress);
        osMessageQueuePut(work_q, &MSG_TEST_START, 0, 0);
    }

    /**
     * Performs any work for this screen, including updating the display if applicable.
     */
    void loop(void)
    {
        if (refresh_timer.is_elapsed())
        {
            if (is_visible())
            {
                osStatus status = osMessageQueueGet(hmi_q, &msg, NULL, 0U);
                if (status == osOK)
                    on_message(msg);

                // Show battery info for these states.
                if (msg == MSG_TEST_WAITING_FOR_CHARGER || msg == MSG_TEST_PROGRESS || msg == MSG_TEST_CHARGING)
                {
                    lcd->printat(action_row, 0, "%.1fV/%iA %.1fV/%iA ",
                            batteries[0].module_voltage, batteries[0].current,
                            batteries[1].module_voltage, batteries[1].current);
                    graph(progress);
                }

            }
            refresh_timer.restart();
        }
        Screen::loop();
    }

    void on_message(message msg)
    {
        if (msg == MSG_TEST_START)
        {
            clear();
            lcd->printat(0, 0, STR_STARTING_CHARGE_TEST);
        }

        else if (msg == MSG_TEST_ACTIVE_FAULT)
        {
            clear();
            uint32_t faults = fault.get();
            lcd->printat(info_row1, 0, STR_THERE_IS_A_FAULT);
            lcd->printat(info_row2, 0, STR_FAULT_EQUALS, faults << 16, faults & 0xffff);
            lcd->printat(action_row, 0, STR_UNABLE_TO_CONTINUE);
        }

        else if (msg == MSG_TEST_BATTERY_FULL)
        {
            clear();
            lcd->printat(info_row1, 0, STR_BATTERY_SOC_TOO_HIGH);
            uint16_t b1 = batteries[0].relative_state_of_charge;
            uint16_t b2 = batteries[1].relative_state_of_charge;
            uint16_t soc = b1 > b2 ? b1 : b2;
            lcd->printat(info_row2, 0, STR_TO_RUN_TEST_SOC, soc);
            lcd->printat(action_row, 0, STR_ALLOW_TO_DISCHARGE);
        }

        else if (msg == MSG_TEST_BATTERY_EMPTY)
        {
            clear();
            lcd->printat(info_row1, 0, STR_BATTERY_IS_EMPTY_OR);
            lcd->printat(info_row2, 0, STR_NOT_COMMUNICATING);
            lcd->printat(action_row, 0, STR_UNABLE_TO_CONTINUE);
        }

        else if (msg == MSG_TEST_CONTACTOR_ENERGIZED)
        {
            clear();
            lcd->printat(info_row1, 0, STR_ENERGIZING_CONTACTOR);
            lcd->printat(info_row2, 0, STR_AND_CHECKING_THE);
            lcd->printat(action_row, 0, STR_RELAY_CONTACTS);
            graph(1);
        }

        else if (msg == MSG_TEST_CONTACTOR_FEEDBACK)
        {
            clear();
            lcd->printat(info_row1, 0, STR_CONTACTOR_CONTACTS);
            lcd->printat(info_row2, 0, STR_FAILED_TO_RESPOND);
            lcd->printat(action_row, 0, STR_UNABLE_TO_CONTINUE);
        }

        else if (msg == MSG_TEST_PILOT_ENERGIZED)
        {
            clear();
            lcd->printat(info_row1, 0, STR_ENERGIZING_PILOT);
            lcd->printat(info_row2, 0, STR_RELAY_AND_CHECKING);
            lcd->printat(action_row, 0, STR_THE_CONTACTS);
            graph(2);
        }

        else if (msg == MSG_TEST_PILOT_FEEDBACK)
        {
            clear();
            lcd->printat(info_row1, 0, STR_PILOT_CONTACTS);
            lcd->printat(info_row2, 0, STR_FAILED_TO_RESPOND);
            lcd->printat(action_row, 0, STR_UNABLE_TO_CONTINUE);
        }

        else if (msg == MSG_TEST_WAITING_FOR_CHARGER)
        {
            clear();
            lcd->printat(info_row1, 0, STR_WAITING_FOR_CHARGER);
            lcd->printat(info_row2, 0, STR_TO_RAMP_UP_CURRENT);
            graph(3);
        }

        else if (msg == MSG_TEST_INSUFFICIENT_CURRENT)
        {
            clear();
            lcd->printat(info_row1, 0, STR_CHARGE_CURRENT_DID);
            lcd->printat(info_row2, 0, STR_NOT_REACH_EXPECTED);
            lcd->printat(action_row, 0, STR_LEVEL_TEST_FAILED);
        }

        else if (msg == MSG_TEST_PROGRESS)
        {
            graph(++progress);
        }

        else if (msg == MSG_TEST_CONTACTOR_DEENERGIZED)
        {
            clear();
            lcd->printat(info_row1, 0, STR_RELEASING_CONTACTOR);
            lcd->printat(info_row2, 0, STR_AND_CHECKING_THE);
            lcd->printat(action_row, 0, STR_RELAY_CONTACTS);
            graph(18);
        }

        else if (msg == MSG_TEST_PILOT_DEENERGIZED)
        {
            clear();
            lcd->printat(info_row1, 0, STR_RELEASING_PILOT);
            lcd->printat(info_row2, 0, STR_RELAY_AND_CHECKING);
            lcd->printat(action_row, 0, STR_RELAY_CONTACTS);
            graph(19);
        }

        else if (msg == MSG_TEST_CHARGING)
        {
            clear();
            lcd->printat(info_row1, 0, STR_ALLOWING_CHARGER);
            lcd->printat(info_row2, 0, STR_TO_CHARGE_BATTERY);
            lcd->printat(action_row, 0, STR_PLEASE_WAIT);
        }

        else if (msg == MSG_TEST_SUCCESSFUL)
        {
            clear();
            lcd->printat(info_row1, 0, STR_TEST_COMPLETED);
            lcd->printat(info_row2, 0, STR_SUCCESS);
            graph(20);
        }
    }

    void clear(void)
    {
        lcd->clear_row(0);
        lcd->clear_row(1);
        lcd->clear_row(2);
    }

    void hide(void) override
    {
        // Reset faults to clean state.
        fault.reset(Fault::ChargePilotFeedback);
        fault.reset(Fault::ContactorFeedback);
    }

    void graph(uint8_t value)
    {
        uint8_t width = 20;
        lcd->move(status_row, 0);
        for (int i=0; i<width; i++)
        {
            lcd->write(i < value ? 0xff : 0xdb); //b0
        }
        lcd->move(status_row, value);
        progress = value;
    }

private:
    message msg, last_msg;
    const uint8_t status_row = 3;
    const uint8_t system_row = 0;
    const uint8_t action_row = 2;
    const uint8_t info_row1 = 0;
    const uint8_t info_row2 = 1;
    uint8_t progress = 0;
};
#endif
