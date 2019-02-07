MODULE CodeGen;

VAR a, b, v, x, y, z: INTEGER;

BEGIN
    v := 3;
    x := -3;
    y := 5;
    z := -5;

    a := v - y; (* pos + pos; assertEquals -2 *)
    b := y - v; (* pos + pos; assertEquals 2 *)

    a := y - x; (* pos + neg; assertEquals 8 *)
    b := x - y; (* neg + pos; assertEquals -8 *)

    a := v - x; (* pos + neg; assertEquals 6 *)
    b := x - v; (* neg + pos; assertEquals -6 *)

    a := v - z; (* pos + neg; assertEquals 8 *)
    b := z - v; (* neg + pos; assertEquals -8 *)

    a := z - x; (* pos + neg; assertEquals -2 *)
    b := x - z (* neg + pos; assertEquals 2 *)

END CodeGen.