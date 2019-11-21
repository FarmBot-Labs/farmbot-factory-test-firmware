defmodule POST.ButtonPopulator do
  require Logger
  use GenServer

  def start_link(args, opts \\ [name: __MODULE__]) do
    GenServer.start_link(__MODULE__, args, opts)
  end

  def init(args) do
    Logger.info("populating buttons")

    for button_id <- Application.get_env(:post, __MODULE__)[:buttons] do
      POST.ButtonSupervisor.start_child(button_id)
    end

    {:ok, args, :hibernate}
  end
end
