use Mix.Config

config :post, POST.PlatformSupervisor,
  children: [
    POST.SSH,
    POST.GPIOReset
  ]

import_config "profiles/EXPRESS_K10.ex"
