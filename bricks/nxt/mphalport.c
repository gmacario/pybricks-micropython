// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2023 The Pybricks Authors
// Copyright (c) 2013, 2014 Damien P. George

#include <stdint.h>

#include <at91sam7s256.h>
#include <contiki.h>

#include <nxos/interrupts.h>
#include <nxos/drivers/systick.h>
#include <nxos/drivers/bt.h>

#include <pbdrv/clock.h>
#include <pbio/error.h>
#include <pbsys/main.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mpconfig.h"
#include "py/stream.h"

void pb_event_poll_hook_leave(void) {
    // There is a possible race condition where an interrupt occurs and sets
    // the Contiki poll_requested flag after all events have been processed. So
    // we have a critical section where we disable interrupts and check see if
    // there are any last second events. If not, we can enter Idle Mode, which
    // still wakes up the CPU on interrupt even though interrupts are otherwise
    // disabled.
    uint32_t state = nx_interrupts_disable();

    if (!process_nevents()) {
        // disable the processor clock which puts it in Idle Mode.
        AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_PCK;
    }

    nx_interrupts_enable(state);
}

void pb_stack_get_info(char **sstack, char **estack) {
    extern uint32_t __stack_start__;
    extern uint32_t __stack_end__;
    *sstack = (char *)&__stack_start__;
    *estack = (char *)&__stack_end__;
}

// TODO
bool interrupts_get() {
    return true;
}

// Core delay function that does an efficient sleep and may switch thread context.
// If IRQs are enabled then we must have the GIL.
void mp_hal_delay_ms(mp_uint_t Delay) {
    if (interrupts_get()) {
        // IRQs enabled, so can use systick counter to do the delay
        uint32_t start = pbdrv_clock_get_ms();
        // Wraparound of tick is taken care of by 2's complement arithmetic.
        do {
            // This macro will execute the necessary idle behaviour.  It may
            // raise an exception, switch threads or enter sleep mode (waiting for
            // (at least) the SysTick interrupt).
            MICROPY_EVENT_POLL_HOOK
        } while (pbdrv_clock_get_ms() - start < Delay);
    } else {
        // IRQs disabled, so need to use a busy loop for the delay.
        nx_systick_wait_ms(Delay);
    }
}

// delay for given number of microseconds
void mp_hal_delay_us(uint32_t usec) {
    // TODO
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;

    if ((poll_flags & MP_STREAM_POLL_RD) && nx_bt_stream_opened() && nx_bt_stream_data_read()) {
        ret |= MP_STREAM_POLL_RD;
    }

    return ret;
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {

    uint8_t rx_char;

    // Start reading again for next char
    nx_bt_stream_read(&rx_char, sizeof(rx_char));

    // wait for data to be read
    while (nx_bt_stream_data_read() != sizeof(rx_char)) {
        MICROPY_EVENT_POLL_HOOK
    }

    return rx_char;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {

    // Nothing to do if disconnected or empty data
    if (!nx_bt_stream_opened() || len == 0) {
        return;
    }

    nx_bt_stream_write((uint8_t *)str, len);
    while (!nx_bt_stream_data_written()) {
        MICROPY_EVENT_POLL_HOOK;
    }
}

void mp_hal_stdout_tx_flush(void) {
    // currently not buffered
}
