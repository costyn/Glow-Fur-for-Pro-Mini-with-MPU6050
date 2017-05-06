# Glow Fur for Arduino Pro Mini with MPU6050

Adafruit Glow Fur for Arduino Pro Mini

I adapted the code from Adafruit with additional routines for the Pro Mini, which has a lot more space and room to play with.

https://learn.adafruit.com/animated-neopixel-gemma-glow-fur-scarf?embeds=allow

=== Hardware Notes ===

!!! WARNING !!! Do not use onboard power regulator to power LEDs. It will quickly fry!!  (don't use the RAW pin for voltage input!)

Wiring:
- VCC to 5v on LED strip
- GND to GND on LED strip
- Pin 12 to Din (data IN) on LED strip
- Pin 2 or 3 (see BUTTON_PIN in code) and GND to a momentary push button

Put regulated 5v to any other VCC pin on the board. For example from a USB Power Bank (any that can provide 1000 mAh or more).
