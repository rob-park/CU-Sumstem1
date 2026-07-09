/* 
  Two-Player Color-Matching Whack-a-Mole Race
  Arduino Uno Version with Point/Winner Indicator LEDs

  WIRING:

  Shared color LEDs:
    Red LED    -> D2
    Green LED  -> D3
    Yellow LED -> D4
    Blue LED   -> D5

  Player 1 buttons:
    Red button    -> D6
    Green button  -> D7
    Yellow button -> D8
    Blue button   -> D9

  Point/winner indicator LEDs:
    Player 1 indicator LED -> D10
    Player 2 indicator LED -> D11

  Player 1 passive buzzer:
    Buzzer + -> D12
    Buzzer - -> GND

  Player 2 buttons:
    Red button    -> A1
    Green button  -> A2
    Yellow button -> A3
    Blue button   -> A4

  Player 2 passive buzzer:
    Buzzer + -> A5
    Buzzer - -> GND

  Open Serial Monitor at 9600 baud to see score.

  CODE ORGANIZATION:
  Player 1 and Player 2 use identical logic, so values are stored in
  2-element arrays and looped over by "player" index (0 = P1, 1 = P2).
  TIE (=2) is an out-of-range player index meaning "both".

  FILE MAP:
    1. Constants & Pins
    2. Game State
    3. Buzzer Tone Data
    4. Arduino Entry Points (setup/loop)
    5. Hardware Setup
    6. Game Flow
    7. Button Input Handling
    8. LED Display
    9. Buzzer / Audio
   10. Serial Output
*/

// ============================================================
// 1. CONSTANTS & PINS
// ============================================================


// Game settings & pin assignments. Nothing runs here, just labels.

const byte NUM_COLORS = 4;
const byte NUM_PLAYERS = 2;
const byte TIE = NUM_PLAYERS;  // tie / both players

const char* COLOR_NAMES[NUM_COLORS] = {
  "RED",
  "GREEN",
  "YELLOW",
  "BLUE"
};

const byte LED_PINS[NUM_COLORS] = {
  2, 3, 4, 5
};

// BUTTON_PINS[player][color]
const byte BUTTON_PINS[NUM_PLAYERS][NUM_COLORS] = {
  { 6, 7, 8, 9 },     // Player 1
  { A1, A2, A3, A4 }  // Player 2
};

const byte WIN_LED_PINS[NUM_PLAYERS] = { 10, 11 };
const byte BUZZER_PINS[NUM_PLAYERS] = { 12, A5 };

const byte MAX_ROUNDS = 10;
const byte POINTS_TO_WIN = 5;

const unsigned long ROUND_TIME_MS = 3000;
const unsigned long BETWEEN_ROUNDS_MS = 900;
const unsigned long DEBOUNCE_MS = 35;
const unsigned long CORRECT_ANSWER_PAUSE_MS = 350;

// ============================================================
// 2. GAME STATE
// ============================================================

// Tracks one button's raw reading, its "trusted" stable reading, when it
// last changed, and whether a fresh press just happened.

struct ButtonState {
  bool rawState;
  bool stableState;
  unsigned long lastRawChangeTime;
  bool pressEvent;
};

// buttons[player][color]
ButtonState buttons[NUM_PLAYERS][NUM_COLORS];

byte scores[NUM_PLAYERS] = { 0, 0 };
byte currentRound = 0;
byte previousColor = 255;

// ============================================================
// 3. BUZZER TONE DATA (each player's melodies as beep lists)
// ============================================================

// One note: pitch (Hz), how long to play it, and the silence after it.
struct Beep {
  unsigned int freq;
  unsigned int durationMs;
  unsigned int gapMs;
};

const Beep CORRECT_TONE_P1[] = { { 900, 80, 35 }, { 1300, 80, 35 }, { 1700, 100, 0 } };
const Beep WRONG_TONE_P1[] = { { 180, 550, 0 } };

const Beep CORRECT_TONE_P2[] = { { 1800, 60, 25 }, { 1100, 60, 25 }, { 1800, 60, 25 }, { 1100, 60, 25 } };
const Beep WRONG_TONE_P2[] = { { 300, 110, 50 }, { 190, 110, 50 }, { 300, 110, 50 }, { 190, 110, 50 }, { 300, 110, 50 }, { 190, 110, 50 } };

// Bundles a player's full "correct" and "wrong" melodies together.
struct ToneSet {
  const Beep* correct;
  byte correctLength;
  const Beep* wrong;
  byte wrongLength;
};

const ToneSet PLAYER_TONES[NUM_PLAYERS] = {
  { CORRECT_TONE_P1, sizeof(CORRECT_TONE_P1) / sizeof(Beep), WRONG_TONE_P1, sizeof(WRONG_TONE_P1) / sizeof(Beep) },
  { CORRECT_TONE_P2, sizeof(CORRECT_TONE_P2) / sizeof(Beep), WRONG_TONE_P2, sizeof(WRONG_TONE_P2) / sizeof(Beep) }
};

// ============================================================
// 4. ARDUINO ENTRY POINTS
// ============================================================


// Runs once at power-on: sets up all pins, seeds the random number
// generator with electrical noise so colors aren't the same every run,
// clears the LEDs, and plays a startup jingle.
void setup() {
  Serial.begin(9600);

  setupPins();
  initializeButtons();

  randomSeed(analogRead(A0) + micros());

  allLedsOff();

  Serial.println();
  Serial.println(F("======================================"));
  Serial.println(F(" Two-Player Color Whack-a-Mole Race"));
  Serial.println(F(" Indicator LED Version"));
  Serial.println(F("======================================"));
  Serial.println(F("Open Serial Monitor at 9600 baud to see score."));
  Serial.println(F("First to 5 points OR highest score after 10 rounds wins."));
  Serial.println();

  playGameStartSignal();
}

// Runs forever after setup(). Plays full games back to back: reset scores,
// play rounds until someone wins or 10 rounds pass, announce the winner,
// then freeze until the board is physically reset.
void loop() {
  resetGame();

  while (gameInProgress()) {
    currentRound++;
    playRound(currentRound);
    delay(BETWEEN_ROUNDS_MS);
  }

  announceWinner();

  Serial.println();
  Serial.println(F("Game complete. Press reset on the Arduino to play again."));
  Serial.println();

  while (true) {
    // Game is finished. Wait here until Arduino reset.
  }
}

// ============================================================
// 5. HARDWARE SETUP
// ============================================================

// Sets every LED/buzzer pin to OUTPUT and every button pin to
// INPUT_PULLUP (unpressed reads HIGH, pressed reads LOW).
void setupPins() {
  for (byte color = 0; color < NUM_COLORS; color++) {
    pinMode(LED_PINS[color], OUTPUT);
  }

  for (byte player = 0; player < NUM_PLAYERS; player++) {
    for (byte color = 0; color < NUM_COLORS; color++) {
      pinMode(BUTTON_PINS[player][color], INPUT_PULLUP);
    }

    pinMode(WIN_LED_PINS[player], OUTPUT);
    pinMode(BUZZER_PINS[player], OUTPUT);

    digitalWrite(WIN_LED_PINS[player], LOW);
    digitalWrite(BUZZER_PINS[player], LOW);
  }
}

// Takes a starting snapshot of all 8 buttons so updateButton()'s
// debounce logic has a real baseline before anyone presses anything.
void initializeButtons() {
  for (byte player = 0; player < NUM_PLAYERS; player++) {
    for (byte color = 0; color < NUM_COLORS; color++) {
      ButtonState& b = buttons[player][color];
      b.rawState = digitalRead(BUTTON_PINS[player][color]);
      b.stableState = b.rawState;
      b.lastRawChangeTime = millis();
      b.pressEvent = false;
    }
  }
}

// ============================================================
// 6. GAME FLOW
// ============================================================

// True while the game should keep going: under 10 rounds AND both
// scores still below the winning threshold.

bool gameInProgress() {
  return currentRound < MAX_ROUNDS && scores[0] < POINTS_TO_WIN && scores[1] < POINTS_TO_WIN;
}

// Clears scores, round counter, and last-color memory for a fresh game.
void resetGame() {
  scores[0] = 0;
  scores[1] = 0;
  currentRound = 0;
  previousColor = 255;
  allLedsOff();

  Serial.println(F("New game started!"));
  printScores();
  Serial.println();
}

// Picks a random target color, re-rolling if it matches last round's
// color so the same color never repeats back-to-back.

byte getRandomColor() {
  byte newColor = random(NUM_COLORS);

  while (newColor == previousColor) {
    newColor = random(NUM_COLORS);
  }

  previousColor = newColor;
  return newColor;
}
// Runs one full round: lights the target color, then polls both
// players' buttons for up to ROUND_TIME_MS, handing each press to
// handleButtonPress() until someone scores or time runs out.

void playRound(byte roundNumber) {
  clearButtonEvents();

  byte targetColor = getRandomColor();

  Serial.print(F("Round "));
  Serial.print(roundNumber);
  Serial.print(F(" / "));
  Serial.print(MAX_ROUNDS);
  Serial.print(F(" - Target color: "));
  Serial.println(COLOR_NAMES[targetColor]);

  playRoundStartSignal();
  lightTargetColor(targetColor);

  unsigned long roundStartTime = millis();
  bool roundEnded = false;
  bool wrongThisRound[NUM_PLAYERS] = { false, false };

  while (!roundEnded && millis() - roundStartTime < ROUND_TIME_MS) {
    updateAllButtons();

    for (byte player = 0; player < NUM_PLAYERS && !roundEnded; player++) {
      if (wrongThisRound[player]) continue;

      for (byte color = 0; color < NUM_COLORS; color++) {
        if (buttons[player][color].pressEvent) {
          buttons[player][color].pressEvent = false;
          roundEnded = handleButtonPress(player, color, targetColor, wrongThisRound);
          if (roundEnded) break;
        }
      }
    }
  }

  allLedsOff();

  if (!roundEnded) {
    Serial.println(F("Time expired. No point awarded."));
    playTimeoutSignal();
  }

  printScores();
  Serial.println();
}

// Scores an immediate point on a correct press (round ends). On a
// wrong press, locks that player out for the rest of the round so the
// other player can still try; ends the round with no score only if
// both players have now missed.
bool handleButtonPress(byte player, byte pressedColor, byte targetColor, bool wrongThisRound[]) {
  Serial.print(F("Player "));
  Serial.print(player + 1);
  Serial.print(F(" pressed "));
  Serial.print(COLOR_NAMES[pressedColor]);
  Serial.print(F(". "));

  if (pressedColor == targetColor) {
    allLedsOff();
    Serial.println(F("Correct! +1 point."));

    scores[player]++;
    showWinnerLeds(player);
    playCorrectTone(player);

    delay(CORRECT_ANSWER_PAUSE_MS);
    return true;
  }

  Serial.println(F("Wrong! No point. The other player can still try."));

  wrongThisRound[player] = true;
  playWrongTone(player);

  if (wrongThisRound[0] && wrongThisRound[1]) {
    allLedsOff();
    Serial.println(F("Both players were wrong. No point awarded."));
    return true;
  }

  lightTargetColor(targetColor);
  return false;
}


// Compares final scores, decides win/lose/tie, and triggers the
// matching LED + buzzer celebration.
void announceWinner() {
  allLedsOff();

  Serial.println(F("======================================"));
  Serial.println(F(" Final Result"));
  Serial.println(F("======================================"));
  printScores();

  byte winner;
  if (scores[0] > scores[1]) {
    winner = 0;
  } else if (scores[1] > scores[0]) {
    winner = 1;
  } else {
    winner = TIE;
  }

  if (winner == TIE) {
    Serial.println(F("TIE GAME!"));
    Serial.println(F("Tie LEDs: both indicator LEDs."));
  } else {
    Serial.print(F("PLAYER "));
    Serial.print(winner + 1);
    Serial.println(F(" WINS!"));
    Serial.print(F("Winner LED: Player "));
    Serial.print(winner + 1);
    Serial.print(F(" indicator on D"));
    Serial.println(WIN_LED_PINS[winner]);
  }

  showWinnerLeds(winner);
  playWinnerSignal(winner);
}

// ============================================================
// 7. BUTTON INPUT HANDLING (debouncing)
// ============================================================

// Clears the "just pressed" flag on every button before a new round.
void clearButtonEvents() {
  for (byte player = 0; player < NUM_PLAYERS; player++) {
    for (byte color = 0; color < NUM_COLORS; color++) {
      buttons[player][color].pressEvent = false;
    }
  }
}

// Refreshes the debounced state of all 8 buttons once per loop pass.
void updateAllButtons() {
  for (byte player = 0; player < NUM_PLAYERS; player++) {
    for (byte color = 0; color < NUM_COLORS; color++) {
      updateButton(buttons[player][color], BUTTON_PINS[player][color]);
    }
  }
}
// Debounces one button: only trusts a state change once it holds
// steady for DEBOUNCE_MS, then flags a press if it settled on LOW.
void updateButton(ButtonState& button, byte pin) {
  bool reading = digitalRead(pin);

  if (reading != button.rawState) {
    button.rawState = reading;
    button.lastRawChangeTime = millis();
  }

  if ((millis() - button.lastRawChangeTime) > DEBOUNCE_MS) {
    if (reading != button.stableState) {
      button.stableState = reading;

      if (button.stableState == LOW) {
        button.pressEvent = true;
      }
    }
  }
}

// ============================================================
// 8. LED DISPLAY
// ============================================================

// Turns off all LEDs, then lights only the requested target color.
void lightTargetColor(byte colorIndex) {
  allLedsOff();
  digitalWrite(LED_PINS[colorIndex], HIGH);
}

// Turns off every color LED and both winner-indicator LEDs.

void allLedsOff() {
  for (byte color = 0; color < NUM_COLORS; color++) {
    digitalWrite(LED_PINS[color], LOW);
  }

  for (byte player = 0; player < NUM_PLAYERS; player++) {
    digitalWrite(WIN_LED_PINS[player], LOW);
  }
}

// winner: 0/1 = player index, TIE = both. Each player's color LEDs are
// LED_PINS[2*player] and [2*player + 1].
// Lights a player's pair of color LEDs plus their indicator LED,
// or lights everything if the result is a tie.

void showWinnerLeds(byte winner) {
  allLedsOff();
  if (winner == TIE) {
    for (byte color = 0; color < NUM_COLORS; color++) {
      digitalWrite(LED_PINS[color], HIGH);
    }
    digitalWrite(WIN_LED_PINS[0], HIGH);
    digitalWrite(WIN_LED_PINS[1], HIGH);
  } else {
    digitalWrite(LED_PINS[winner * 2], HIGH);
    digitalWrite(LED_PINS[winner * 2 + 1], HIGH);
    digitalWrite(WIN_LED_PINS[winner], HIGH);
  }
}

// ============================================================
// 9. BUZZER / AUDIO
// ============================================================


// Generates the square wave; every beep/tone function below wraps this.
// Generates a square-wave tone by rapidly toggling the given pin(s)
// on/off at the right speed for the requested pitch and duration.
void toneOnPins(const byte* pins, byte pinCount, unsigned int frequency, unsigned int durationMs) {
  unsigned long halfPeriodUs = 1000000UL / (frequency * 2UL);
  unsigned long cycles = ((unsigned long)durationMs * 1000UL) / (halfPeriodUs * 2UL);

  for (unsigned long i = 0; i < cycles; i++) {
    for (byte p = 0; p < pinCount; p++) digitalWrite(pins[p], HIGH);
    delayMicroseconds(halfPeriodUs);

    for (byte p = 0; p < pinCount; p++) digitalWrite(pins[p], LOW);
    delayMicroseconds(halfPeriodUs);
  }

  for (byte p = 0; p < pinCount; p++) digitalWrite(pins[p], LOW);
}

// Plays a tone through one buzzer / through both buzzers at once.
void beepOne(byte buzzerPin, unsigned int frequency, unsigned int durationMs) {
  toneOnPins(&buzzerPin, 1, frequency, durationMs);
}

void beepBoth(unsigned int frequency, unsigned int durationMs) {
  toneOnPins(BUZZER_PINS, NUM_PLAYERS, frequency, durationMs);
}

// Plays a list of notes in order, building a short melody from
// single beeps.
void playSequence(byte buzzerPin, const Beep* sequence, byte length) {
  for (byte i = 0; i < length; i++) {
    beepOne(buzzerPin, sequence[i].freq, sequence[i].durationMs);
    if (sequence[i].gapMs > 0) delay(sequence[i].gapMs);
  }
}

// Plays this player's correct / wrong melody.
void playCorrectTone(byte player) {
  playSequence(BUZZER_PINS[player], PLAYER_TONES[player].correct, PLAYER_TONES[player].correctLength);
}

void playWrongTone(byte player) {
  playSequence(BUZZER_PINS[player], PLAYER_TONES[player].wrong, PLAYER_TONES[player].wrongLength);
}


// Fixed short jingles for round-start, timeout, game-start, and the
// final winner fanfare.
void playRoundStartSignal() {
  beepBoth(880, 90);
}

void playTimeoutSignal() {
  beepBoth(330, 140);
}

void playGameStartSignal() {
  beepBoth(660, 90);
  delay(60);
  beepBoth(880, 90);
  delay(60);
  beepBoth(990, 130);
}

void playWinnerSignal(byte winner) {
  for (byte i = 0; i < 3; i++) {
    if (winner == TIE) {
      beepBoth(1400, 180);
    } else {
      beepOne(BUZZER_PINS[winner], 1400, 180);
    }
    delay(180);
  }
}

// ============================================================
// 10. SERIAL OUTPUT
// ============================================================
// Prints the current score line to the Serial Monitor.

void printScores() {
  Serial.print(F("Score -> Player 1: "));
  Serial.print(scores[0]);
  Serial.print(F(" | Player 2: "));
  Serial.println(scores[1]);
}
