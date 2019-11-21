defmodule POST.Application do
  # See https://hexdocs.pm/elixir/Application.html
  # for more information on OTP Applications
  @moduledoc false

  use Application

  @spec start(Application.start_type(), any()) :: {:error, any} | {:ok, pid()}
  def start(_type, _args) do
    # See https://hexdocs.pm/elixir/Supervisor.html
    # for other strategies and supported options
    opts = [strategy: :one_for_one, name: POST.Supervisor]
    registry_name = registry_name()

    children = [
      {Registry, [keys: :duplicate, name: registry_name]},
      POST.PlatformSupervisor,
      POST.Comms.FlashFirmware,
      {POST.ButtonTable, registry_name},
      POST.ButtonSupervisor,
      POST.ButtonPopulator,
      POST.LEDSupervisor,
      POST.LEDPopulator,
      POST.TestSuite
    ]

    Supervisor.start_link(children, opts)
  end

  def registry_name() do
    DepTracker.Registry
  end
end
