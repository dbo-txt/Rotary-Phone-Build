Rotary Dial Phone Restoration & Embedded Control
Overview

This project involves restoring the core functionality of a vintage rotary dial telephone using a microcontroller-based control system.

The goal is to decode rotary dial pulse signals, interpret user input reliably, and trigger audio feedback through a speaker while maintaining the original mechanical components (motorised ringer, hook switch, rotary dial).

The project focuses on embedded systems design, interrupt-driven input handling, and hardware–software integration.

Hardware

RaspberryPi Pico

Vintage rotary dial mechanism

Hook switch detection

Speaker for audio playback

Custom-designed PCB (schematic + layout completed and populated)

Software Design
Pulse Decoding

The rotary dial generates a series of electrical pulses corresponding to the dialled number.

Includes:

GPIO interrupt handling to detect pulse edges

Timing-based pulse counting to determine digit values

Debounce logic to reduce signal noise

State-based digit parsing

This allows reliable decoding of numbers entered via mechanical rotation.

System Logic

Detect off-hook state via switch input

Capture and decode dial pulses

Parse complete digits

Trigger corresponding audio playback

Reset system state after number completion

The system uses structured, modular code to separate hardware input handling from higher-level logic.
