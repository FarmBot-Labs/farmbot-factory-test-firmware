# FarmBot factory test firmware

Tests electronics box LEDs,
Farmduino peripherals using load sensors (including rotary tool on Genesis),
and motors using encoders.

Press the red E-STOP button to proceed through tests.

All LEDs flashing indicates error.

```
MIX_TARGET=rpi4 mix deps.get
MIX_TARGET=rpi4 mix firmware.image factory_test_genesis_v1.6_pi4.img
MIX_TARGET=rpi02w mix deps.get
MIX_TARGET=rpi02w mix firmware.image factory_test_express_v1.1_pi02w.img
```

[Nerves docs](https://hexdocs.pm/nerves/getting-started.html)
