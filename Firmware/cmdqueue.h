#ifndef CMDQUEUE_H
#define CMDQUEUE_H

#include "Marlin.h"
#include "language.h"


// String circular buffer. Commands may be pushed to the buffer from both sides:
// Chained commands will be pushed to the front, interactive (from LCD menu) 
// and printing commands (from serial line or from SD card) are pushed to the tail.
// First character of each entry indicates the type of the entry: 
#define CMDBUFFER_CURRENT_TYPE_UNKNOWN  0
// Command in cmdbuffer was sent over USB.
#define CMDBUFFER_CURRENT_TYPE_USB      1
// Command in cmdbuffer was read from SDCARD.
#define CMDBUFFER_CURRENT_TYPE_SDCARD   2
// Command in cmdbuffer was generated by the UI.
#define CMDBUFFER_CURRENT_TYPE_UI       3
// Command in cmdbuffer was generated by another G-code.
#define CMDBUFFER_CURRENT_TYPE_CHAINED  4
// Command has been processed and its SD card length has been possibly pushed 
// to the planner queue, but not yet removed from the cmdqueue. 
// This is a temporary state to reduce stepper interrupt locking time.
#define CMDBUFFER_CURRENT_TYPE_TO_BE_REMOVED 5
//Command in cmdbuffer was sent over USB and contains line number
#define CMDBUFFER_CURRENT_TYPE_USB_WITH_LINENR 6

// How much space to reserve for the chained commands
// of type CMDBUFFER_CURRENT_TYPE_CHAINED,
// which are pushed to the front of the queue?
// Maximum 5 commands of max length 20 + null terminator.
#define CMDBUFFER_RESERVE_FRONT       (5*21)

extern char cmdbuffer[BUFSIZE * (MAX_CMD_SIZE + 1) + CMDBUFFER_RESERVE_FRONT];
extern size_t bufindr;
extern int buflen;
extern bool cmdbuffer_front_already_processed;
extern bool cmdqueue_serial_disabled;

// Type of a command, which is to be executed right now.
#define CMDBUFFER_CURRENT_TYPE   (cmdbuffer[bufindr])
// String of a command, which is to be executed right now.
#define CMDBUFFER_CURRENT_STRING (cmdbuffer+bufindr+CMDHDRSIZE)

// Enable debugging of the command buffer.
// Debugging information will be sent to serial line.
//#define CMDBUFFER_DEBUG

extern uint32_t sdpos_atomic;

extern int serial_count;
extern bool comment_mode;
extern char *strchr_pointer;

extern unsigned long TimeSent;
extern unsigned long TimeNow;

extern long gcode_N;
extern long gcode_LastN;
extern long Stopped_gcode_LastN;

extern bool cmdqueue_pop_front();
extern void cmdqueue_reset();
#ifdef CMDBUFFER_DEBUG
extern void cmdqueue_dump_to_serial_single_line(int nr, const char *p);
extern void cmdqueue_dump_to_serial();
#endif /* CMDBUFFER_DEBUG */
extern bool cmd_buffer_empty();
extern void enquecommand(const char *cmd, bool from_progmem = false);
extern void enquecommand_front(const char *cmd, bool from_progmem = false);
extern void repeatcommand_front();
extern void get_command();
extern uint16_t cmdqueue_calc_sd_length();

// Return True if a character was found
static inline bool    code_seen(char code) { return (strchr_pointer = strchr(CMDBUFFER_CURRENT_STRING, code)) != NULL; }
static inline bool    code_seen_P(const char *code_PROGMEM) { return (strchr_pointer = strstr_P(CMDBUFFER_CURRENT_STRING, code_PROGMEM)) != NULL; }
static inline float   code_value()      { return strtod(strchr_pointer+1, NULL);}
static inline long    code_value_long()    { return strtol(strchr_pointer+1, NULL, 10); }
static inline int16_t code_value_short()   { return int16_t(strtol(strchr_pointer+1, NULL, 10)); };
static inline uint8_t code_value_uint8()   { return uint8_t(strtol(strchr_pointer+1, NULL, 10)); };

static inline float code_value_float()
{
    char* e = strchr(strchr_pointer, 'E');
    if (!e) return strtod(strchr_pointer + 1, NULL);
    *e = 0;
    float ret = strtod(strchr_pointer + 1, NULL);
    *e = 'E';
    return ret;
}


#endif //CMDQUEUE_H
