#include <Arduino.h>

// v0.1: ESP32 + 定时提醒 + 按钮响应 + 分级提醒
// 逻辑尽量简单，先验证行为闭环。

const int BTN_PIN = 18;
const int LED_PIN = 2;
const int BUZZER_PIN = 23;
const int MOTOR_PIN = 19;

const unsigned long FOCUS_INTERVAL_MS = 45UL * 60UL * 1000UL;
const unsigned long SITTING_INTERVAL_MS = 60UL * 60UL * 1000UL;
const unsigned long RESPONSE_WINDOW_MS = 2UL * 60UL * 1000UL;
const unsigned long COOL_DOWN_MS = 10UL * 60UL * 1000UL;
const unsigned long ADAPT_STEP_MS = 5UL * 60UL * 1000UL;
const unsigned long MIN_INTERVAL_MS = 20UL * 60UL * 1000UL;

enum ReminderType {
  REMINDER_NONE,
  REMINDER_FOCUS,
  REMINDER_SITTING
};

enum ReminderStage {
  STAGE_IDLE,
  STAGE_WARN_1,
  STAGE_WARN_2,
  STAGE_ESCALATED
};

ReminderType activeReminder = REMINDER_NONE;
ReminderStage stage = STAGE_IDLE;
ReminderStage nextStartStage = STAGE_WARN_1;

unsigned long lastFocusStartMs = 0;
unsigned long lastMovementMs = 0;
unsigned long reminderStartMs = 0;
unsigned long coolDownUntilMs = 0;
unsigned long lastHeartbeatMs = 0;
unsigned long stageTickMs = 0;

int ignoreCount = 0;
int responseCount = 0;

bool buttonPressed() {
  return digitalRead(BTN_PIN) == LOW;
}

void setFeedback(bool ledOn, bool buzzerOn, bool motorOn) {
  digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
  digitalWrite(BUZZER_PIN, buzzerOn ? HIGH : LOW);
  digitalWrite(MOTOR_PIN, motorOn ? HIGH : LOW);
}

void clearReminder() {
  activeReminder = REMINDER_NONE;
  stage = STAGE_IDLE;
  setFeedback(false, false, false);
}

void startReminder(ReminderType type) {
  activeReminder = type;
  stage = nextStartStage;
  reminderStartMs = millis();
  stageTickMs = millis();
}

void escalateStage() {
  if (stage == STAGE_WARN_1) {
    stage = STAGE_WARN_2;
  } else if (stage == STAGE_WARN_2) {
    stage = STAGE_ESCALATED;
  }
  stageTickMs = millis();
}

void applyStageOutput() {
  switch (stage) {
    case STAGE_WARN_1:
      setFeedback(true, false, false);
      break;
    case STAGE_WARN_2:
      setFeedback(true, true, false);
      break;
    case STAGE_ESCALATED:
      setFeedback(true, true, true);
      break;
    default:
      setFeedback(false, false, false);
      break;
  }
}

void recordResponse() {
  responseCount++;
  if (ignoreCount > 0) {
    ignoreCount--;
  }
  nextStartStage = (ignoreCount >= 3) ? STAGE_ESCALATED : (ignoreCount >= 1 ? STAGE_WARN_2 : STAGE_WARN_1);
  coolDownUntilMs = millis() + COOL_DOWN_MS;
  lastFocusStartMs = millis();
  lastMovementMs = millis();
  clearReminder();
}

unsigned long adaptiveInterval(unsigned long baseMs) {
  unsigned long reduceMs = (unsigned long)ignoreCount * ADAPT_STEP_MS;
  if (reduceMs >= baseMs - MIN_INTERVAL_MS) {
    return MIN_INTERVAL_MS;
  }
  return baseMs - reduceMs;
}

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  Serial.begin(115200);

  clearReminder();
  lastFocusStartMs = millis();
  lastMovementMs = millis();
}

void loop() {
  unsigned long now = millis();

  if (buttonPressed()) {
    delay(30);
    if (buttonPressed()) {
      recordResponse();
      while (buttonPressed()) {
        delay(10);
      }
    }
  }

  if (activeReminder == REMINDER_NONE && now < coolDownUntilMs) {
    delay(20);
    return;
  }

  if (activeReminder == REMINDER_NONE) {
    unsigned long focusInterval = adaptiveInterval(FOCUS_INTERVAL_MS);
    unsigned long sittingInterval = adaptiveInterval(SITTING_INTERVAL_MS);
    bool focusDue = (now - lastFocusStartMs) >= focusInterval;
    bool sittingDue = (now - lastMovementMs) >= sittingInterval;

    if (focusDue) {
      startReminder(REMINDER_FOCUS);
    } else if (sittingDue) {
      startReminder(REMINDER_SITTING);
    }
  }

  if (activeReminder != REMINDER_NONE) {
    if ((now - stageTickMs) >= 15000UL) {
      if (stage != STAGE_ESCALATED) {
        escalateStage();
      }
    }

    if ((now - reminderStartMs) >= RESPONSE_WINDOW_MS) {
      ignoreCount++;
      coolDownUntilMs = now + COOL_DOWN_MS;
      if (ignoreCount >= 3) {
        nextStartStage = STAGE_ESCALATED;
      } else if (ignoreCount >= 1) {
        nextStartStage = STAGE_WARN_2;
      }
      clearReminder();
      lastFocusStartMs = now;
      lastMovementMs = now;
    } else {
      applyStageOutput();
    }
  }

  if (now - lastHeartbeatMs >= 5000UL) {
    lastHeartbeatMs = now;
    Serial.print("active=");
    Serial.print((int)activeReminder);
    Serial.print(" stage=");
    Serial.print((int)stage);
    Serial.print(" ignore=");
    Serial.print(ignoreCount);
    Serial.print(" response=");
    Serial.println(responseCount);
  }

  delay(30);
}
