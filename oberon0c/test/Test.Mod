MODULE HelloWorld;

(* Length of the array to be sorted. *)
CONST Dim = 20;

(* Record definition for a two-dimensional point. *)
(* TYPE Point2D = RECORD x, y: INTEGER END; *)

(* Array to be sorted. *)
VAR a: ARRAY Dim OF INTEGER;
    (* p: Point2D; *)
    i, j: INTEGER;

BEGIN
    i := Dim;
    WHILE i > 1 DO
        j := i-1;
        i := a[i] - a[j];
        WHILE j > 0 DO
            IF (a[i] > a[j]) & (2+(3*5)-2 > 1) THEN
                i := a[i] - a[j];
            ELSIF 1 > 2 THEN
                WHILE 1 > 2 DO END;
            ELSE
                WHILE 1 > 2 DO END;
            END;
            j := j - 1
        END;
        i := i - 1
    END
END HelloWorld.