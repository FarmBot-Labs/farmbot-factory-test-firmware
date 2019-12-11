defmodule POST.TestSuite do
  require Logger
  use GenServer

  def led_on(led) do
    Application.get_env(:post, POST.LEDSupervisor)[:led].on(led)
  end

  def led_off(led) do
    Application.get_env(:post, POST.LEDSupervisor)[:led].off(led)
  end

  def buttons do
    Application.get_env(:post, POST.ButtonPopulator)[:buttons]
  end

  def leds do
    Application.get_env(:post, POST.LEDPopulator)[:leds]
  end

  def motors do
    Application.get_env(:post, __MODULE__)[:motors]
  end

  def peripherals do
    Application.get_env(:post, __MODULE__)[:peripherals]
  end

  def start_link(args, opts \\ [name: __MODULE__]) do
    GenServer.start_link(__MODULE__, args, opts)
  end

  def init(_args) do
    for button <- buttons() do
      POST.ButtonTable.subscribe(button)
    end

    send(self(), :init)
    {:ok, %{state: :init, peripherals: peripherals(), motors: motors(), leds: leds()}}
  end

  def handle_info(:encoder_error, state) do
    # Logger.info "in error state"
    for led <- leds() do
      led_on(led)
    end

    Process.sleep(600)

    for led <- leds() do
      led_off(led)
    end

    Process.send_after(self(), :encoder_error, 600)
    {:noreply, %{state | state: :error}}
  end

  def handle_info(:peripheral_error, state) do
    # Logger.info "in error state"
    for led <- leds() do
      led_on(led)
    end

    Process.sleep(600)

    for led <- leds() do
      led_off(led)
    end

    Process.send_after(self(), :peripheral_error, 600)
    {:noreply, %{state | state: :error}}
  end

  def handle_info(:init, state) do
    Logger.info("starting tests")

    for led <- leds() do
      led_off(led)
    end

    led_on(17)
    send(self(), :maybe_skip_leds)

    {:noreply,
     %{state | state: :leds, leds: leds(), peripherals: peripherals(), motors: motors()}}
  end

  # if there is only led 17 in the list, skip the led test
  def handle_info(:maybe_skip_leds, %{state: :leds, leds: [17]} = state) do
    Logger.info("skipping LED test")
    {:noreply, %{state | state: :peripherals}}
  end

  def handle_info(:maybe_skip_leds, %{state: :leds, leds: [17 | rest]} = state) do
    Logger.info("not skipping LED test")
    {:noreply, %{state | leds: rest}}
  end

  def handle_info({:button, _, _, 1}, state) do
    {:noreply, state}
  end

  def handle_info({:button, _, _, 0}, %{state: :leds, leds: [led]} = state) do
    Logger.info("testing led: #{led} (this is the last one)")
    led_on(led)
    {:noreply, %{state | leds: [], state: :peripherals}}
  end

  def handle_info({:button, _, _, 0}, %{state: :leds, leds: [led | rest]} = state) do
    Logger.info("testing led: #{led}")
    led_on(led)
    {:noreply, %{state | leds: rest}}
  end

  def handle_info({:button, _, _, 0}, %{state: :peripherals, peripherals: [peripheral]} = state) do
    Logger.info("testing peripheral: #{peripheral} (this is the last one)")
    _ = POST.Comms.pin(peripheral)
    {:noreply, %{state | peripherals: [], state: :motors}}
  end

  def handle_info(
        {:button, _, _, 0},
        %{state: :peripherals, peripherals: [peripheral | rest]} = state
      ) do
    case POST.Comms.pin(peripheral) do
      {:ok, load} when load <= 3 ->
        Logger.error("failed testing peripheral: #{peripheral} load: #{load}")
        send(self(), :peripheral_error)

      {:ok, load} when load >= 256 ->
        Logger.error("failed testing peripheral: #{peripheral} load: #{load}")
        send(self(), :peripheral_error)

      {:ok, load} ->
        Logger.info("testing peripheral: #{peripheral} load: #{load}")

      error ->
        Logger.error("failed testing peripheral: #{peripheral} error: #{inspect(error)}")
        send(self(), :peripheral_error)
    end

    {:noreply, %{state | peripherals: rest}}
  end

  def handle_info({:button, _, _, 0}, %{state: :motors, motors: [motor]} = state) do
    Logger.info("testing motor: #{motor} (this is the last one)")

    case POST.Comms.move2(motor, 500) do
      {:ok, steps} when steps >= 1 ->
        send(self(), :init)
        :ok

      {:ok, steps} ->
        send(self(), :encoder_error)
        Logger.error("movement test failed! #{steps}")

      :error ->
        send(self(), :encoder_error)
        Logger.error("movement test failed (no response)")
    end

    {:noreply, %{state | motors: [], state: :complete}}
  end

  def handle_info({:button, _, _, 0}, %{state: :motors, motors: [motor | rest]} = state) do
    Logger.info("testing motor: #{motor}")

    case POST.Comms.move2(motor, 500) do
      {:ok, steps} when steps >= 1 ->
        :ok

      {:ok, steps} ->
        send(self(), :encoder_error)
        Logger.error("movement test failed! #{steps}")

      :error ->
        send(self(), :encoder_error)
        Logger.error("movement test failed (no response)")
    end

    {:noreply, %{state | motors: rest}}
  end
end
