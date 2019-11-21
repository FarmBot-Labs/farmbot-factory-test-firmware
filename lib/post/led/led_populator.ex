defmodule POST.LEDPopulator do
  require Logger
  use GenServer

  def start_link(args, opts \\ [name: __MODULE__]) do
    GenServer.start_link(__MODULE__, args, opts)
  end

  def init(args) do
    Logger.info("populating leds")

    for led_id <- Application.get_env(:post, __MODULE__)[:leds] do
      POST.LEDSupervisor.start_child(led_id)
    end

    {:ok, args, :hibernate}
  end
end
