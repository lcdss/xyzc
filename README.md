### XYZ Compiler

XYZC is a incomplete compiler project, specified by a teacher, whose syntax
is demonstrated below (yes, it's the ugliest thing I ever seen):

```c
main(){
  int &birthday, &money = 100 + (50 * 11);
  int &age = 2017 - &birthday;
  char &name(100) = "Your name" + "is unknown";
  char &motto(50);
  char &message(250);
  dec &balance(10.2);
  dec &value(1.3) = 5.789 + (1.554 * 4.875);

  &birthday = 1980;

  gets(&name);
  gets(&name, &balance);
  puts("My name is " + &name + "and my age is " + &age);

  if (&age >= 18) then {
    puts("You are of legal age.");
  } else {
    puts("You are not old enough.");
    &age++;
  }

  if (&age < 18) then &money = 0;

  if (&age < 18) then puts("Money is " + &money);
  else &money = &money * 2;

  int &i = 0;

  for (int &j = 0, &l = &age; &i <= &age; &i++) {
    puts("j = " + &j + ", l = " + &l);
  }

  if (&i == &age + 1) then {
    puts("Loop goes until the end");

    if (&i != &age + (&money * 3 / 5) + &birthday &&
        &age < &i ||
        &age == &i + 1 * 2
    ) then {
      &age = &age * 2;
    } else {
      &age = &age / 2;
    }
  }
}

```

#### Info
- The XYZC was tested using the GCC version 4.9.2 and 6.3.0.
- The flag `-std=c99` or prior needs to be specified during the compilation
  process to get `for` variable declaration support.
- This project has only implements only the lexer, parser and a poor semantic
  analyzer
