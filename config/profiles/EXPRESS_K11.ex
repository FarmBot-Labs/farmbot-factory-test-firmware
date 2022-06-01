import Config
IO.puts("using EXPRESS_K11 profile")

config :post, POST.Comms, device: "ttyUSB0"

config :post, POST.ButtonSupervisor, button: POST.GPIOButton

config :post, POST.ButtonPopulator, buttons: [16, 22]

config :post, POST.LEDSupervisor, led: POST.GPIOLED

config :post, POST.LEDPopulator, leds: [17, 23, 24, 25]

config :post, POST.TestSuite, peripherals: [0, 1, 2]
config :post, POST.TestSuite, motors: [0, 1, 2, 3]
