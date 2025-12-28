# Dictionary

So we need to be able to define more complex words - this is done using a
dictionary. A dictionary is (in my mind) a parallel set of arrays: one array for
the "Keys" (strings) and another for "Values" (integers or whatever).

For our Forth CPU, we'll need at least one dictionary: one that holds user
defined Words and maps them to Compiled Opcodes. 

## Requirements

For the dictionary to be useful, we need to be able to:

- DEFINE new key value pairs
- SET a key to a particular value
- GET a value from a particular key
- DELETE a key value pair

Other behaviors that would be nice to have:

- Tell us if a Key wasn't in the dictionary
- 1 clock SET and GET and DELETE
- Parametrizes sizes and length
- Testable

## Implementation

Modules:

- src/dict.v
- src/find_index.v

I broke the dictionary up into two problems:

1. Finding if a provided Key matches a known Key
2. Everything else

I figure that for our dictionary to work, we need to have two parallel lists, in
which each key and value share a common integer index into those lists. 

### Find Index of Provided Key

To find the index which corresponds to the provided key is pretty simple: check
all the known keys and see if they match:

``` verilog
always @(posedge i_update) begin
    o_found = 0;
    o_index = 0;

    for(int i = 0; i < ENTRIES; i++) begin
        if ((0 == o_found) && (i_key == i_keys[i])) begin
            o_index = i;
            o_found = 1;
        end
    end
end
```

The block uses the `i_update` line to do work. I figured that this would be best
done asynchronously (IE without an `i_clk`) since it's just some gates and
stuff.

Two of these blocks are used by `dict.v`: one do find the index of the provided
key, and one to find an index of EMPTY keys:

``` verilog
find_index #(
    .ENTRIES(ENTRIES),
    .KEY_WIDTH(KEY_WIDTH),
    .KEY_LENGTH(KEY_LENGTH)
) find_key (
    .i_update(update_index),
    .i_key(i_key),
    .i_keys(keys),
    .o_index(index),
    .o_found(found_key)
);

...

find_index #(
    .ENTRIES(ENTRIES),
    .KEY_WIDTH(KEY_WIDTH),
    .KEY_LENGTH(KEY_LENGTH)
) find_empty (
    .i_update(update_index),
    .i_key(empty_key),
    .i_keys(keys),
    .o_index(index_empty),
    .o_found(found_empty)
);
```

Finding an empty index is a simple way to find an open slot for saving key /
value pairs.

### Dictionary

Because I offloaded the `find_index` stuff into a separate module, and we want
the dictionary to be a clocked system, it will take at least one clock to
find empty slots / look up keys (since we have no way of knowing if the provided
KEY is new or already is in the dictionary).

To save a clock or so, I decided to add an alternate way to access dicitonary
information: an INDEX. This allows one clock access to the dictionary if you
know the slot that the data is saved in. This information is provided on SET via
`o_index`, and can be queried using the ENCODE opcode with the KEY.

Because some states require two clocks to complete, there is also a STATE
MACHINE implemented to keep track of what to do next. The state is visible and
testable from `d_state`.

Normal inputs:

- `i_clk`
- `i_rst` : Active HIGH
- `i_en`

Parameters:

- `ENTRIES` : Number of Key / Value pairs
- `KEY_WIDTH` : Key Character Size
- `KEY_LENGTH` : Number of Key Characters
- `VALUE_WIDTH` : Value Character Size
- `VALUE_LENGTH` : Number of Value Characters

Key Value related stuff:

- `i_op` - Opcode
- `i_key` - KEY String
- `i_value` - VALUE 
- `i_ready` - Inputs are VALID
- `o_value` - Output VALUE 
- `o_index` - Output INDEX
- `o_done` - Outputs are VALID
- `o_err` - An error occurred

Index related stuff:

- `i_index` - INDEX to access the KEYs and VALUEs with

Debug related stuff:

- `d_state`

#### OP CODES

OP CODES with the `_FAST` suffix use the `i_index` input in addittion to the
normal inputs.

``` verilog
localparam OP_SET = 0;
localparam OP_GET = 1;
localparam OP_ENCODE = 2;
localparam OP_SET_FAST = 3;
localparam OP_GET_FAST = 4;
localparam OP_DELETE = 5;
localparam OP_DELETE_FAST = 6;
```

#### STATES

Because the `_FAST` operations occur in one clock, no STATE is needed for those.

``` verilog
localparam STATE_IDLE = 0;
localparam STATE_SET = 1;
localparam STATE_GET = 2;
localparam STATE_ENCODE = 3;
localparam STATE_DELETE = 4;
```

#### Access

The main way to access elements in the dictionary is using the KEY, which is
typically a string I suppose. This requires two clocks to complete any
operations: one clock to find the provided key, and another to actually to the
work.

The user specifies the KEY, and sets the OPCODE to the appropriate value, and
asserts the READY signal. When the dictionary is ready, the DONE signal will be
asserted for one clock. The VALUE and ERR signals will be persistant until
updated in another transaction.

An alternative way to access data is to use the index and the FAST OP CODES.
This allows direct access to the Dictionary using the index. This saves a whole
clock cycle on all dictionary operations.

Why provide a second way to access data?

This *should* allow the CPU to run commands out of the Dictionary by providing
the Index, which should simplify the CPU processing a bit.

Something like: `Dxxx` -> `OP_GET_FAST index=xxx` -> Value without having to
use the Key.

#### Block Initialization

1. Set `i_rst` high and back to low
2. Set `i_en` high

#### Normal Dictionary Operation

1. Configure `i_op` to the appropriate OP CODE
2. Set `i_key` to the expected KEY
3. Set `i_value` to the appropriate VALUE
4. Set `i_index` to the appropriate INDEX
5. Set `i_ready` to start 
6. Wait for `i_done` to process the outputs

#### KEY VALUE SET

SET can be used to define a new KEY VALUE pair, or to change the VALUE of a
specified KEY. Using `OP_SET` takes two clock cycles: one to see if the key is
already in the dictionary, and one to update the dictionary.

After the `index` is updated from `find_index`:

``` verilog
if (1'b1 == found_key)
begin
    keys[index] <= i_key;
    values[index] <= i_value;
    o_index <= index;
    o_err <= 0;
end
else if (1'b1 == found_empty)
begin
    keys[index_empty] <= i_key;
    values[index_empty] <= i_value;
    o_index <= index_empty;
    o_err <= 0;
end
else // Error - No place to save data
begin
    o_err <= 1;
end
o_done <= 1;
d_state <= STATE_IDLE;
update_index <= 0;
```

where:

- `update_index` is used to trigger the `find_index` blocks
- `index` and `found_key` are from the `find_index::find_key` block
- `index_empty` and `found_empty` are from the `find_index::find_empty` block
- `empty_key` is the default KEY used in the dictionary
- `empty_value` is the default VALUE used in the dictionary

When using `OP_SET_FAST`, things are a bit more simple since the index is
provided:

``` verilog
keys[i_index] <= i_key;
o_index <= i_index;
o_done <= 1;
o_err <= 0;
```

#### KEY VALUE GET

GET can be used to retreive a VALUE from a known KEY. Using `OP_GET` takes two
clock cycles: one to see if the key is in the dictionary, and one to actually
retreive the information.

After the `index` is updated from `find_index`:

``` verilog
if (1'b1 == found_key)
begin
    o_value <= values[index];
    o_index <= index;
    o_err <= 0;
end
else
begin
    o_value <= empty_value;
    o_index <= 0;
    o_err <= 1;
end
o_done <= 1;
d_state <= STATE_IDLE;
update_index <= 0;
```

where:

- `update_index` is used to trigger the `find_index` blocks
- `index` and `found_key` are from the `find_index::find_key` block

When using `OP_GET_FAST`, it's even more simple:

``` verilog
o_value <= values[i_index];
o_index <= i_index;
o_done <= 1;
o_err <= 0;
```

#### KEY VALUE DELETE

DELETE is used to remove KEY VALUE pairs from the dictionary. Just like `OP_GET`
and `OP_SET`, `OP_DELETE` takes two clock cycles: one to see if the key is in
the dictionary, and one to actually delete the information.

After the `index` is updated from `find_index`:

``` verilog
if (1'b1 == found_key)
begin
    o_index <= index;
    o_value <= values[index];

    // ERASE
    for (int i = 0; i < KEY_LENGTH; i++) keys[index] <= empty_key;
    for (int i = 0; i < VALUE_LENGTH; i++) values[index] <= empty_value;

    o_err <= 0;
end
else
begin
    o_index <= 0;
    o_value <= empty_value;
    o_err <= 1;
end
o_done <= 1;
d_state <= STATE_IDLE;
update_index <= 0;
```

where:

- `update_index` is used to trigger the `find_index` blocks
- `index` and `found_key` are from the `find_index::find_key` block
- `empty_key` is the default KEY used in the dictionary
- `empty_value` is the default VALUE used in the dictionary

When using `OP_DELETE_FAST` it's a bit more simple:

``` verilog
o_value <= values[i_index];
o_index <= i_index;

// ERASE
for (int i = 0; i < KEY_LENGTH; i++) keys[i_index] <= empty_key;
for (int i = 0; i < VALUE_LENGTH; i++) values[i_index] <= empty_value;

o_done <= 1;
o_err <= 0;
```

#### ENCODE

The ENCODE operation is used to retreive the `index` from the provided `i_key`
in the dictionary. There is no FAST version (since you have to know the index
for the FAST stuff), so this always takes two clocks. The index is provided on
`SET` and `GET`, but... it doesn't hurt to have a way to look it up anyways.

`ENCODE` takes two clocks in the same way as `GET`, `SET`, and `DELETE`: one
clock to look up the key, and one to actually do the operation.

``` verilog
if (1'b1 == found_key)
begin
    o_index <= index;
    o_err <= 0;
end
else
begin
    o_index <= 0;
    o_err <= 1;
end
o_done <= 1;
d_state <= STATE_IDLE;
update_index <= 0;
```

- `update_index` is used to trigger the `find_index` blocks
- `index` and `found_key` are from the `find_index::find_key` block

