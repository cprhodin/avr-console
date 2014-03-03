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
#include <util/atomic.h>
#include <avr/pgmspace.h>

extern void console_init(void);


/*
 * command interpreter
 */

%%{
    machine command_interpreter;
    write data;
}%%

%%{
    action stop_cmd {
        printf_P(PSTR("stop\n"));
    }

    action start_cmd {
        printf_P(PSTR("start\n"));
    }

    action put_cmd {
        printf_P(PSTR("put\n"));
    }

    action get_cmd {
        printf_P(PSTR("get\n"));
    }

    action return_cmd {
        printf_P(PSTR("return\n"));
    }

    action rest_cmd {
        printf_P(PSTR("rest\n"));
    }

    action reset_cmd {
        printf_P(PSTR("reset\n"));
    }

    command =
        'RETURN' @return_cmd |
        'REST' @rest_cmd |
        'RESET' @reset_cmd |
        'STOP' @stop_cmd |
        'START' @start_cmd |
        'PUT' @put_cmd |
        'GET' @get_cmd;

    main := space* command '\n';
}%%


int main(void)
{
    uint8_t cs;

    /*
     * initialize
     */
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        console_init();
    }
    /* interrupts are enabled */

    %%write init;

    printf_P(PSTR("\n\nUp, up and away!\n\n"));

    for (;;) {
        printf_P(PSTR("Command: "));

        for (;;) {
            int c;
            char cv;
            char * p;
            char * pe;

            if ((c = getchar()) == EOF) continue;

            cv = (char) c;
            p = &cv;
            pe = p + 1;

            %%write exec;
        }
    }
}

