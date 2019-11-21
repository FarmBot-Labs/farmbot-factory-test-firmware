defmodule POST.LEDSupervisor do
  require Logger
  use DynamicSupervisor

  @led_impl Application.get_env(:post, __MODULE__)[:led]

  @doc "Start a led instance"
  def start_child(led_id) do
    Logger.info("Starting led id=#{led_id}")
    DynamicSupervisor.start_child(__MODULE__, {@led_impl, led_id})
  end

  def start_link(args) do
    DynamicSupervisor.start_link(__MODULE__, args, name: __MODULE__)
  end

  def init(_args) do
    DynamicSupervisor.init(strategy: :one_for_one)
  end
end
