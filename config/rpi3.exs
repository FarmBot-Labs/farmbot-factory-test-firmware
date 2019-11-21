use Mix.Config

config :post, POST.PlatformSupervisor, children: [POST.SSH]
