use Mix.Config
IO.puts("using EXPRESS_K10 profile")

config :post, POST.Comms, device: "ttyAMA0"

config :post, POST.Comms.FlashFirmware, reset: POST.GPIOReset

config :post, POST.ButtonSupervisor, button: POST.GPIOButton

config :post, POST.ButtonPopulator, buttons: [16]

config :post, POST.LEDSupervisor, led: POST.GPIOLED

config :post, POST.LEDPopulator, leds: [17]

config :post, POST.TestSuite, peripherals: [0, 1, 2]
