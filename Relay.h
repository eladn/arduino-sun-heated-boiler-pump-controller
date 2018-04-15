#ifndef RELAY_H_
#define RELAY_H_

/* Relay module has opposite-logic. */
#define RELAY_ON(pin) digitalWrite(pin, LOW);
#define RELAY_OFF(pin) digitalWrite(pin, HIGH);

#endif /* RELAY_H_ */
