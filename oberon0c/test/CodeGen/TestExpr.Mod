MODULE CodeGen;

        CONST Pi = 3;

        VAR x, y, z, t: INTEGER;

        BEGIN
        y := Pi; (* assert 3 *)
        z := -Pi * y;  (* assert -9 *)
        t := -2;

        (* BUG: AST returns 5 * (3/4) instead of (5*3)/4 *)
        x := 5 * 3 DIV 4; (* assert 3; returns 0 *)

        x := -10; (* assert 10 *)
        y := 3; (* assert 3 *)

        (*Klappt nicht: parser fails: [ERROR] Expected a factor but was something else factor() expected but got "-" *)
        (* z := -10 DIV -2; *)
        (* z := -3 * -2; *)
        (* z := -1 + -1; *)
        (* z := -1 - -1 *)

        (* klappt *)
        z := -10 DIV (-2); (* assert 5 *)
        z := x DIV t;      (* assert 5 *)


        (* Z = 0; should be 5 *)
        x := (Pi * (-z) - y) * (Pi * z - y); (* assert -216 *)
        y := -x * (1 DIV z); (* assert 0 *)
        y := -x * (10 DIV z); (* assert 432 *)

        (* Number too large *)
        x := 1234567891;
        y := 64;
        z := x DIV y; (* assert 19 290 123 *)

        (* zero division:  9227 floating point exception (core dumped)  ./out *)
        (* x := 1 DIV 0; *)

        (* left first precedence on sums *)
        x := 1+2-3;     (* assert 0 *)


        x := 17 MOD 5;  (* assert 2 *)
        x := (-17) MOD 5; (* assert 3 *)
        x := 17 MOD (-5);  (* assert -3 *)
        x := (-17) MOD (-5); (* assert -2 *)


        (* Write(x); *)
        (* Write(y); *)
        (* Write(z); *)
        END CodeGen.