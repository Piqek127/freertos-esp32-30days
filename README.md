# FreeRTOS on ESP32 — 30-Day Challenge

> My attempt to complete ACLAB's 30-Day RTOS Challenge and learn **FreeRTOS, ESP-IDF, and real-time embedded programming** on the ESP32.

I'm documenting the entire process here: code, experiments, notes, bugs, and things I misunderstood along the way.

**Curriculum:** [ACLAB 30-Day RTOS Challenge](https://github.com/nhanksd85/ACLAB_Challanges_RTOS_30Days)

**Started:** 2026-09-29
**Target finish:** 2026-11-01
**Progress:** `0 / 30`

---

## 🎯 Goals

By the end of this challenge, I want to be comfortable with:

* FreeRTOS tasks and scheduling
* Task priorities and states
* Queues, semaphores, mutexes, and notifications
* Interrupts and ISRs
* Timers and memory management
* ESP32 multicore programming
* Debugging and runtime monitoring
* Basic real-time performance considerations

More importantly, I want to be able to **design and debug small multitasking embedded systems myself**, rather than simply copying FreeRTOS examples.

---

## 🛠️ Setup

| Component | Details                             |
| --------- | ----------------------------------- |
| Board     | ESP32-DevKitC / Velxio simulator    |
| Framework | ESP-IDF                             |
| Language  | C                                   |
| Editor    | VS Code + ESP-IDF Extension         |
| OS        | Windows / Linux                     |
| Hardware  | LEDs, resistors, button, breadboard |

### Build / Flash / Monitor

From inside a day's project directory:

```bash
idf.py set-target esp32
idf.py build
idf.py flash
idf.py monitor
```

Or, once the project is configured:

```bash
idf.py build flash monitor
```

---

# 📚 30-Day Progress

**Legend:**
⬜ Not started · 🟨 In progress · ✅ Complete

### Week 1 — Tasks & Scheduling

| Day | Topic                             | Status | Project                       |
| --- | --------------------------------- | ------ | ----------------------------- |
| 1   | Intro to RTOS & FreeRTOS          | ✅      | [Day 01](Day_01_Intro/)       |
| 2   | ESP-IDF with VS code              | ✅      | [Day 02](Day_02_ESP_IDF/)     |
| 3   | Scheduling & Core Affinity        | ⬜      | [Day 03](Day_03_Scheduling/)  |
| 4   | Creating & Deleting Tasks         | ⬜      | [Day 04](Day_04_Tasks/)       |
| 5   | Task States & Priorities          | ⬜      | [Day 05](Day_05_States/)      |
| 6   | `vTaskDelay` vs `vTaskDelayUntil` | ⬜      | [Day 06](Day_06_Delay/)       |
| 7   | Two LEDs, Two Tasks               | ⬜      | [Day 07](Day_07_Two_LEDs/)    |

### Week 2 — Communication & Synchronization

| Day | Topic                       | Status | Project                         |
| --- | --------------------------- | ------ | ------------------------------- |
| 8   | Queue Usage                 | ⬜      | [Day 08](Day_08_Queue_Monitor/) |
| 9   | Queues with ISRs            | ⬜      | [Day 09](Day_09_Queue_ISR/)     |
| 10  | Binary Semaphores           | ⬜      | [Day 10](Day_10_Binary_Sem/)    |
| 11  | Counting Semaphores         | ⬜      | [Day 11](Day_11_Counting_Sem/)  |
| 12  | Mutexes & Recursive Mutexes | ⬜      | [Day 12](Day_12_Mutexes/)       |
| 13  | Priority Inversion          | ⬜      | [Day 13](Day_13_Inversion/)     |
| 14  | UART Logger + Mutex         | ⬜      | [Day 14](Day_14_UART_Logger/)   |

### Week 3 — Advanced FreeRTOS

| Day | Topic                    | Status | Project                         |
| --- | ------------------------ | ------ | ------------------------------- |
| 15  | Event Groups             | ⬜      | [Day 15](Day_15_Event_Groups/)  |
| 16  | Task Notifications       | ⬜      | [Day 16](Day_16_Notifications/) |
| 17  | Stream & Message Buffers | ⬜      | [Day 17](Day_17_Buffers/)       |
| 18  | Software Timers          | ⬜      | [Day 18](Day_18_Timers/)        |
| 19  | Memory Management        | ⬜      | [Day 19](Day_19_Memory/)        |
| 20  | Interrupt Management     | ⬜      | [Day 20](Day_20_Interrupts/)    |
| 21  | Idle & Daemon Tasks      | ⬜      | [Day 21](Day_21_Idle_Daemon/)   |

### Week 4 — Debugging & Performance

| Day   | Topic                      | Status | Project                        |
| ----- | -------------------------- | ------ | ------------------------------ |
| 22    | Tick Rate & Tickless Idle  | ⬜      | [Day 22](Day_22_Tick/)         |
| 23    | Debugging FreeRTOS Apps    | ⬜      | [Day 23](Day_23_Debugging/)    |
| 24    | Power Management           | ⬜      | [Day 24](Day_24_Power/)        |
| 25    | Inter-Task Communication   | ⬜      | [Day 25](Day_25_Integration/)  |
| 26    | Real-Time Trace Debugging  | ⬜      | [Day 26](Day_26_Trace/)        |
| 27    | Error Handling & Watchdogs | ⬜      | [Day 27](Day_27_Watchdogs/)    |
| 28    | Real-Time Performance      | ⬜      | [Day 28](Day_28_Optimization/) |
| 29–30 | **Capstone Project**       | ⬜      | [Capstone](Capstone/)          |

---

# 📁 Repository Structure

```text
.
├── README.md
│
├── docs/
│   ├── cheatsheet.md
│   └── common-mistakes.md
│
├── Day_01_02_Intro/
├── Day_03_Scheduling/
├── Day_04_Tasks/
├── ...
├── Day_28_Optimization/
│
└── Capstone/
```

Each day will ideally contain:

```text
Day_XX/
├── README.md
├── CMakeLists.txt
├── main/
│   ├── CMakeLists.txt
│   └── main.c
└── images/
```

The daily README will document:

* **Goal** — what I was trying to learn
* **Implementation** — what I built
* **What I learned** — concepts that finally clicked
* **Results** — serial output / hardware behavior
* **Gotchas** — bugs and mistakes
* **Things I still don't understand**

---

# 🧠 Key Takeaways

I'll update this section as I progress.

### Week 1 — Tasks & Scheduling

*

### Week 2 — Queues, Semaphores & Mutexes

*

### Week 3 — Events, Notifications, Timers & Memory

*

### Week 4 — Debugging, Power & Watchdogs

*

---

# 🐛 Things I Got Wrong

This is where I'll record misconceptions and mistakes that were useful to learn from.

1. **Day X:** I thought `...`, but actually `...`.
2. **Day X:**
3. **Day X:**

---

# 🚀 Capstone

The final project will combine concepts learned throughout the challenge into a small real-time embedded system.

### Project

**TBD**

### Concepts used

* Tasks
* Scheduling
* Inter-task communication
* Synchronization
* Interrupts
* Timers
* Error handling
* Debugging

### Demo

Coming later.

---

# 🤔 What I'd Do Differently

I'll fill this in after completing the challenge.

Things I'll evaluate:

* Was the 30-day pacing realistic?
* Which topics required the most time?
* Which resources were most useful?
* What concepts should I have learned earlier?
* What would I change if I did the challenge again?

---

# 📖 Resources

* [ACLAB 30-Day RTOS Challenge](https://github.com/nhanksd85/ACLAB_Challanges_RTOS_30Days)
* *Mastering the FreeRTOS Real Time Kernel*
* ESP-IDF FreeRTOS documentation

---

## License

MIT
