use Mix.Config

config :post, POST.PlatformSupervisor, children: [POST.SSH]
import_config "profiles/FARMDUINO_K15.ex"
