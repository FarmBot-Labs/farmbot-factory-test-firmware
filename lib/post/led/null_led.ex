defmodule POST.NullLED do
  use GenServer

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
    {:ok, %{pin_number: pin_number, value: 0}}
  end

  def handle_call({:write, value}, _from, state) do
    {:reply, :ok, %{state | value: value}}
  end

  def name(pin_number), do: :"#{__MODULE__}.#{pin_number}"
end
