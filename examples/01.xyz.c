// All the code should be inside the main statement
main(){
  int &birthday, &money = (100 + 50) * 11; // Variable declaration
  int &age = 2017 - &birthday;
  char &name(100) = "Your name" + "is unknown";
  char &motto(50) = &name + "Name";
  char &message(250) = &motto + " Last";
  dec &balance(10.2);
  dec &value(1.3) = 5.789 + 1.554 * 4.875;

  &birthday = 1980;

  gets(&name);
  gets(&name, &balance);
  puts("My name is " + &name + "and my age is " + &age);

  /*
  if (&age >= 18) then {
    puts("You are of legal age.");
  } else {
    puts("You are not old enough.");
    &age++;
  }
  */

  if (&age < 18) then &money = 0;

  if (&age < 18) then puts("Money is " + &money);
  else &money = &money * 2;

  int &i = 0;

  for (int &j = 0, &l = &age; &i <= &age; &i++) {
    puts("j = " + &j + ", l = " + &l);
  }

  if (&i == &age + 1) then {
    puts("Loop goes until the end");

    if (&i != (&age + &money) * 3 / (5 + &birthday) && (&age < &i || &age == (&i + 1) * 2)) then {
      &age = &age * 2;
    } else {
      &age = &age / 2;
    }
  }
}
