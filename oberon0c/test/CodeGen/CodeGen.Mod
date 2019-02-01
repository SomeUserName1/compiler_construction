MODULE CodeGen;

        CONST Pi = 3;

        VAR x, y, z: INTEGER;

        BEGIN
        x := 10;                                (* assert 10 *)
        y := 3;                                 (* assert 3 *)
        z := x DIV (y + 1);                     (* assert 2 *)
        x := (Pi * z - y) * (Pi * z - y);       (* assert 9 *)
        y := x * (1 DIV z)                      (* assert 0 *)
        (* Write(x); *)
        (* Write(y); *)
        (* Write(z); *)
        END CodeGen.