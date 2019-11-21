defmodule POST.GPIOButton do
  require Logger
  use GenServer
  alias Circuits.GPIO

  @debounce_timeout_ms 700

  def start_link(pin_number) do
    GenServer.start_link(__MODULE__, pin_number, name: name(pin_number))
  end

  def terminate(reason, state) do
    Logger.warn("GPIOButton #{state.pin_number} crash: #{inspect(reason)}")
  end

  def init(pin_number) do
    Logger.info("GPIOButton #{pin_number} init")
    {:ok, pin} = GPIO.open(pin_number, :input)
    :ok = GPIO.set_interrupts(pin, :rising)
    # this has been checked on v1.3 and v1.5 hardware
    # and it seems to be fine. 
    :ok = GPIO.set_pull_mode(pin, :pulldown)
    {:ok, %{pin_number: pin_number, pin: pin, debounce: nil}}
  end

  def handle_info(:timeout, state) do
    Logger.info("GPIOButton #{state.pin_number} debounce cleared")
    POST.ButtonTable.put({state.pin_number, 0})
    {:noreply, %{state | debounce: nil}}
  end

  def handle_info({:circuits_gpio, pin, _timestamp, _}, %{debounce: timer} = state)
      when is_reference(timer) do
    left = Process.read_timer(timer)
    Logger.info("GPIOButton #{pin} still debounced for #{left} ms")
    {:noreply, state}
  end

  def handle_info({:circuits_gpio, pin, _timestamp, _}, state) do
    Logger.debug("GPIOButton #{pin} triggered")
    POST.ButtonTable.put({pin, 1})
    {:noreply, %{state | debounce: debounce_timer()}}
  end

  def name(pin_number), do: :"#{__MODULE__}.#{pin_number}"

  defp debounce_timer, do: Process.send_after(self(), :timeout, @debounce_timeout_ms)
end
