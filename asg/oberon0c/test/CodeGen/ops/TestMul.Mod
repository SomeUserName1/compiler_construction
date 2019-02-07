MODULE CodeGen;

VAR a, b, v, x, y, z: INTEGER;

BEGIN
    v := 3;
    x := -3;
    y := 5;
    z := -5;

    a := v * y; (* pos + pos; assertEquals 15 *)
    b := y * v; (* pos + pos; assertEquals 15 *)

    a := y * x; (* pos + neg; assertEquals -15 *)
    b := x * y; (* neg + pos; assertEquals -15 *)

    a := v * x; (* pos + neg; assertEquals -9 *)
    b := x * v; (* neg + pos; assertEquals -9 *)

    a := v * z; (* pos + neg; assertEquals -15 *)
    b := z * v; (* neg + pos; assertEquals -15 *)

    a := z * x; (* pos + neg; assertEquals 15 *)
    b := x * z (* neg + pos; assertEquals 15 *)
END CodeGen.