use Mix.Config
IO.puts("using NULL profile")

config :post, POST.Comms, device: nil

config :post, POST.Comms.FlashFirmware, reset: POST.Comms.FlashFirmware.NullReset

config :post, POST.ButtonSupervisor, button: POST.NullButton
config :post, POST.ButtonPopulator, buttons: [16]

config :post, POST.LEDSupervisor, led: POST.NullLED
config :post, POST.LEDPopulator, leds: [17, 18, 19]

config :post, POST.TestSuite,
  peripherals: [7, 8, 9],
  motors: [0, 1, 2, 3]
