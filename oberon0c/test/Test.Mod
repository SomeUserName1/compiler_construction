MODULE HelloWorld;

(* Length of the array to be sorted. *)
CONST Dim = 2 * 10;

(* Record definition for a two-dimensional point. *)
TYPE Point2D = RECORD x, y: INTEGER END;

(* Array to be sorted. *)
VAR a: ARRAY Dim OF INTEGER;
    p: Point2D;
    d: ARRAY Dim OF Point2D;


END HelloWorld.