#include<avr/io.h>

#define OE 0    // PORTB
#define CLK 1   // PORTB
#define DATA 2  // PORTB
#define SHLD 0  // OutA

#define Lo(bit) (PORTB &= ~(1 << bit))
#define Hi(bit) (PORTB |= (1 << bit))

uint8_t outA = 0;
uint8_t inA = 0;

void UpdateOutput();
void UpdateInput();

int main() {
    DDRB |= (1 << OE) | (1 << DATA) | (1 << CLK); // Set as output
    Lo(OE);

    while (1) {
        UpdateInput();
        if (inA & 0b00000001) {
            outA |= 0b00000001;
        } else {
            outA &= ~0b00000001;
        }
        UpdateOutput();
    }
}

void UpdateOutput() {
    DDRB |= 1 << DATA; // Switch DATA pin as output
    Lo(CLK);
    uint8_t x = outA;
    for (uint8_t i = 0; i < 8; i++) {
        if (x & 128) {
            Hi(DATA);
        } else {
            Lo(DATA);
        }
        Hi(CLK);
        Lo(CLK);
        x = x << 1;
    }
    Hi(OE);
    Lo(OE);
}

void UpdateInput() {
    outA &= ~(1 << SHLD); // Enable 74165 pin reading, load it to 74165 internal register
    UpdateOutput();
    outA |= 1 << SHLD; // Disable pin reading
    UpdateOutput();

    DDRB &= ~(1 << DATA); // Switch DATA pin as input
    Hi(DATA); // Pull up resistor

    Lo(CLK);
    uint8_t x = 0;
    for (uint8_t i = 0; i < 8; i++) {
        Hi(CLK);
        x = x >> 1;
        if (PINB & (1 << DATA)) {
            x |= 128;
        }
        Lo(CLK);
    }
    inA = x;
}
