defmodule POST.ButtonSupervisor do
  require Logger
  use DynamicSupervisor

  @button_impl Application.get_env(:post, __MODULE__)[:button]

  @doc "Start a button instance"
  def start_child(button_id) do
    Logger.info("Starting button id=#{button_id}")
    DynamicSupervisor.start_child(__MODULE__, {@button_impl, button_id})
  end

  @doc false
  def start_link(args) do
    DynamicSupervisor.start_link(__MODULE__, args, name: __MODULE__)
  end

  @doc false
  def init(_args) do
    Logger.info("button supervisor init")
    DynamicSupervisor.init(strategy: :one_for_one)
  end
end
