(* Implementation of the Bubble Sort algorithm. *)
MODULE BubbleSort;

(* Length of the array to be sorted. *)
CONST Dim = 2 * 10;
      Dim1 = 3 * 10;
      Dim2 = 4 * 10;

(* Record definition for a two-dimensional point. *)
TYPE Point2D = RECORD x, y: INTEGER END;
     Point3D = RECORD x, y, z: INTEGER; w: Point2D END;
     Point4D = RECORD w, x, y, z: INTEGER END;
     Point5D = Point4D;
     Point6D = ARRAY 6 OF INTEGER;
     Point7D = RECORD x,y,z: Point6D END;
     Point8D = RECORD x,y,z: Point7D END;

(* Array to be sorted. *)
VAR a: ARRAY Dim OF INTEGER;
    p: Point2D;
    c: ARRAY Dim1 OF INTEGER;
    d: Point3D;
    o,r,q : INTEGER;
    e: Point7D;
    f: RECORD a,b,c: INTEGER END;
    g: Point8D;

(* Initializes the array. *)
PROCEDURE Init();
VAR i: INTEGER;
BEGIN
    i := 0;
    WHILE i < Dim DO
        a[i] := Dim-i;
        i := i + 1
    END
END Init;

(* Swaps the two values passed as var-parameters. *)
PROCEDURE Swap(VAR a, b: INTEGER; VAR c : INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b; 
    b := t
END Swap;
    
(* Applies the bubble sort algorithm to the array. *)
PROCEDURE Bubble;
VAR i, j: INTEGER;
BEGIN
    i := Dim;
    WHILE i > 1 DO
        j := i-1; 
        WHILE j > 0 DO
            IF (a[i] > a[j]) & (2 > 1)
            THEN
                Swap(a[i], a[j])
            ELSIF 1 > 2
            THEN
                WHILE 1 > 2 DO END;
            ELSE
                WHILE 1 > 2 DO END;
            END;
            j := j + 1
        END;
        i := i + 1
        (* ; s := "Hello \"World\"!\n" *)
    END
END Bubble;

(* Main program. *)
BEGIN
    Init;
    Bubble();
    (* Testing sophisticated selectors *)
    g.x.x[2] := 1;
    e.x.x[2] := e.x[2];
END BubbleSort.