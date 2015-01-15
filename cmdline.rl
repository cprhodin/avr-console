/*
 * Copyright 2013, 2014 Chris Rhodin <chris@notav8.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "project.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>

#include "librb.h"
#include "console.h"

extern void led_write_number(uint8_t line, uint16_t number);
extern void adc_restart(void);

/*
 * X and Y Axis Offsets
 */
uint16_t x_offset = 0;
uint16_t y_offset = 0;

#define OFFSET_X(x) (x - x_offset)
#define OFFSET_Y(y) (y_offset - y)

uint16_t sample_count = 0;
uint16_t sample_delta = 0;
uint16_t sample_start;

#define ADC_IDLE  0
#define ADC_START 1
#define ADC_READY 2
#define ADC_RUN   3

uint8_t adc_state = ADC_IDLE;

/* GPIOR00: new sample ready flag */
/* GPIOR01: restart */
extern uint16_t adc_ch6;
extern uint16_t adc_ch7;

void idle_task(void)
{
    uint8_t flags;
    uint16_t x;
    uint16_t y;

    /*
     * capture current ADC state
     */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        flags = GPIOR0;

        if (test_bit(&GPIOR0, GPIOR00)) {
            clear_bit(&GPIOR0, GPIOR00);
            clear_bit(&GPIOR0, GPIOR01);
        }

        x = adc_ch6;
        y = adc_ch7;
    }

    /*
     * process ADC sample
     */
    if (test_bit(&flags, GPIOR00)) {
        x >>= 3;
        x += x >> 10;

        y >>= 3;
        y += y >> 10;

        if (test_bit(&flags, GPIOR02)) {
            clear_bit(&GPIOR0, GPIOR02);
            x_offset = x;
            y_offset = y;
        }

        x = OFFSET_X(x);
        y = OFFSET_Y(y);

        led_write_number(0, x);
        led_write_number(1, y);

        switch (adc_state) {
        case ADC_START:
            if (test_bit(&flags, GPIOR01)) {
                adc_state = ADC_READY;
                sample_start = y;
            }
            break;
        case ADC_READY:
            if (sample_start > y)
                if ((sample_start - y) >= sample_delta)
                    adc_state = ADC_RUN;
                else
                    break;
            else
                if ((y - sample_start) >= sample_delta)
                    adc_state = ADC_RUN;
                else
                    break;
        case ADC_RUN:
            if (sample_count)
                printf("%5d: %4d %4d\n", sample_count, x, y);
            if (--sample_count == 0)
                adc_state = ADC_IDLE;
            break;
        }
    }
}

void do_zero(uint8_t nparm, uint32_t * parm)
{
    set_bit(&GPIOR0, GPIOR02);
    putchar('\n');
}

void do_adc(uint8_t nparm, uint32_t * parm)
{
    switch (nparm) {
    case 0:
        printf_P(PSTR(
            "\n"
            "usage: adc count [delta]\n"
            "\n"));
        return;
    case 1:
        sample_delta = 0;
        break;
    default:
        sample_delta = (uint16_t) parm[1];
        break;
    }

    sample_count = (uint16_t) parm[0];

    adc_restart();
    adc_state = ADC_START;

    while (sample_count) idle_task();
    putchar('\n');
}

void do_readram(uint8_t nparm, uint32_t * parm)
{
    uint8_t * addr;
    uint16_t cnt;

    switch (nparm) {
    case 0:
        printf_P(PSTR(
            "\n"
            "usage: rr address [count]\n"
            "\n"));
        return;
    case 1:
        cnt = 1;
        break;
    default:
        cnt = (uint16_t) parm[1];
        break;
    }

    addr = (uint8_t *) (uint16_t) parm[0];

    while (cnt) {
        uint8_t buffer[16];
        uint8_t n = (cnt > 16) ? (16) : (cnt);
        uint8_t p;

        printf_P(PSTR("%04x  "), (uint16_t) addr);
        for (p = 0; p < 16; p++) {
            if (p == 8) putchar(' ');
            if (p < n) {
                buffer[p] = addr[p];
                printf_P(PSTR("%02x "), buffer[p]);
            }
            else
                printf_P(PSTR("   "));
        }
        putchar(' ');
        for (p = 0; p < n; p++) {
            if (p == 8) putchar(' ');
            putchar(isprint(buffer[p]) ? buffer[p] : '.');
        }
        putchar('\n');

        addr += n;
        cnt -= n;
    }
    putchar('\n');
}

void do_readflash(uint8_t nparm, uint32_t * parm)
{
    uint8_t * addr;
    uint16_t cnt;

    switch (nparm) {
    case 0:
        printf_P(PSTR(
            "\n"
            "usage: rf address [count]\n"
            "\n"));
        return;
    case 1:
        cnt = 1;
        break;
    default:
        cnt = (uint16_t) parm[1];
        break;
    }

    addr = (uint8_t *) (uint16_t) parm[0];

    while (cnt) {
        uint8_t n = (cnt > 16) ? (16) : (cnt);
        uint8_t p;

        printf_P(PSTR("%04x  "), (uint16_t) addr);
        for (p = 0; p < 16; p++) {
            if (p == 8) putchar(' ');
            if (p < n)
                printf_P(PSTR("%02x "), pgm_read_byte(&addr[p]));
            else
                printf_P(PSTR("   "));
        }
        putchar(' ');
        for (p = 0; p < n; p++) {
            if (p == 8) putchar(' ');
            putchar(isprint(pgm_read_byte(&addr[p])) ?
                    pgm_read_byte(&addr[p]) : '.');
        }
        putchar('\n');

        addr += n;
        cnt -= n;
    }
    putchar('\n');
}

static void do_writeram(uint8_t nparm, uint32_t * parm)
{
    uint8_t * addr;
    uint16_t val;

    switch (nparm) {
    case 0:
    case 1:
        printf_P(PSTR(
            "\n"
            "usage: wr address value\n"
            "\n"));
        return;
    default:
        val = (uint16_t) parm[1];
        break;
    }

    addr = (uint8_t *) (uint16_t) parm[0];

    *addr = val;

    putchar('\n');
}

void do_help(uint8_t nparm, uint32_t * parm)
{
    printf_P(PSTR(
        "\n"
        "Command usage:\n"
        "\n"
        "  help - display this message\n"
        "  rr   - read RAM\n"
        "  rf   - read flash\n"
        "  wr   - write RAM\n"
        "  adc  - read ADC\n"
        "  zero - zero ADC\n"
        "\n"));
}


/******************************************************************************
 *
 * Ragel command line interpreter.
 *
 ******************************************************************************/
%%{
    #
    # ragel command line interpreter
    #
    machine command_line_interpreter;
}%%

/*
 * Command parameter state machines.
 */
%%{
    #
    # command parameters
    #

    action set_parameter {
        if (nparm < MAX_PARM) parm[nparm++] = acc;
    }

    action bin { acc = acc * 2  + (cv - '0'); }
    action oct { acc = acc * 8  + (cv - '0'); }
    action dec { acc = acc * 10 + (cv - '0'); }
    action hex {
        acc = acc * 16 + ((cv > '9') ? ((cv - 'a') + 10) : (cv - '0'));
    }

    action clr_acc { acc = 0; }

    parameter =
        ( ( '0'        [0-7]*    $oct ) |
          ( [1-9] $dec [0-9]*    $dec ) |
          ( '0b'       [0-1]+    $bin ) |
          ( '0o'       [0-7]+    $oct ) |
          ( '0d'       [0-9]+    $dec ) |
          ( '0x'       [0-9a-f]+ $hex ) ) >clr_acc %set_parameter;
}%%

/*
 * Command identfication state machines.
 */
%%{
    #
    # command identfication
    #

    action cmd_help { cmdid = CMDID_HELP;      }
    action cmd_rr   { cmdid = CMDID_READRAM;   }
    action cmd_rf   { cmdid = CMDID_READFLASH; }
    action cmd_wr   { cmdid = CMDID_WRITERAM;  }
    action cmd_adc  { cmdid = CMDID_ADC;       }
    action cmd_zero { cmdid = CMDID_ZERO;      }

    command =
        'rr'   %cmd_rr   |
        'rf'   %cmd_rf   |
        'wr'   %cmd_wr   |
        'adc'  %cmd_adc  |
        'zero' %cmd_zero |
        'help' %cmd_help ;
}%%

/*
 * Command execution state machines.
 */
%%{
    #
    # command execution
    #

    action execute {
        ((void (*)(uint8_t, uint32_t *)) pgm_read_word(&cmdtbl[cmdid]))
                                                      (nparm, parm);
    }

    action start_cmd { cmdid = 0; nparm = 0; }

    main := ((' '* '\n') |
             (' '* command >start_cmd (' '+ parameter)* ' '* '\n') @execute)*;
}%%

/*
 * Instantiate Ragel data set.
 */
%%write data;


/*
 * Command identifiers.
 */
enum {
    CMDID_HELP = 1,
    CMDID_READRAM,
    CMDID_READFLASH,
    CMDID_WRITERAM,
    CMDID_ADC,
    CMDID_ZERO,
};

/*
 * Command handler lookup table.
 */
__attribute__((__progmem__)) void (* const cmdtbl[])(uint8_t, uint32_t *) = {
    [CMDID_HELP]      = do_help,
    [CMDID_READRAM]   = do_readram,
    [CMDID_READFLASH] = do_readflash,
    [CMDID_WRITERAM]  = do_writeram,
    [CMDID_ADC]       = do_adc,
    [CMDID_ZERO]      = do_zero,
};

/*
 * Default maximum number of command parameters.
 */
#ifndef MAX_PARM
#define MAX_PARM (4)
#endif

/*
 * Command line interpreter variables.
 */
static uint8_t cmdid;
static uint8_t nparm;
static uint32_t parm[MAX_PARM];

void cmdline(void)
{
    uint32_t acc = 0;
    uint8_t cs;

    /*
     * Make console I/O non-blocking.
     */
    console_setattr(console_getattr() | INONBLOCK);

    /*
     * Instantiate Ragel initialization.
     */
    %%write init;

    for (;;) {
        printf_P(PSTR("cmdline$ "));

        for (;;) {
            int c;
            char cv;
            char * p;
            char * pe;

            while ((c = tolower(getchar())) == EOF) idle_task();
            cv = (char) c;
            p = &cv;
            pe = p + 1;

            /*
             * Instantiate Ragel state machines.
             */
            %%write exec;

            if (cs == command_line_interpreter_first_final) break;

            if (cs == command_line_interpreter_error) {
                if (!cmdid)
                    printf_P(PSTR("\ncmdline: command not found\n\n"));
                else
                    printf_P(PSTR("\ncmdline: parameter %d invalid\n\n"), nparm + 1);

                while (c != '\n') c = getchar();
                cs = command_line_interpreter_start;
                break;
            }
        }
    }
}
