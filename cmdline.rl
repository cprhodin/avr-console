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
#include <stdint.h>
#include <ctype.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>

#include "timer.h"
#include "tick.h"
#include "librb.h"
#include "console.h"

#define MAX_PARMS (4)


enum {
    CMDID_HELP = 1,
    CMDID_READRAM,
    CMDID_READFLASH,
    CMDID_WRITERAM,
    CMDID_TICKEN,
    CMDID_TICKPER,
    CMDID_STOP,
    CMDID_START,
    CMDID_PUT,
    CMDID_PUTTER,
    CMDID_GET,
    CMDID_RETURN,
    CMDID_REST,
    CMDID_RESET,
    CMDID_STTY,
    CMDID_QUIT,
};


void exec_stop(uint8_t nparm, uint32_t * parm)
{
    uint8_t i = 0;

    fputs_P(PSTR("STOP!"), stdout);
    while (i++ < nparm) printf_P(PSTR(" %ld"), *parm++);
    putchar('\n');
}

void exec_stub(uint8_t nparm, uint32_t * parm)
{
    uint8_t i = 0;

    fputs_P(PSTR("stub"), stdout);
    while (i++ < nparm) printf_P(PSTR(" %ld"), *parm++);
    putchar('\n');
}

void do_readram(uint8_t nparm, uint32_t * parm)
{
    uint8_t * addr;
    uint16_t cnt;

    if (nparm < 2) {
        fputs_P(PSTR(
            "\n"
            "usage: rr address count\n"
            "\n"), stdout);
        return;
    }

    addr = (uint8_t *) (uint16_t) parm[0];
    cnt = (uint16_t) parm[1];

    while (cnt) {
        uint8_t n = (cnt > 16) ? (16) : (cnt);
        uint8_t p;

        printf_P(PSTR("%04x  "), (uint16_t) addr);
        for (p = 0; p < 16; p++) {
            if (p == 8) putchar(' ');
            if (p < n)
                printf_P(PSTR("%02x "), addr[p]);
            else
                fputs_P(PSTR("   "), stdout);
        }
        putchar(' ');
        for (p = 0; p < n; p++) {
            if (p == 8) putchar(' ');
            putchar(isprint(addr[p]) ? addr[p] : '.');
        }
        putchar('\n');

        addr += n;
        cnt -= n;
    }
}

void do_readflash(uint8_t nparm, uint32_t * parm)
{
    uint8_t * addr;
    uint16_t cnt;

    if (nparm < 2) {
        fputs_P(PSTR(
            "\n"
            "usage: rf address count\n"
            "\n"), stdout);
        return;
    }

    addr = (uint8_t *) (uint16_t) parm[0];
    cnt = (uint16_t) parm[1];

    while (cnt) {
        uint8_t n = (cnt > 16) ? (16) : (cnt);
        uint8_t p;

        printf_P(PSTR("%04x  "), (uint16_t) addr);
        for (p = 0; p < 16; p++) {
            if (p == 8) putchar(' ');
            if (p < n)
                printf_P(PSTR("%02x "), pgm_read_byte(&addr[p]));
            else
                fputs_P(PSTR("   "), stdout);
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
}

void do_writeram(uint8_t nparm, uint32_t * parm)
{
    uint8_t * addr;
    uint16_t cnt;

    if (nparm < 1) {
        fputs_P(PSTR(
            "\n"
            "usage: wr address\n"
            "\n"), stdout);
        return;
    }

    addr = (uint8_t *) (uint16_t) parm[0];
    cnt = (uint16_t) parm[1];

    while (cnt) {
        uint8_t n = (cnt > 16) ? (16) : (cnt);
        uint8_t p;

        printf_P(PSTR("%04x  "), (uint16_t) addr);
        for (p = 0; p < 16; p++) {
            if (p == 8) putchar(' ');
            if (p < n)
                printf_P(PSTR("%02x "), addr[p]);
            else
                fputs_P(PSTR("   "), stdout);
        }
        putchar(' ');
        for (p = 0; p < n; p++) {
            if (p == 8) putchar(' ');
            putchar(isprint(addr[p]) ? addr[p] : '.');
        }
        putchar('\n');

        addr += n;
        cnt -= n;
    }
}

void do_ticken(uint8_t nparm, uint32_t * parm)
{
    if (nparm < 1) {
        fputs_P(PSTR(
            "\n"
            "usage: te enable\n"
            "\n"), stdout);
        return;
    }

    tick_enable((uint8_t) parm[0]);
}

void do_tickper(uint8_t nparm, uint32_t * parm)
{
    if (nparm < 1) {
        fputs_P(PSTR(
            "\n"
            "usage: tp period\n"
            "\n"), stdout);
        return;
    }

    tick_set_period(TBTICKS_FROM_MS(parm[0]));
}

void do_stty(uint8_t nparm, uint32_t * parm)
{
    uint16_t attr;

    /*
     * display console attributes in an stty like way
     */
    attr = console_getattr();
    printf_P(PSTR("\nstty: %cicrnl %cecho %cicanon %cinonblock %ciascii %conlcr\n\n"),
             (attr & ICRNL)    ?'+':'-',
             (attr & ECHO)     ?'+':'-',
             (attr & ICANON)   ?'+':'-',
             (attr & INONBLOCK)?'+':'-',
             (attr & IASCII)   ?'+':'-',
             (attr & ONLCR)    ?'+':'-');
}

void do_help(uint8_t nparm, uint32_t * parm)
{
    fputs_P(PSTR(
        "\n"
        "Command usage:\n"
        "\n"
        "  help - display this message\n"
        "  rr   - read RAM\n"
        "  rf   - read flash\n"
        "  wr   - write RAM\n"
        "  te   - enable tick\n"
        "  tp   - set tick period\n"
        "\n"), stdout);
}


__attribute__((__progmem__)) void (* const cmdtbl[])(uint8_t, uint32_t *) = {
    [CMDID_HELP]      = do_help,
    [CMDID_READRAM]   = do_readram,
    [CMDID_READFLASH] = do_readflash,
    [CMDID_WRITERAM]  = do_writeram,
    [CMDID_TICKEN]    = do_ticken,
    [CMDID_TICKPER]   = do_tickper,
    [CMDID_STTY]      = do_stty,
    [CMDID_STOP]      = exec_stop,
    [CMDID_START]     = exec_stub,
    [CMDID_PUT]       = exec_stub,
    [CMDID_PUTTER]    = exec_stub,
    [CMDID_GET]       = exec_stub,
    [CMDID_RETURN]    = exec_stub,
    [CMDID_REST]      = exec_stub,
    [CMDID_RESET]     = exec_stub,
    [CMDID_QUIT]      = exec_stub,
};


/******************************************************************************
 *
 * command line processing
 *
 ******************************************************************************/

%%{
    #
    # ragel command line interpreter
    #
    machine command_line_interpreter;

    #
    # command parameters
    #

    action set_parameter {
        if (nparm < MAX_PARMS) parm[nparm++] = acc;
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

    #
    # command identfication
    #

    action cmd_help   { cmdid = CMDID_HELP;      }
    action cmd_rr     { cmdid = CMDID_READRAM;   }
    action cmd_rf     { cmdid = CMDID_READFLASH; }
    action cmd_wr     { cmdid = CMDID_WRITERAM;  }
    action cmd_te     { cmdid = CMDID_TICKEN;    }
    action cmd_tp     { cmdid = CMDID_TICKPER;   }
    action cmd_stty   { cmdid = CMDID_STTY;      }
    action cmd_quit   { cmdid = CMDID_QUIT;      }
    action cmd_return { cmdid = CMDID_RETURN;    }
    action cmd_rest   { cmdid = CMDID_REST;      }
    action cmd_reset  { cmdid = CMDID_RESET;     }
    action cmd_stop   { cmdid = CMDID_STOP;      }
    action cmd_start  { cmdid = CMDID_START;     }
    action cmd_put    { cmdid = CMDID_PUT;       }
    action cmd_get    { cmdid = CMDID_GET;       }

    command =
        'help'   %cmd_help   |
        'rr'     %cmd_rr     |
        'rf'     %cmd_rf     |
        'wr'     %cmd_wr     |
        'te'     %cmd_te     |
        'tp'     %cmd_tp     |
        'stty'   %cmd_stty   |
        'quit'   %cmd_quit   |
        'return' %cmd_return |
        'rest'   %cmd_rest   |
        'reset'  %cmd_reset  |
        'stop'   %cmd_stop   |
        'start'  %cmd_start  |
        'put'    %cmd_put    |
        'get'    %cmd_get    ;

    #
    # command execution
    #

    action execute {
        ((void (*)(uint8_t, uint32_t *)) pgm_read_word(&cmdtbl[cmdid]))(nparm, parm);
    }

    action start_cmd { cmdid = 0; nparm = 0; }

    main := ((' '* '\n') |
             (' '* command >start_cmd (' '+ parameter)* ' '* '\n') @execute)*;

    # ragel global data set
    write data;
}%%


static uint8_t cmdid;

static uint8_t nparm;
static uint32_t parm[MAX_PARMS];

void cmdline(void)
{
    uint8_t cs;
    uint32_t acc = 0;

    /* ragel initialization */
    %%write init;

    fputs_P(PSTR("\n\nRagel Based Command Interpreter\n\n"), stdout);

    for (;;) {
        fputs_P(PSTR("Command: "), stdout);

        for (;;) {
            int c;
            char cv;
            char * p;
            char * pe;

            while ((c = tolower(getchar())) == EOF);
            cv = (char) c;
            p = &cv;
            pe = p + 1;

            /* ragel state machine */
            %%write exec;

            if (cs == command_line_interpreter_first_final) break;

            if (cs == command_line_interpreter_error) {
                if (!cmdid)
                    fputs_P(PSTR("\nERROR: Invalid command\n\n"), stdout);
                else
                    fputs_P(PSTR("\nERROR: Invalid parameter\n\n"), stdout);

                while (c != '\n') c = getchar();
                cs = command_line_interpreter_start;
                break;
            }
        }
    }
}
