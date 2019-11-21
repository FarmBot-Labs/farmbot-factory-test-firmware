defmodule POST.GPIOLED do
  use GenServer
  alias Circuits.GPIO

  def on(pin_number) do
    GenServer.call(name(pin_number), {:write, 1})
  end

  def off(pin_number) do
    GenServer.call(name(pin_number), {:write, 0})
  end

  def start_link(pin_number) do
    GenServer.start_link(__MODULE__, pin_number, name: name(pin_number))
  end

  def init(pin_number) do
    {:ok, pin} = GPIO.open(pin_number, :output, initial_value: 0)
    :ok = GPIO.set_pull_mode(pin, :none)
    {:ok, %{pin_number: pin_number, pin: pin}}
  end

  def handle_call({:write, value}, _from, state) do
    reply = GPIO.write(state.pin, value)
    {:reply, reply, state}
  end

  def name(pin_number), do: :"#{__MODULE__}.#{pin_number}"
end
