import Config
IO.puts("using FARMDUINO_K16 profile")

config :post, POST.Comms, device: "ttyACM0"

config :post, POST.ButtonSupervisor, button: POST.GPIOButton

config :post, POST.ButtonPopulator, buttons: [16, 22, 26, 5, 20]

config :post, POST.LEDSupervisor, led: POST.GPIOLED

config :post, POST.LEDPopulator, leds: [17, 23, 27, 06, 21, 24, 25, 12, 13]

config :post, POST.TestSuite,
  peripherals: [0, 1, 2, 3, 4, 5, 6],
  motors: [0, 1, 2, 3, 4]
