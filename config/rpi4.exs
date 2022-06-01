import Config

config :post, POST.PlatformSupervisor, children: []
import_config "profiles/FARMDUINO_K16.ex"
