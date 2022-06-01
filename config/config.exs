# This file is responsible for configuring your application
# and its dependencies with the aid of the Config module.
#
# This configuration file is loaded before any dependency and
# is restricted to this project.
import Config

config :post, target: Mix.target()

# Customize non-Elixir parts of the firmware. See
# https://hexdocs.pm/nerves/advanced-configuration.html for details.

config :nerves, :firmware, rootfs_overlay: "rootfs_overlay"

# Use shoehorn to start the main application. See the shoehorn
# docs for separating out critical OTP applications such as those
# involved with firmware updates.

config :shoehorn,
  init: [:nerves_runtime],
  app: Mix.Project.config()[:app]

# Use Ringlogger as the logger backend and remove :console.
# See https://hexdocs.pm/ring_logger/readme.html for more information on
# configuring ring_logger.

config :logger, backends: [:console]
config :logger, handle_sasl_reports: true, handle_otp_reports: true

config :post, POST.PlatformSupervisor, children: []

if Mix.target() != :host do
  import_config "target.exs"
else
  import_config "profiles/FARMDUINO_K16.ex"
  # import_config "profiles/EXPRESS_K11.ex"
  # config :post, POST.Comms, device: "ttyUSB0"
  config :post, POST.ButtonSupervisor, button: POST.NullButton
  config :post, POST.LEDSupervisor, led: POST.NullLED
end
