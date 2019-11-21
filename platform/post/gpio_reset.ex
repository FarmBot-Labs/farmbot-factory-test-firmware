defmodule POST.GPIOReset do
  @moduledoc """
  Uses GPIO pin 19 to reset the uart port
  """
  @behaviour POST.Comms.FlashFirmware.Reset

  use GenServer

  @impl POST.Comms.FlashFirmware.Reset
  def reset do
    GenServer.call(__MODULE__, :reset)
  end

  @doc false
  def start_link(args) do
    GenServer.start_link(__MODULE__, args, name: __MODULE__)
  end

  @impl GenServer
  def init(_) do
    {:ok, gpio} = Circuits.GPIO.open(19, :output)
    {:ok, %{gpio: gpio}}
  end

  @impl GenServer
  def handle_call(:reset, _from, state) do
    with :ok <- Circuits.GPIO.write(state.gpio, 1),
         :ok <- Circuits.GPIO.write(state.gpio, 0) do
      {:reply, :ok, state}
    else
      error -> {:reply, error, state}
    end
  end
end
