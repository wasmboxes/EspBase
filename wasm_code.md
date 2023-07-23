(module
  (func $print      (import "imports" "print")      (param i32))
  (func $print_cstr (import "imports" "print_cstr") (param i32))
  
  (memory $mem 1)
  (data (i32.const 1048) "\01\00\00\00\08\00\00\00i\00n\00i\00t")
  
  (table 2 funcref)
  (elem (i32.const 0)
    $result_ok
    $result_error
  )
  
  (func $setCell (param $x i32) (param $value i32)
    local.get $x
    local.get $value
    i32.store
  )
  
  (func $getCell (param $x i32) (result i32)
    local.get $x
    i32.load
  )
    
  (func $add (param i32 i32) (result i32 i32)
    local.get 0
    local.get 1
    call $print
    i32.const 1
    call $getCell
   )

  (func $result_ok (result i32)
    i32.const 1
  )

  (func $result_error (result i32)
    i32.const 0
  )
  
  (func $getString (result i32 i32)
    i32.const 1056
    call $print_cstr
    i32.const 0
    i32.const 2
  )
  
  (export "add" (func $add))
  (export "getString" (func $getString))
  (export "memory" (memory $mem))
)
