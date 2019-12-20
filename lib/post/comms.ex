defmodule POST.Comms do
  @moduledoc """
  handles communicating with the arduino.
  UART connection is not stored, and shoudl be closed after every test
  """

  alias Circuits.UART
  alias POST.Comms.Framing

  @serial_port Application.get_env(:post, __MODULE__)[:device]
  @serial_port ||
    Mix.raise("""
    No serial device configured.
    """)

  @doc "returns the configured serial port"
  def serial_port(), do: @serial_port

  @doc "tests comms"
  def test do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, serial_port(), active: true, speed: 9600, framing: Framing)
    do_sleep_hack()
    data = <<0x0::integer-size(8), 0x0::integer-size(8)>>
    :ok = UART.write(uart, data)
    recv_close(uart)
  end

  def move_enc(axis, steps) do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, serial_port(), active: true, speed: 9600, framing: Framing)
    do_sleep_hack()

    movement =
      <<109::integer-size(8), 0x5::integer-size(8), axis::integer-size(8),
        steps::unsigned-integer-big-size(32)>>

    :ok = UART.write(uart, movement)
    recv(uart)

    test = <<116::integer-size(8), 0x0::integer-size(8)>>
    :ok = UART.write(uart, test)
    recv_close(uart)
  end

  def test2() do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, serial_port(), active: true, speed: 9600, framing: Framing)
    do_sleep_hack()

    test = <<116::integer-size(8), 0x0::integer-size(8)>>
    :ok = UART.write(uart, test)
    recv_close(uart)
  end

  @doc "does a movement on an axis for a number of steps"
  def move(axis, steps) do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, serial_port(), active: true, speed: 9600, framing: Framing)
    do_sleep_hack()

    data =
      <<109::integer-size(8), 0x5::integer-size(8), axis::integer-size(8),
        steps::unsigned-integer-big-size(32)>>

    :ok = UART.write(uart, data)
    recv_close(uart, 10000)
  end

  def move2(axis, steps) do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, serial_port(), active: true, speed: 9600, framing: Framing)
    do_sleep_hack()

    move1 =
      <<109::integer-size(8), 0x5::integer-size(8), axis::integer-size(8),
        steps::unsigned-integer-big-size(32)>>

    move2 =
      <<109::integer-size(8), 0x5::integer-size(8), axis::integer-size(8),
        steps * -1::unsigned-integer-big-size(32)>>

    :ok = UART.write(uart, move1)
    recv(uart, 10000)
    :ok = UART.write(uart, move2)
    recv_close(uart, 10000)
  end

  def move_all(motors, steps) do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, serial_port(), active: true, speed: 9600, framing: Framing)
    do_sleep_hack()

    r =
      motors
      |> Enum.map(fn
        axis ->
          move1 =
            <<109::integer-size(8), 0x5::integer-size(8), axis::integer-size(8),
              steps::unsigned-integer-big-size(32)>>

          move2 =
            <<109::integer-size(8), 0x5::integer-size(8), axis::integer-size(8),
              steps * -1::unsigned-integer-big-size(32)>>

          :ok = UART.write(uart, move1)
          recv(uart, 10000)
          :ok = UART.write(uart, move2)
          recv(uart, 10000)
      end)
      |> Enum.all?(fn
        {:ok, steps} when steps >= 1 ->
          true

        _ ->
          false
      end)

    UART.close(uart)

    r
  end

  @doc "writes a pin number"
  def pin(number) do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, serial_port(), active: true, speed: 9600, framing: Framing)
    do_sleep_hack()
    data = <<112::integer-size(8), 0x1::integer-size(8), number::integer-size(8)>>
    :ok = UART.write(uart, data)
    recv_close(uart)
  end

  def all_pins(pins) do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, serial_port(), active: true, speed: 9600, framing: Framing)
    do_sleep_hack()

    r =
      pins
      |> Enum.map(fn
        number ->
          data = <<112::integer-size(8), 0x1::integer-size(8), number::integer-size(8)>>
          :ok = UART.write(uart, data)
          recv(5000)
      end)
      |> Enum.all?(fn
        {:ok, load} when load <= 3 ->
          false

        {:ok, _} ->
          true

        _ ->
          false
      end)

    UART.close(uart)
    r
  end

  def reset_test do
    {:ok, uart} = UART.start_link()
    :ok = UART.open(uart, serial_port(), active: true, speed: 9600, framing: Framing)
    Circuits.UART.set_dtr(uart, true)
    # Circuits.UART.set_dtr(uart, false)
    # Process.sleep(10)
    do_sleep_hack()
    data = <<0x0::integer-size(8), 0x0::integer-size(8)>>
    :ok = UART.write(uart, data)
    recv_close(uart)
  end

  @doc false
  def do_sleep_hack, do: Process.sleep(1000)

  @doc false
  def recv_close(uart, timeout \\ 5000) do
    receive do
      {_, _, {100, _, data}} ->
        debug_message = IO.iodata_to_binary(data)
        IO.write(:stdio, debug_message)
        recv_close(uart)

      {_, _, {0, 4, <<return::integer-big-size(32)>>}} ->
        IO.inspect(return, label: "RETURN")
        UART.close(uart)
        :ok = GenServer.stop(uart, :normal)
        {:ok, return}
    after
      timeout ->
        IO.puts("timeout waiting for command to complete!")
        UART.close(uart)
        :ok = GenServer.stop(uart, :normal)
        :error
    end
  end

  def recv(uart, timeout \\ 5000) do
    receive do
      {_, _, {100, _, data}} ->
        debug_message = IO.iodata_to_binary(data)
        IO.write(:stdio, debug_message)
        recv(uart)

      {_, _, {0, 4, <<return::integer-big-size(32)>>}} ->
        {:ok, return}
    after
      timeout ->
        IO.puts("timeout waiting for command to complete!")
        UART.close(uart)
        :ok = GenServer.stop(uart, :normal)
        :error
    end
  end
end
