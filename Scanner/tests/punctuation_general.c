int x = 0;
int y = (x + 1);
{ x = y; }

Serial.println("A");
Serial.print("B");

x = x + 1;
y = y + (x * 2);

Serial.println("Done");