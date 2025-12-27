
#ifndef DICT_APIS_H_
#define DICT_APIS_H_

#include "Vdict.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define ENTRIES (10)
#define KEY_WIDTH (8)
#define KEY_LENGTH (1)

#define VALUE_WIDTH (32)
#define VALUE_LENGTH (1)

enum dict_ops {
    OP_SET = 0,
    OP_GET,
    OP_ENCODE,
    OP_SET_FAST,
    OP_GET_FAST,
    OP_DELETE,
    OP_DELETE_FAST
};

enum dict_states {
    STATE_IDLE = 0,
    STATE_SET,
    STATE_GET,
    STATE_ENCODE,
    STATE_DELETE
};

void op_set(char name, uint32_t value);
void op_get(char name);
void op_delete(char name);

// Untested helpers
void op_encode(char name);
void op_set_fast(uint8_t index, char name, uint32_t value);
void op_get_fast(uint8_t index);
void op_delete_fast(uint8_t index);

#endif  // DICT_APIS_H_
 
// EOF
