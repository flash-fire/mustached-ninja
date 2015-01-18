program test (input, output);
  var a : integer;
  var b : real;
  var c : array [1..2] of integer;

  procedure proc1(x:integer; y:real; 
                  z:array [1..2] of integer; q: real);
    var d: integer;
    begin
      a:= 2;
      z[a] := 4;
      c[3] := 3
     end;

   procedure proc2(x: integer; y: integer);
     var e: real;

     procedure proc3(n: integer; z: real);
       var e: integer;

       procedure proc4(a: integer; z: array [1..3] of real);
         var x: integer;
         begin
           a:= e 
         end;

       begin
         a:= e;
         e:= c[e]
       end;

     begin
       call proc1(x, e, c, b);
       call proc3(c[1], e);
       e := e + 4.44;
       a:= (a mod y) div x;
       while ((a >= 4) and ((b <= e) 
                       or (not (a = c[a])))) do
         begin
           a:= c[a] + 1
         end
     end;

begin
  call proc2(c[4], c[5]);
  call proc2(c[4],2);
  if (a < 2) then a:= 1 else a := a + 2;
  if (b > 4.2) then a := c[a]
end.
