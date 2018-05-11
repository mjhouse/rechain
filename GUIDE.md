## Style Guide

### Functions

#### Naming

Function names should be snake case (`my_function()`) and when they just get or set a value
they should be prepended with `get_` and `set_`. Names should be only a few words long whenever
possible for readability (like `find_publication`) although this isn't mandatory.

Parameters to functions should be prepended with `t_` so that in the body of the function it's
clear that they are given values and not local or member variables. Additionally, parameters
should have full and descriptive names, not single letters or cryptic combinations of characters. 

##### Good variable names:

* `t_hash`
* `t_file_handle`
* `t_record`

##### Bad variable names:

* `t_hash_of_record_to_find`
* `p`
* `t_vptr_fh`
* `t_f`

#### Logging

All functions should start with a debug statement (like: `RCDEBUG("finding a record by hash: " + t_hash)`) and
use the other logging macros (RCINFO, RCWARNING and RCERROR) liberally. These statements should be in each branch
of each check, so for example:

``` c++
bool Manager::is_valid(){
    RCDEBUG("checking if blockchain is valid");
    
    if(blockchain->is_valid()){
        RCINFO("blockchain is valid");
        return true;
    }
    
    RCWARNING("blockchain is not valid");
    return false;
}
```

This ensures that regardless of the result of the operation, there is logging output. Additionally, try/catch
should have logging statements in the catch block. Even if the exception will just be re-thrown, it should be
logged before it is thrown again.
