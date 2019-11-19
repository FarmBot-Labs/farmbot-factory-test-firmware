defmodule POST.PlatformSupervisor do
  use Supervisor

  def start_link(args, opts \\ [name: __MODULE__]) do
    Supervisor.start_link(__MODULE__, args, opts)
  end

  def init(_args) do
    children = Application.get_env(:post, __MODULE__)[:children] || []
    Supervisor.init(children, strategy: :one_for_all)
  end
end
