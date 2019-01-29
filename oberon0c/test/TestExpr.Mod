MODULE CodeGen;

        CONST Pi = 3;

        VAR x, y, z: INTEGER;

        BEGIN
        (*y := Pi; *)
        (*z := -Pi * y; *)
        (* BUG: AST returns 5 * (3/4) instead of (5*3)/4 *)
        (*x := 5 * 3 DIV 4; *)

        x := -10;
        y := 3;
        (* BUG: parser fails: [ERROR] Expected a factor but was something else factor() expected but got "-" *)
        (* z := -10 DIV -2; *)
        z := x DIV (-2);
        (* Z = 0; should be 5 *)
        x := (Pi * (-z) - y) * (Pi * z - y);
        y := -x * (1 DIV z)
        (* zero division:  9227 floating point exception (core dumped)  ./out *)
        (* Write(x); *)
        (* Write(y); *)
        (* Write(z); *)
        END CodeGen.