MODULE CodeGen;

VAR a, b, c, d, v, x, y, z: INTEGER;

BEGIN
        v := 4;
        x := -4;
        y := 12;
        z := -12;
        c := -10;
        d := -3;


    a := v DIV y; (* assertEquals 0 => correct *)
    b := y DIV v; (* assertEquals 3  => correct *)

    a := y DIV x; (* assertEquals -3  => correct *)
    b := x DIV y; (* assertEquals 0 => correct *)

    a := v DIV x; (* assertEquals -1  => correct *)
    b := x DIV v; (* assertEquals -1  => correct *)

    a := v DIV z; (* assertEquals  0 => correct *)
    b := z DIV v; (* assertEquals -3 => correct *)

    a := z DIV x; (* assertEquals 3  => correct *)
    b := x DIV z; (* assertEquals 0 => correct *)

    a := x DIV y; (* assertEquals 0 => correct *)
    b := x DIV z; (* assertEquals 0 => correct *)

    a := c DIV 3; (* assertEquals -3 => correct *)
    b := c DIV d; (* assertEquals 3 => correct *)
END CodeGen.