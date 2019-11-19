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

    children = [
      POST.PlatformSupervisor
      # Children for all targets
      # Starts a worker by calling: POST.Worker.start_link(arg)
      # {POST.Worker, arg},
    ]

    Supervisor.start_link(children, opts)
  end
end
